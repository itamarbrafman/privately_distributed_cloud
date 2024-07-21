#include <iostream>
#include <mutex>
//#include <stdlib.h>
//#include <stdio.h>
#include <sys/socket.h> //send
#include "slicer.hpp"
#include "../nbd_buse/buse.h"
#include<mutex>

static std::mutex g_mutex;

namespace ilrd_rd141
{


ilrd_rd141::Slicer::Slicer(std::shared_ptr<std::vector<int>> fdsVectorPtr, std::shared_ptr<NBDCommunication::Request> fullRequest) : m_fdsVectorPtr(fdsVectorPtr), m_fullRequest(fullRequest)
{
    //empty
}

Slicer::~Slicer()
{
    //empty
}

std::vector<std::shared_ptr<DeviceRequest>> ilrd_rd141::Slicer::Slice()
{
    const size_t FRAGMENT_SIZE = 1024;
    std::vector<std::shared_ptr<DeviceRequest>> requests_vector;   
    static size_t request_counter = 0;
    static std::mutex mutexCounter;

    {
        std::lock_guard<std::mutex> lock(mutexCounter);
        m_requestUid = request_counter;
        ++request_counter;
    }    
    
    size_t num_of_fragment = m_fullRequest->m_len / FRAGMENT_SIZE;
    size_t num_of_iots = m_fdsVectorPtr->size();

    size_t first_frag_idx = m_fullRequest->m_from / FRAGMENT_SIZE;
    size_t starting_iot = first_frag_idx % num_of_iots;
    size_t starting_offset = first_frag_idx / num_of_iots * FRAGMENT_SIZE;
    
    size_t curr_iot = starting_iot;

    char *data_addr = m_fullRequest->m_data;
    size_t fragment_counter = 0;
    for(; fragment_counter < num_of_fragment; ++fragment_counter)
    {
        std::shared_ptr<DeviceRequest> device_ptr(new DeviceRequest);

        AtlasHeader *atlasHdr_ptr = &(device_ptr->m_request);
        atlasHdr_ptr->m_iotOffset = starting_offset;
        atlasHdr_ptr->m_dataLen = FRAGMENT_SIZE;
        atlasHdr_ptr->m_requestUid = m_requestUid;   
        atlasHdr_ptr->m_fragmentIdx = fragment_counter; 
        atlasHdr_ptr->m_type = m_fullRequest->m_type;
        atlasHdr_ptr->m_alarmUid = 0;
        device_ptr->m_deviceFd = m_fdsVectorPtr->at(curr_iot);
        device_ptr->m_data = data_addr + fragment_counter * FRAGMENT_SIZE;
        requests_vector.push_back(device_ptr);
        
        ++curr_iot;
        curr_iot %= num_of_iots;
        if(0 == curr_iot)
        {
            starting_offset += FRAGMENT_SIZE;
        }
    }
    
    m_countSlices = fragment_counter;
    return requests_vector;
}

void ilrd_rd141::Slicer::Merge(int nbd_fd, std::shared_ptr<NBDCommunication::Request> request)
{

    int return_value = NbdRequestDone(nbd_fd, reinterpret_cast<buse_request_t *>(request.get()));
    if (0 != return_value)
    {
        std::cerr << "NbdRequestDone failed" << std::endl;
    }
}

uint32_t ilrd_rd141::Slicer::GetRequestUID() const noexcept
{
    return m_requestUid;
}

std::shared_ptr<NBDCommunication::Request> ilrd_rd141::Slicer::GetBuseRequest() const noexcept
{
    return m_fullRequest;
}

bool ilrd_rd141::Slicer::IsReplyReady() noexcept
{
    static std::mutex mutexCounter;
    std::lock_guard<std::mutex> lock(mutexCounter);

    --m_countSlices;
    return (0 == m_countSlices);
}

size_t g_counter = 0;

ilrd_rd141::Sender::Sender(std::shared_ptr<DeviceRequest> device_ptr)
{
    AtlasHeader *m_atlas_ptr = &(device_ptr->m_request);
    switch (m_atlas_ptr->m_type)
    {

        case BUSE_CMD_READ:
        {
            std::lock_guard<std::mutex> lock(g_mutex);

            std::cout << g_counter++ << "\tread request\t" << m_atlas_ptr->m_dataLen << "bytes" << std::endl;
            send(device_ptr->m_deviceFd, m_atlas_ptr, sizeof(*m_atlas_ptr), 0);
            break;
        }

        case BUSE_CMD_WRITE:
        {
            std::lock_guard<std::mutex> lock(g_mutex);

           std::cout << g_counter++ << "\twrite request\t" << m_atlas_ptr->m_dataLen << "bytes" << std::endl;
            send(device_ptr->m_deviceFd, m_atlas_ptr, sizeof(*m_atlas_ptr), 0);
            send(device_ptr->m_deviceFd, device_ptr->m_data, m_atlas_ptr->m_dataLen, 0);

            break;
        }

        default:
        {
            std::cout <<  "NOT WRITE NOR READ \t\t m_type: " << m_atlas_ptr->m_type <<std::endl;
            break;
        }
    }
}

}