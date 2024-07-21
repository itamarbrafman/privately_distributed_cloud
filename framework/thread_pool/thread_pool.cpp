#include <unistd.h> //nice
#include <iostream>

#include "thread_pool.hpp"

using namespace ilrd_rd141;

class MyException : public std::exception 
{
public:
    MyException(const char* message) : message_(message) {}

    const char* what() const noexcept override 
    {
        return message_.c_str();
    }

private:
    std::string message_;
};

bool ThreadPool::CheckPriority:: operator()(const value_type& lhs, const value_type& rhs) const
{
    return lhs.second < rhs.second;
}

ThreadPool::ThreadPool(int niceness, size_t numOfThreads) 
                        : m_niceness(niceness), m_numOfThreads(numOfThreads), 
                        m_targetNumOfThreads(numOfThreads), m_isRunning1(false), m_isRunning2(false)
{
    Pause();
    CreateThreads(0, m_numOfThreads);
}

void ThreadPool::Run()
{
    const std::lock_guard<std::mutex> lock(m_mutex);
    m_isRunning1 = true;
    m_isRunning2 = true;
    m_mutex.unlock();
    m_cond_var.notify_all();
}

void ThreadPool::Pause()
{
    const std::lock_guard<std::mutex> lock(m_mutex);
    m_isRunning1 = false;
    m_mutex.unlock();
    size_t i = (m_numOfThreads < m_targetNumOfThreads) ? m_numOfThreads : 0;
    std::shared_ptr<Callable> pause_task = std::make_shared<CallableFunction>(&PauseTask1, this);
    for (; i < m_targetNumOfThreads; ++i)
    {
        m_thread_queue.Push(std::make_pair(pause_task, static_cast<Priority>
                                                                  (InternalPriority::super_high)));
    }
}

void ThreadPool::SetNumOfThreads(size_t num)
{   
    m_targetNumOfThreads = num;
    m_targetNumOfThreads > m_numOfThreads ? IncreaseNumOfTheads() : DecreaseNumOfTheads();
    m_numOfThreads = m_targetNumOfThreads;
}

void ThreadPool::AddTask(std::shared_ptr<Callable> TaskToDo, Priority priorty)
{
    m_thread_queue.Push(std::make_pair(TaskToDo, priorty));
}

void ThreadPool::ThreadFunction(ThreadPool &/*pool*/)
{
    if (-1 == nice(m_niceness)) 
    {
        perror("nice"); //in future add to logger
    }
    
    while(true)
    {
        std::pair<std::shared_ptr<Callable>, Priority> outparam;
        m_thread_queue.Pop(outparam);
        try //move this elsewhere
        {
            (*outparam.first)();
        }
        catch(const std::exception& e)
        {
            return;
        }
    }
}

void ThreadPool::PauseTask1(ThreadPool *pool)
{
    std::unique_lock<std::mutex> lock(pool->m_mutex);
    while(false == pool->m_isRunning1)
    {
        pool->m_cond_var.wait(lock);
    }
    
}

void ilrd_rd141::ThreadPool::PauseTask2(ThreadPool *pool)
{
    std::unique_lock<std::mutex> lock(pool->m_mutex);
    while(false == pool->m_isRunning2)
    {
        pool->m_cond_var.wait(lock);
    }
}

void ThreadPool::SuicideTask(ThreadPool *pool)
{
    pool->m_zombie_queue.Push(std::this_thread::get_id());
    throw MyException("exterminate_exceptions"); //change to something ie terminate
}


void ThreadPool::IncreaseNumOfTheads()
{
    if(false == m_isRunning1)
    {
        Pause();
    }

    CreateThreads(m_numOfThreads, m_targetNumOfThreads); //m_numOfThreads could also be size of thread_map
}

void ThreadPool::CreateThreads(size_t currNumOfThreads, size_t targetNumOfThreads) //can tell only one parameters, num of threads to create
{
    for (size_t i = currNumOfThreads ; i < targetNumOfThreads; ++i)
    {
        std::thread newThread([this]() { ThreadFunction(*this); });
        std::thread::id threadId = newThread.get_id();
        m_thread_map[threadId] = std::move(newThread);     
    }
}

void ThreadPool::DecreaseNumOfTheads()
{
    size_t thread_to_kill = m_numOfThreads - m_targetNumOfThreads;

    std::shared_ptr<Callable> suicide_task = std::make_shared<CallableFunction>(&ThreadPool::SuicideTask, this);
    for (size_t i = 0; i < thread_to_kill; ++i)
    {
        m_thread_queue.Push(std::make_pair(suicide_task, static_cast<ThreadPool::Priority>(ThreadPool::InternalPriority::super_duper_high)));
    }

    if(false == m_isRunning1)
    {
        const std::lock_guard<std::mutex> lock(m_mutex);
        m_isRunning2 = false;
        m_mutex.unlock();   

        std::shared_ptr<Callable> pause_task2 = std::make_shared<CallableFunction>(PauseTask2, this);
        for(size_t i = thread_to_kill; i < m_targetNumOfThreads; ++i)
        {
            AddTask(pause_task2, Priority(InternalPriority::super_high));
        }

        m_mutex.lock();
        m_isRunning1 = true;
        m_mutex.unlock();
        m_cond_var.notify_all();
    }
    else if(false == m_isRunning2) //code duplicated almost
    {
        m_mutex.lock();
        m_isRunning1 = false;
        m_mutex.unlock();
        std::shared_ptr<Callable> pause_task1 = std::make_shared<CallableFunction>(PauseTask1, this);
        for(size_t i = thread_to_kill; i < m_targetNumOfThreads; i++)
        {
            AddTask(pause_task1, Priority(InternalPriority::super_high));
        }
        m_mutex.lock();
        m_isRunning2 = true;
        m_mutex.unlock();

        m_cond_var.notify_all();
    }

    for (size_t i = 0; i < thread_to_kill; ++i)
    {
        std::thread::id outparam;

        m_zombie_queue.Pop(outparam);
        std::thread& to_join = m_thread_map.at(outparam);
        to_join.join();
        m_thread_map.erase(outparam);
    }
}

ThreadPool::~ThreadPool()
{
    SetNumOfThreads(0);
}