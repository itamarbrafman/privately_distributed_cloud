#include "logger.hpp"
#include <ctime>
#include <iostream>
#include <thread>
#include <unistd.h> // write
#include <iostream>

namespace ilrd_rd141
{

Logger::Logger() : m_is_running(true), m_file(".log")
{
    std::thread workingThread([this]() { ThreadFunction(*this);});
    m_workingThread = std::move(workingThread); 
}

void Logger::ThreadFunction(Logger &logger)
{
    std::pair<std::string, std::string> curr_pop;
    while(m_is_running || !logger.m_queue.IsEmpty())
    {
        logger.m_queue.Pop(curr_pop);
        logger.m_file << curr_pop.first << " " << curr_pop.second << "\n";
    }
}

Logger::~Logger()
{
    m_is_running = false;
    m_workingThread.join();
    m_file.close();
}

void Logger::Log(const std::string &str)
{
    std::time_t currentTime = std::time(nullptr);
    std::tm* timeInfo = std::localtime(&currentTime);
    char formattedTime[20];

    (std::strftime(formattedTime, sizeof(formattedTime), "%d%m%Y:%H:%M:%S", timeInfo));
    
    m_queue.Push(std::make_pair(formattedTime, str));
}


}
