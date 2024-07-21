#ifndef __ILRD_RD141_IMONITOR_HPP__
#define __ILRD_RD141_IMONITOR_HPP__

#include <set> 
#include <sys/select.h>

namespace ilrd_rd141
{
class IMonitor
{
public:
    virtual ~IMonitor() = default;
    virtual void RegisterRead(int fd) noexcept = 0;
    virtual bool Deregister(int fd) = 0;
    virtual int WaitForEvents() = 0;
    virtual bool IsReadable(int fd) = 0;
    virtual int GetNextReadEvent(int starting_fd) = 0;
  
};

class SelectMonitor : public IMonitor
{
public:
    SelectMonitor();
    void RegisterRead(int fd) noexcept;
    bool Deregister(int fd);
    int WaitForEvents();
    bool IsReadable(int fd);
    int GetNextReadEvent(int starting_fd);

//    void AdjustMaxFd(int fd);
private:
    fd_set m_fdset[2];
    int m_maxfd;
    std::set<int> m_registered_fds;
};
}

#endif /* __ILRD_RD141_IMONITOR_HPP__ */
