#define _POSIX_C_SOURCE >= 199309L

#include "scheduler.hpp"
#include <signal.h>  //struct sigevent

namespace ilrd_rd141
{ 

class wrappedCallableTask
{
public:
    wrappedCallableTask(std::shared_ptr<Callable> task) : m_task(task){}

    void operator()()
    {
        (*m_task)();
    }

private:
    std::shared_ptr<Callable> m_task; 

};

static void TimerCallback(union sigval sv) 
{
    wrappedCallableTask *task = static_cast<wrappedCallableTask *>(sv.sival_ptr);
    (*task)();
    delete task;
}


Scheduler::Scheduler()
{
    //empty
}

Scheduler::~Scheduler()
{
    for (timer_t timerId : m_timerIds) 
    {
        std::cout << "timer deleted" << std::endl;
        timer_delete(timerId);
    }
}

void Scheduler::schedule(std::shared_ptr<Callable> task, std::chrono::nanoseconds time_nano)
{
        std::cout << "schedule func called" << std::endl;

    timer_t timerid;
    struct sigevent sev;

    sev.sigev_signo = 0; 
    sev.sigev_value.sival_int = 0; 
    sev.sigev_notify_attributes = nullptr;


    wrappedCallableTask *task_ptr = new wrappedCallableTask(task);

    sev.sigev_notify = SIGEV_THREAD;
    sev.sigev_notify_function = TimerCallback;
    sev.sigev_value.sival_ptr = task_ptr;

    if (-1 == timer_create(CLOCK_REALTIME, &sev, &timerid)) 
    {
        throw std::runtime_error("timer_create failed");    
    }

    struct itimerspec its;
    its.it_interval.tv_sec = 0;  
    its.it_interval.tv_nsec = 0;
    its.it_value.tv_sec = std::chrono::duration_cast<std::chrono::seconds>(time_nano).count();   
    its.it_value.tv_nsec = time_nano.count() - std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::seconds(its.it_value.tv_sec)).count();

    if (-1 == timer_settime(timerid, 0, &its, NULL)) 
    {
        throw std::runtime_error("timer_settime failed");    
    }  

    m_timerIds.push_back(timerid);
}



}//namespace ilrd_rd141