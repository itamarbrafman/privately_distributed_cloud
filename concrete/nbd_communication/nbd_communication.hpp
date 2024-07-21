#ifndef ILRD_RD141_NBDCOMMUNICATION_HPP
#define ILRD_RD141_NBDCOMMUNICATION_HPP

#include <cstdint>//uint64_t, uint32_t
#include <string>//string
#include <memory>//shared_ptr
namespace ilrd_rd141
{
class NBDCommunication
{
public:
	enum Command
	{
		READ,
		WRITE,
		DISC,
		FLUSH,
		TRIM
	};
	struct Request
	{
		uint64_t m_from;
		char *m_data;
		int32_t m_len;
		Command m_type;
	};

	NBDCommunication(const std::string &deviceName, size_t deviceSize);
	NBDCommunication(const NBDCommunication& other) = delete;
	NBDCommunication& operator=(const NBDCommunication& other) = delete;
	~NBDCommunication() noexcept;
	
	std::shared_ptr<Request> GetRequest();
	int GetFD() const noexcept;
private:
	int m_nbdSocket;
	void FreeRequest(std::shared_ptr<Request> request);
};
} //namespace ilrd_rd141

#endif //ILRD_RD141_NBDCOMMUNICATION_HPP
