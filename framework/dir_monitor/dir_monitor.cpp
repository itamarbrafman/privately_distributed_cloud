#include <unistd.h> //close
#include <sys/inotify.h> //inotify_init
#include <stdexcept> //std::runtime_error
#include <cstring> //memcpy
#include <limits.h> //NAME_MAX
#include <fcntl.h> //O_NONBLOCK

#include "dir_monitor.hpp"

void ilrd_rd141::DirMonitor::MonitorThread()
{
    while (m_running) 
    {
        char buffer[sizeof(struct inotify_event) + NAME_MAX + 1];
        ssize_t numBytes = read(m_fd, buffer, sizeof(buffer));
        if (-1 == numBytes) 
        {
            continue;
        }

        struct inotify_event *event = reinterpret_cast<struct inotify_event*>(buffer);

        switch (event->mask) 
        {
            case IN_CREATE:
                m_creation_dispatcher.Notify(m_path + '/' + event->name); 
                break;
            case IN_DELETE:
                m_deletion_dispatcher.Notify(m_path + '/' + event->name);
                break;
            case IN_CLOSE_WRITE:
                m_motifier_dispatcher.Notify(m_path + '/' + event->name);
                break;
            default:
                throw std::runtime_error("Invalid event type");
        }
    }
}

ilrd_rd141::DirMonitor::DirMonitor(const std::string &path) : m_path(path),
                                                              m_fd(inotify_init1(O_NONBLOCK)),
                                                              m_running(true)
{
    if (-1 == m_fd)
    {
        throw std::runtime_error("inotify_init Error");
    }

    int watched = inotify_add_watch(m_fd, path.c_str(), IN_CREATE | IN_DELETE | IN_CLOSE_WRITE);
    if (-1 == watched) 
    {
        close(m_fd);
        throw std::runtime_error("inotify_add_watch Error");
    }

    m_monitorThread = std::thread([this]() { MonitorThread();});
}


ilrd_rd141::DirMonitor::~DirMonitor() noexcept
{
    close(m_fd);
    m_running = false;
    m_monitorThread.join();
}

void ilrd_rd141::DirMonitor::Subscribe(Callback<std::string> &toDoOnNotify, WhatToMonitor flag)
{
    switch (flag) 
    {
        case creation:
            toDoOnNotify.SetDispatcher(m_creation_dispatcher);
            break;
        case deletion:
            toDoOnNotify.SetDispatcher(m_deletion_dispatcher);
            break;
        case modify:
            toDoOnNotify.SetDispatcher(m_motifier_dispatcher);
            break;
        default:
            throw std::runtime_error("Invalid event type");
    }
}    
