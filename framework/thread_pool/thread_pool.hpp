#ifndef ILRD_RD141_TREAD_POOL_HPP
#define ILRD_RD141_TREAD_POOL_HPP

#include <thread>//hardware_concurrency
#include <memory>//shared_ptr
#include <unordered_map> //unordered_map

#include "callable_function.hpp"//Callable
#include "../waitable_queue/waitable_queue.hpp"//WaitableQueue
#include "../waitable_queue/pq_to_q.hpp"//pq_to_q

namespace ilrd_rd141
{
class ThreadPool
{
public:
    enum Priority{lowest = 0, low, medium, high, highest, numOfPriority};
    
    ThreadPool(int niceness = 0, size_t numOfThreads = std::thread::hardware_concurrency());
    ~ThreadPool();
    ThreadPool(const ThreadPool &rhs) = delete;
    ThreadPool &operator=(const ThreadPool &rhs) = delete;

    void Run();
    void Pause();
    void SetNumOfThreads(size_t num);
    void AddTask(std::shared_ptr<Callable> TaskToDo, Priority priority = medium);
private:
    using value_type = std::pair<std::shared_ptr<Callable>, Priority>;
    struct CheckPriority
    {
        bool operator()(const value_type& lhs, const value_type& rhs) const;
    };
    enum InternalPriority { super_high = numOfPriority, super_duper_high }; //get rid of class
    
    int m_niceness;
    size_t m_numOfThreads;
    size_t m_targetNumOfThreads;
    bool m_isRunning1;    
    bool m_isRunning2;    
    WaitableQueue<value_type,FrontablePQ<value_type, std::vector<value_type>, CheckPriority>> m_thread_queue; //task_queue
    WaitableQueue<std::thread::id> m_zombie_queue;
    std::unordered_map<std::thread::id, std::thread> m_thread_map;
    std::mutex m_mutex;
    std::condition_variable m_cond_var;
    
    void ThreadFunction(ThreadPool &pool);
    static void PauseTask1(ThreadPool *pool);
    static void PauseTask2(ThreadPool *pool);
    static void SuicideTask(ThreadPool *pool);
    void IncreaseNumOfTheads();
    void DecreaseNumOfTheads();
    void CreateThreads(size_t currNumOfThreads, size_t targetNumOfThreads);
};

} // namespace ilrd_rd141

#endif // ILRD_RD141_TREAD_POOL_HPP