#include <stdexcept> //runtime_error
#include <unistd.h> //close
#include <iostream> 
#include <mutex> 

#include "nbd_communication.hpp"
#include "../nbd_buse/buse.h"

const int ERROR = -1;
std::mutex g_mutex;

namespace ilrd_rd141
{

NBDCommunication::NBDCommunication(const std::string &folderName, size_t folderSize)
{
    m_nbdSocket = NbdOpen(folderName.c_str(), folderSize);
    if (ERROR == m_nbdSocket)
    {   
        throw std::runtime_error("NbdOpen failed");
    }
}

NBDCommunication::~NBDCommunication() noexcept
{
    close(m_nbdSocket);
}

static auto FreeRequestDeleter = [](NBDCommunication* obj, buse_request_t* request)
{
    std::lock_guard<std::mutex> lock(g_mutex);
    NbdRequestDone(obj->GetFD(), request);
};

std::shared_ptr<NBDCommunication::Request> NBDCommunication::GetRequest()
{
    buse_request_t *curr_request = NbdGetRequest(m_nbdSocket);

    std::shared_ptr<NBDCommunication::Request> request_ptr(
        reinterpret_cast<NBDCommunication::Request *>(curr_request),
        [this](NBDCommunication::Request* request)
        {
            FreeRequestDeleter(this, reinterpret_cast<buse_request_t *>(request));
        });

    return request_ptr;
}

int NBDCommunication::GetFD() const noexcept
{
    return m_nbdSocket;
}

}
