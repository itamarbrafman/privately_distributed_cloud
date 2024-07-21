#include <iostream>
#include <unistd.h> /*STDIN_FILENO*/
#include <stdlib.h>
#include <stdio.h>
#include <linux/nbd.h>
#include <string.h>
#include <unordered_map>
#include <memory> /*shared_ptr*/

#include <arpa/inet.h> //struct sockaddr_in

#include "../../framework/reactor/imonitor.hpp"
#include "../../framework/reactor/reactor.hpp"

#include "../nbd_buse/buse.h"
#include "../slicer/slicer.hpp"
#include "../nbd_communication/nbd_communication.hpp"
#include "../communication_manager/communication_manager.hpp"

using namespace ilrd_rd141;

const int ERROR = -1;
const size_t MB = 1 << 20;
const size_t SIZE = 512 * MB;
const size_t BUF_SIZE = 1024;

static int ReadAll(int fd, char* buf, size_t count);

std::unordered_map<uint32_t, std::shared_ptr<Slicer>> g_slicerMap;
int g_nbd_fd;

class NBDManager
{
public:
    NBDManager(const char *device_name, int size);
    ~NBDManager();
    void ManageRequests(std::pair<NBDCommunication&, std::vector<std::pair<int, size_t>>> pair);
    int Getnbd();
    void SetFdsVector(std::vector<std::pair<int, size_t>> fds);
    
private:
    const char *m_device_name;
    int m_nbd;
    char *m_IoT_buffer;
    int m_counter;
    std::vector<std::pair<int, size_t>> m_fds;
};

class IoTManager
{
public:
    IoTManager(int port_num);
    ~IoTManager();
    int GetSocketFd();

private:
    int m_socket_fd;
};

NBDManager::NBDManager(const char *device_name, int size) : m_device_name(device_name), m_nbd(0), m_counter(0)
{
}

NBDManager::~NBDManager()
{
    close(m_nbd);
    delete[] m_IoT_buffer;
}

void NBDManager::ManageRequests(std::pair<NBDCommunication&, std::vector<std::pair<int, size_t>>> pair)
{
    m_fds = pair.second;

    std::shared_ptr<Raid> raidPtr = std::make_shared<Splicer>()
    CommunicationManager communicationManager(std::shared_ptr<Raid> raidPtr, std::shared_ptr<Communication> communicationPtr);


    std::shared_ptr<Slicer> slicerPtr = std::make_shared<Slicer>(m_fds, pair.first.GetRequest());
           
    std::vector<std::shared_ptr<DeviceRequest>> device_ptrs = slicerPtr->Slice();
    g_slicerMap[slicerPtr->GetRequestUID()] = slicerPtr;

    for(size_t i = 0; i < device_ptrs.size(); ++i)
    {
        Sender sender((device_ptrs[i]));
    }
}

int NBDManager::Getnbd()
{
    return m_nbd;
}

void NBDManager::SetFdsVector(std::vector<std::pair<int, size_t>> fds)
{
    m_fds = fds;
}

IoTManager::IoTManager(int port_num)
{
    struct sockaddr_in serverAddr;
    int clientSock;

    clientSock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (-1 == clientSock)
    {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(port_num);
    serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1");
    if (-1 == connect(clientSock, reinterpret_cast<struct sockaddr *>(&serverAddr), sizeof(serverAddr)))
    {
        perror("connect");
        exit(EXIT_FAILURE);
    }

    m_socket_fd = clientSock;
}

IoTManager::~IoTManager()
{
    //empty
}

int IoTManager::GetSocketFd()
{
    return m_socket_fd;
}

static int ReadAll(int fd, char* buf, size_t count)
{
    int bytes_read;

    while (count > 0)
    {
        bytes_read = recv(fd, buf, count, 0);

        if (bytes_read <= 0)
        {
            return -1;
        }

        buf += bytes_read;
        count -= bytes_read;
    }

    return 0;
}

void ManageReplies(std::pair<NBDCommunication&, std::pair<int, size_t>> pair)
{
    AtlasHeader atlas;
    AtlasHeader *atlas_ptr = &atlas;
   
    ReadAll(pair.second.first, reinterpret_cast<char *>(atlas_ptr), sizeof(atlas));

    std::shared_ptr<Slicer> slicer_ptr = g_slicerMap[atlas_ptr->m_requestUid];
    if(BUSE_CMD_READ == atlas_ptr->m_type)
    {
        ReadAll(pair.second.first, 
                slicer_ptr->GetBuseRequest()->m_data + 
                (atlas_ptr->m_dataLen * atlas_ptr->m_fragmentIdx),
                atlas_ptr->m_dataLen);

    }    

    if(true == slicer_ptr->IsReplyReady())
    {
        g_slicerMap.erase(atlas_ptr->m_requestUid);
    } 
}

int main()
{
    const size_t NUM_OF_IOTS = 1;
    const char *device_name = "/dev/nbd1";
    int port_num = 29000;

    std::vector<std::pair<int, size_t>> fds;
    Reactor<SelectMonitor> r1(new SelectMonitor);
    std::vector<Function<void(void)>> functionVector;
    
    NBDCommunication nbd_communicator(device_name, SIZE);
    
    for (size_t i = 0; i < NUM_OF_IOTS; ++i)
    {
        IoTManager iot_manager(port_num);
        ++port_num;
        int iot_socket = iot_manager.GetSocketFd();
        std::pair<int, size_t> pair = {iot_socket, SIZE};
        fds.push_back(pair);
        std::pair<NBDCommunication&, std::pair<int, size_t>> pair_for_replies = {nbd_communicator, pair};
        Function<void(void)> f(&ManageReplies, pair_for_replies);
        functionVector.push_back(f);
        r1.RegisterReadEventHandler(iot_socket, functionVector.at(i));    
    }
 
    char *IoT_buffer = new char[BUF_SIZE];
    if (NULL == IoT_buffer)
    {
        std::cerr << "Memory allocation failed" << std::endl;
        exit(1);
    }

    NBDManager nbd_manager(device_name, SIZE);
    g_nbd_fd = nbd_communicator.GetFD();
    std::pair<NBDCommunication&, std::vector<std::pair<int, size_t>>> pair_to_send = {nbd_communicator, fds};
    Function<void(void)> f1(&NBDManager::ManageRequests, &nbd_manager, pair_to_send);

    r1.RegisterReadEventHandler(g_nbd_fd, f1);

    r1.Run();

    delete[] IoT_buffer;

    return 0;
}


