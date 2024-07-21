#ifndef __ILRD_RD141_REACTOR_HPP__
#define __ILRD_RD141_REACTOR_HPP__

#include <unordered_map> // std::unordered_map
#include <stdexcept>

#include "../../../../cpp/src/function/function.hpp" // Function<void(void)> callback API

namespace ilrd_rd141
{
using Handler = ilrd_rd141::Function<void(void)>;
template<typename Monitor> 
class Reactor
{
public:
    Reactor(Monitor *monitor_sptr);
    Reactor(const Reactor &) = delete;
    Reactor & operator=(const Reactor &) = delete;
    ~Reactor();
    void RegisterReadEventHandler(int fd, const Handler &call_back);
    void DeregisterEventHandler(int fd) noexcept;
    void Run(void);

private:
    std::shared_ptr<Monitor> m_monitor;
    std::unordered_map<int, Handler> m_call_backs;
};

/***************************************class exception*******************************************************/
class SelectFailedException : public std::runtime_error
{
public:
    SelectFailedException(const std::string& message) : std::runtime_error(message) {}
};
/*******************************************************************************************************/
template <typename Monitor>
Reactor<Monitor>::Reactor(Monitor *monitor_sptr) : m_monitor(monitor_sptr), m_call_backs()
{
    //empty
}

template <typename Monitor>
Reactor<Monitor>::~Reactor()
{
    //empty
}

template <typename Monitor>
void Reactor<Monitor>::RegisterReadEventHandler(int fd, const Handler &call_back)
{
    m_monitor->Monitor::RegisterRead(fd);
    m_call_backs.insert({fd, call_back}); 
}

template <typename Monitor>
void Reactor<Monitor>::Run(void)
{
    while (1)
    {
        int ready_fds = m_monitor->Monitor::WaitForEvents();
        if (-1 == ready_fds)
        {
            throw SelectFailedException("select failed");
        }

        int starting_fd = 0;
        while (ready_fds > 0)
        {
            int fd_to_read = m_monitor->Monitor::GetNextReadEvent(starting_fd);
            m_call_backs.at(fd_to_read)();
            --ready_fds;
            starting_fd = fd_to_read + 1;
        }
    }
}

template <typename Monitor>
void Reactor<Monitor>::DeregisterEventHandler(int fd) noexcept
{
    m_monitor->Monitor::Deregister(fd);
    m_call_backs.erase(fd);
}

}
#endif /* __ILRD_RD141_REACTOR_H__ */