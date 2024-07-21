#include <string.h> /*memcpy*/
#include <mutex>
#include <iostream>

#include "communication_manager.hpp"

static std::mutex g_mutex;

namespace ilrd_rd141
{
Message::~Message()
{
    //empty
}

RaidedMessage::RaidedMessage(std::vector<std::shared_ptr<DeviceRequest>> requestVector)
{
    m_requestVector = requestVector;
}

std::vector<std::shared_ptr<DeviceRequest>> RaidedMessage::GetVector()
{
    return m_requestVector;
}

RaidedMessage::~RaidedMessage()
{
    //empty
}

Splicer::Splicer(std::shared_ptr<std::vector<int>> fdsVectorPtr) : m_fdsVectorPtr(fdsVectorPtr)
{
    //empty
}

std::shared_ptr<Message> Splicer::Request(std::shared_ptr<NBDCommunication::Request> requestPtr)
{
    std::shared_ptr<Slicer> slicerPtr = std::make_shared<Slicer>(m_fdsVectorPtr, requestPtr);
	std::vector<std::shared_ptr<DeviceRequest>> request = slicerPtr->Slice();						

    {
        std::lock_guard<std::mutex> lock(g_mutex);
        m_slicerMap[slicerPtr->GetRequestUID()] = slicerPtr;
    }    
 
    std::shared_ptr<Message> messagePtr = std::make_shared<RaidedMessage>(request);
    
    return messagePtr;
}

void Splicer::Reply(const AtlasHeader &atlas, const char *data)
{
    std::shared_ptr<Slicer> slicer_ptr;
    
    std::lock_guard<std::mutex> lock(g_mutex);
    slicer_ptr = m_slicerMap[atlas.m_requestUid];
    

    memcpy(slicer_ptr->GetBuseRequest()->m_data + (atlas.m_dataLen * atlas.m_fragmentIdx), 
           data,
           atlas.m_dataLen);

    if(true == slicer_ptr->IsReplyReady())
    {
        m_slicerMap.erase(atlas.m_requestUid);
    }    
}

void RequestSender::Send(std::shared_ptr<Message> message)
{
    RaidedMessage *messageToSend = reinterpret_cast<RaidedMessage *>(message.get());
    std::vector<std::shared_ptr<DeviceRequest>> devicePtrs = messageToSend->GetVector();

    for(size_t i = 0; i < devicePtrs.size(); ++i)
    {
        Sender sender((devicePtrs[i]));
    }

}

void RequestSender::SetFdVector(std::vector<int> fds)
{
    m_fds = fds;
}

std::vector<int> RequestSender::GetVector()
{
    return m_fds;
}

CommunicationManager::CommunicationManager(std::shared_ptr<Raid> raidPtr, std::shared_ptr<Communication> communicationPtr)
                                            : m_raidPtr(raidPtr), m_communicationPtr(communicationPtr)
{
    //empty
}

void CommunicationManager::Request(std::shared_ptr<NBDCommunication::Request> requestPtr)
{
    std::shared_ptr<Message> messagePtr = m_raidPtr->Request(requestPtr);
    m_communicationPtr->Send(messagePtr);
}

void CommunicationManager::Reply(const AtlasHeader &atlas, const char *data)
{
    m_raidPtr->Reply(atlas, data);
}

}