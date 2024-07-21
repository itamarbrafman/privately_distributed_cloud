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
#include "../../framework/framework/framework.hpp"
#include "../../framework/thread_pool/thread_pool.hpp"

using namespace ilrd_rd141;

const int ERROR = -1;
const size_t MB = 1 << 20;
const size_t SIZE = 512 * MB;
const size_t BUF_SIZE = 1024;

static std::mutex g_requestMutex;
static std::mutex g_replyMutex;

struct OutCommunication
{
    NBDCommunication& nbdCommunication;
    CommunicationManager& communicationManager;

    OutCommunication(NBDCommunication& nc, CommunicationManager& cm)
        : nbdCommunication(nc), communicationManager(cm)
    {
    }
};

struct InCommunication
{
    int socket_fd;
    CommunicationManager& communicationManager;

    InCommunication(int sock_fd, CommunicationManager& cm)
        : socket_fd(sock_fd), communicationManager(cm)
    {
    }
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

static int ReadAll(int fd, char* buf, size_t count);
std::shared_ptr<Callable> ManageRequests(const OutCommunication outCommunication);
std::shared_ptr<Callable> ManageReplies(const InCommunication InCommunication);

/*********************************************************************************************************/
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

/*************************************Handlers (Reactor Callbacks)****************************************/

using RequestWrapperArgs = std::pair<OutCommunication, std::shared_ptr<NBDCommunication::Request>>;
using ReplyWrapperArgs = std::tuple<std::shared_ptr<char>, AtlasHeader, InCommunication>;

static void requestWrapperFunction(const RequestWrapperArgs args)
{
    args.first.communicationManager.Request(args.second);
}

static void replyWrapperFunction(const ReplyWrapperArgs args)
{
    std::get<2>(args).communicationManager.Reply(std::get<1>(args), std::get<0>(args).get());
}

std::shared_ptr<Callable> ManageRequests(const OutCommunication outCommunication)
{
    std::shared_ptr<NBDCommunication::Request> request = outCommunication.nbdCommunication.GetRequest();

    std::shared_ptr<Callable> TaskToDo = std::make_shared<CallableFunction>(&requestWrapperFunction, RequestWrapperArgs(outCommunication, request));

    return TaskToDo;
}

std::shared_ptr<Callable> ManageReplies(const InCommunication inCommunication)
{
    AtlasHeader atlas;
   
    ReadAll(inCommunication.socket_fd, reinterpret_cast<char *>(&atlas), sizeof(atlas));

    std::shared_ptr<char> dataPtr(new char[atlas.m_dataLen], std::default_delete<char[]>());

    if(BUSE_CMD_READ == atlas.m_type)
    {
        ReadAll(inCommunication.socket_fd, dataPtr.get(), atlas.m_dataLen);
    } 

    std::shared_ptr<Callable> TaskToDo = std::make_shared<CallableFunction>(&replyWrapperFunction, ReplyWrapperArgs(dataPtr, atlas, inCommunication));

    return TaskToDo;
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

/********************************************************************************************************/

int main()
{
    const size_t NUM_OF_IOTS = 1;
    const char *device_name = "/dev/nbd0";
    int port_num = 29000;

    std::shared_ptr<std::vector<int>> fdsVectorPtr = std::make_shared<std::vector<int>>();

    NBDCommunication nbd_communicator(device_name, SIZE);

    std::shared_ptr<Raid> raidPtr = std::make_shared<Splicer>(fdsVectorPtr);
    std::shared_ptr<Communication> communicationPtr = std::make_shared<RequestSender>();
    CommunicationManager communicationManager(raidPtr, communicationPtr);
    
    std::list<Framework::CauseAndEffect> toMonitor; 
    for (size_t i = 0; i < NUM_OF_IOTS; ++i)
    {
        IoTManager iot_manager(port_num);
        ++port_num;
        int iot_socket = iot_manager.GetSocketFd();
        fdsVectorPtr->push_back(iot_socket);

        InCommunication inCommunication(iot_socket, communicationManager);
        Function<std::shared_ptr<Callable>(void)> manageRepliesCallback(&ManageReplies, inCommunication);
        Framework::CauseAndEffect to_push = std::make_pair(iot_socket, manageRepliesCallback);
        toMonitor.push_back(to_push);
    }

    OutCommunication outCommunication(nbd_communicator, communicationManager);
    Function<std::shared_ptr<Callable>(void)> manageRequestsCallback(&ManageRequests, outCommunication);
    Framework::CauseAndEffect to_push2 = std::make_pair(nbd_communicator.GetFD(), manageRequestsCallback);
    toMonitor.push_back(to_push2);

    Framework framework(toMonitor, 0);

    framework.Run();

    return 0;
}


