#include <iostream>
#include "scheduler.hpp"
#include <climits>
#include <thread>

using namespace ilrd_rd141;

template class ilrd_rd141::Singleton<ilrd_rd141::Scheduler>;

class callableCount: public Callable
{
    virtual void operator()()
    {
        size_t i = 0;
        while(i < 10)
        {
            std::cout << i << std::endl;
            std::this_thread::sleep_for(std::chrono::seconds(1));
            ++i;
        }
    }
};

class callableLetters: public Callable
{
    virtual void operator()()
    {
        char i = 'a';
        while(i < 106)
        {
            std::cout << i << std::endl;
            std::this_thread::sleep_for(std::chrono::seconds(1));
            ++i;
        }
    }
};

void test1()
{ 
    std::shared_ptr<Callable> task1 =  std::make_shared<callableCount>();
    std::shared_ptr<Callable> task2 =  std::make_shared<callableLetters>();
    std::chrono::nanoseconds time_nano1(4800000000);
    std::chrono::nanoseconds time_nano2(2200000000);
    std::chrono::nanoseconds time_nano3(6200000000);
    Scheduler& instance = Singleton<Scheduler>::getInstance();
    std::cout << "before schedule" << std::endl;

    instance.schedule(task1, time_nano1);
    instance.schedule(task2, time_nano2);
    instance.schedule(task1, time_nano3);

    std::cout << "after schedule" << std::endl;

    std::this_thread::sleep_for(std::chrono::seconds(15));

    std::cout << "test1 done" << std::endl;
}   

int main()
{
    test1();
    return 0;
}