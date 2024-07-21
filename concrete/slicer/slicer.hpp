#ifndef __ILRD_RD141_SLICER_HPP__
#define __ILRD_RD141_SLICER_HPP__

#include <cstdlib> // std::size_t
#include <vector> // std::vector
#include <queue> // std::queue
#include <memory> // shared_ptr

#include "../atlas/atlas_header.h" // inner API
#include "../nbd_buse/buse.h" // inner API
#include "../nbd_communication/nbd_communication.hpp"

namespace ilrd_rd141
{

struct DeviceRequest
{
	AtlasHeader m_request;
	int m_deviceFd;
	char *m_data;
}; 

/******************************************************************************/

class Slicer
{
public:
	Slicer(std::shared_ptr<std::vector<int>> fdsVectorPtr,
	std::shared_ptr<NBDCommunication::Request> fullRequest);												
	Slicer(const Slicer& other) = default;
	Slicer& operator=(const Slicer& other) = default;
	~Slicer();
	
	std::vector<std::shared_ptr<DeviceRequest>> Slice();						
	void Merge(int nbd_fd, std::shared_ptr<NBDCommunication::Request> m_fullRequest);
	uint32_t GetRequestUID() const noexcept;
	std::shared_ptr<NBDCommunication::Request> GetBuseRequest() const noexcept;
	bool IsReplyReady() noexcept;
	
private:
	std::shared_ptr<std::vector<int>> m_fdsVectorPtr;							
	std::shared_ptr<NBDCommunication::Request> m_fullRequest;
	size_t m_countSlices;
	uint32_t m_requestUid;
};

/******************************************************************************/

class Sender
{
public:    
	Sender(std::shared_ptr<DeviceRequest> device_ptr);
	Sender(const Sender& other) = delete;
	Sender& operator=(const Sender& other) = delete;
	~Sender() = default;
};

}

#endif /* __ILRD_RD141_SLICER_HPP__ */


