#ifndef ILRD_RD141_DIRMONITOR_HPP
#define ILRD_RD141_DIRMONITOR_HPP

#include <string>//string
#include <thread> //std::thread

#include "../dispatcher/dispatcher.hpp" //Dispatcher

namespace ilrd_rd141
{
class DirMonitor
{
public:
    enum WhatToMonitor{creation, deletion, modify};

    explicit DirMonitor(const std::string &path);
    ~DirMonitor() noexcept;
    DirMonitor(const DirMonitor &other) = delete;
    DirMonitor(DirMonitor &&other) = delete;
    DirMonitor &operator=(const DirMonitor &other) = delete;
    DirMonitor &operator=(DirMonitor &&other) = delete;

    void Subscribe(Callback<std::string> &toDoOnNotify, WhatToMonitor flag);
private:
    void MonitorThread();
    std::thread m_monitorThread;
    Dispatcher<std::string> m_creation_dispatcher;
    Dispatcher<std::string> m_deletion_dispatcher;
    Dispatcher<std::string> m_motifier_dispatcher;   
    std::string m_path;
    int m_fd;
    bool m_running;
};

} // ilrd_rd141

#endif /* ILRD_RD141_DIRMONITOR_HPP*/
