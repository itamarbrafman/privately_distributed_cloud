#ifndef ILRD_RD141_FRAMEWORK_HPP
#define ILRD_RD141_FRAMEWORK_HPP

#include <list>//list, pair
#include <thread>
//#include <functional>//function
#include "../thread_pool/thread_pool.hpp"
#include "../reactor/reactor.hpp"//Reactor
#include "../reactor/imonitor.hpp"

#include "../dir_monitor/dir_monitor.hpp"
#include "../dll_loader/dll_loader.hpp"
#include "../factory/factory.hpp"

namespace ilrd_rd141
{

enum CommandType 
{
    READ,
    WRITE
};

class Command
{
public:
    Command(CommandType type);
    virtual void operator()() = 0;  

private:
    CommandType m_type;
};

extern template class Singleton<Factory::Factory<Command, CommandType, CommandType>>;

struct TaskAndKey 
{
    std::shared_ptr<Callable> taskToDo;    
    CommandType type;    
};

class Framework
{
public:
    using fileDescriptor = int;
    //T will be the key and the argument for the command factory
    using CauseAndEffect = 
    std::pair<fileDescriptor, ilrd_rd141::Function<std::shared_ptr<Callable>(void)>>;

    Framework(const std::list<CauseAndEffect> &toMonitor, int niceness = 0, 
    size_t numOfThreads = std::thread::hardware_concurrency());
    ~Framework() = default;
    Framework(const Framework &other) = delete;
    Framework &operator=(const Framework &other) = delete;
    Framework(Framework &&other) = default;
    Framework &operator=(Framework &&other) = default;

    void Run();
    void Add(const CauseAndEffect &toMonitor);
    void Remove(fileDescriptor fd) noexcept;

private:
    Reactor<SelectMonitor> m_reactor;
    ThreadPool m_threadPool;
    void AddTask(Function<std::shared_ptr<Callable>(void)> handler);
    size_t m_numOfThreads;
    int m_niceness; 

};

} // ilrd_rd141

#endif /* ILRD_RD141_FRAMEWORK_HPP*/
