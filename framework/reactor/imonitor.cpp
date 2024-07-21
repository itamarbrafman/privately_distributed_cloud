    #define _POSIX_C_SOURCE (200112L)
    
    #include <sys/time.h> /*fd_set*/ /*struct timeval*/
    #include <stdlib.h> /*exit*/
    #include <stdio.h> /*printf*/

    #include "../../../../cpp/src/function/function.hpp" // Function<void(void)> callback API
    #include "imonitor.hpp"

namespace ilrd_rd141
{
    SelectMonitor::SelectMonitor(): m_maxfd(0)
    {
        FD_ZERO(&m_fdset[0]);
        FD_ZERO(&m_fdset[1]);
    }

    void SelectMonitor::RegisterRead(int fd) noexcept
    {
        FD_SET(fd, &m_fdset[0]);
        m_registered_fds.insert(fd);
        m_maxfd = *(m_registered_fds.rbegin());;
    }

    bool SelectMonitor::Deregister(int fd)
    {
        FD_CLR(fd, &m_fdset[0]);
        return true;
    }

    int SelectMonitor::WaitForEvents()
    {
        m_fdset[1] = m_fdset[0];
        int ready_fds = select(m_maxfd + 1, &m_fdset[1], NULL, NULL, NULL);

        return ready_fds;
    }

    bool SelectMonitor::IsReadable(int fd)
    {
        return FD_ISSET(fd, &m_fdset[1]);
    }

    int SelectMonitor::GetNextReadEvent(int starting_fd)
    {
        for (; starting_fd < FD_SETSIZE; ++starting_fd)
        {
            if(IsReadable(starting_fd))
            {
                return starting_fd;
            }
        }

        return -1;
    }
} //namespace    
/*
    void SelectMonitor::AdjustMaxFd(int fd)
    {
        auto it = m_registered_fds.find(target);

        if (it != m_registered_fds.end())
            m_maxfd = Max2(m_maxfd, m_fdset[0]);
        }

    }

    int Max2(int a, int b) 
    {
        return (a > b) ? a : b;
    }
*/

