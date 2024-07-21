#ifndef ILRD_RD141_RAIDMANAGER_HPP
#define ILRD_RD141_RAIDMANAGER_HPP

#include <memory>//shared_ptr
#include <unordered_map>//unordered_map
#include "../nbd_communication/nbd_communication.hpp"
#include "../atlas/atlas_header.h"
#include "../slicer/slicer.hpp"

namespace ilrd_rd141
{

class Message
{
public:
    virtual ~Message() = 0;    
};

class Raid
{
public:
	virtual std::shared_ptr<Message> Request(std::shared_ptr<NBDCommunication::Request> requestPtr) = 0;
	virtual void Reply(const AtlasHeader &atlas, const char *data = nullptr) = 0;
};

class Communication
{
public:
	virtual void Send(std::shared_ptr<Message> message) = 0;
};

class CommunicationManager
{
public:
	CommunicationManager(std::shared_ptr<Raid> raidPtr, std::shared_ptr<Communication> communicationPtr);
	void Request(std::shared_ptr<NBDCommunication::Request> requestPtr);
	void Reply(const AtlasHeader &atlas, const char *data = nullptr);

private:
	std::shared_ptr<Raid> m_raidPtr;
	std::shared_ptr<Communication> m_communicationPtr;
};

/********************************************DERIVED_CLASSES************************************************/

class RaidedMessage : public Message
{
public:
    RaidedMessage(std::vector<std::shared_ptr<DeviceRequest>> requestVector);
    std::vector<std::shared_ptr<DeviceRequest>> GetVector();
	virtual ~RaidedMessage();

private:
    std::vector<std::shared_ptr<DeviceRequest>> m_requestVector;    
};

class Splicer: public Raid
{
public:
    Splicer(std::shared_ptr<std::vector<int>> fdsVectorPtr);
	virtual std::shared_ptr<Message> Request(std::shared_ptr<NBDCommunication::Request> requestPtr);
	virtual void Reply(const AtlasHeader &atlas, const char *data = nullptr);
private:
	std::shared_ptr<std::vector<int>> m_fdsVectorPtr;
	std::unordered_map<uint32_t, std::shared_ptr<Slicer>> m_slicerMap;       
};

class RequestSender : public Communication
{
public:
	virtual void Send(std::shared_ptr<Message> message);
	void SetFdVector(std::vector<int> fds);
	std::vector<int> GetVector();
private:
	std::vector<int> m_fds;
};

} //namespace ilrd_rd141

#endif //ILRD_RD141_RAIDMANAGER_HPP
