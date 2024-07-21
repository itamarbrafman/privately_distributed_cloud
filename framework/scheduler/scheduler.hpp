#ifndef ILRD_RD141_SCHEDULER_HPP
#define ILRD_RD141_SCHEDULER_HPP

#include <queue>
#include <memory>
#include <chrono>
#include "../thread_pool/callable.hpp"
#include "../singleton/singleton.hpp"

namespace ilrd_rd141
{    


class Scheduler 
{
public:
    
    ~Scheduler();
    Scheduler(const Scheduler& other) = delete;
    Scheduler& operator = (const Scheduler& other) = delete;
    Scheduler(Scheduler &&other) = delete;
    Scheduler &operator=(Scheduler &&other) = delete;

    void schedule(std::shared_ptr<Callable> task, std::chrono::nanoseconds time_nano);

private:

    Scheduler();
    friend Singleton<Scheduler>;
    std::vector<timer_t> m_timerIds;

};
}

extern template class ilrd_rd141::Singleton<ilrd_rd141::Scheduler>;

#endif  // ILRD_RD141_SCHEDULER_HPP