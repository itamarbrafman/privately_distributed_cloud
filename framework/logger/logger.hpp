#ifndef __ILRD_RD141_LOGGER_HPP__
#define __ILRD_RD141_LOGGER_HPP__

#include <fstream>
#include <thread>
#include "../singleton/singleton.hpp"//my own implementation
#include "../waitable_queue/waitable_queue.hpp"
#include "../waitable_queue/pq_to_q.hpp"

namespace ilrd_rd141
{

class Logger {
public:
    Logger(const Logger&) = delete;
    Logger& operator=(const Logger&) = delete;
    Logger(Logger &&other) = delete;
    Logger &operator=(Logger &&other) = delete;
    ~Logger();
    
    void Log(const std::string& str);     

private:
    Logger();
    friend Singleton<Logger>;
    using value_type = std::pair<std::string, std::string>;

    WaitableQueue<value_type,FrontablePQ<value_type>> m_queue; 
    void ThreadFunction(Logger &logger);
    bool m_is_running;
    std::ofstream m_file;
    std::thread& m_workingThread;
};
}

/*extern*/ template class ilrd_rd141::Singleton<ilrd_rd141::Logger>;

#endif  // ILRD_RD141_LOGGER_HPP


