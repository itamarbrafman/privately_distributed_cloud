#include <iostream>

#include "thread_pool.hpp"

#define UNUSED(x) (void)(x)

using namespace ilrd_rd141;

void printNumbers(int unused)
{
    UNUSED(unused);
    size_t number = 1;

    for (size_t i = 0 ; i < 10; ++i)
    {
        std::cout << number << std::endl;

        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
}

void printAlphabet(int unused)
{
    UNUSED(unused);
    char letter = 'A';

    for (size_t i = 0 ; i < 10; ++i)
    {
        std::cout << letter << std::endl;

        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
}

int main()
{
    ThreadPool pool(0, 2);
    std::shared_ptr<Callable> TaskToDo1 = std::make_shared<CallableFunction>(&printNumbers, 0);
    std::shared_ptr<Callable> TaskToDo2 = std::make_shared<CallableFunction>(&printAlphabet, 0);
    pool.AddTask(TaskToDo1, ThreadPool::Priority::high);
    pool.AddTask(TaskToDo2, ThreadPool::Priority::low);    
    pool.AddTask(TaskToDo1, ThreadPool::Priority::high);
    pool.AddTask(TaskToDo2, ThreadPool::Priority::low);
    pool.AddTask(TaskToDo2, ThreadPool::Priority::low);
    pool.AddTask(TaskToDo2, ThreadPool::Priority::low);
    pool.AddTask(TaskToDo2, ThreadPool::Priority::low);
    pool.AddTask(TaskToDo2, ThreadPool::Priority::low);
    pool.AddTask(TaskToDo2, ThreadPool::Priority::low);
    pool.Run();
    std::chrono::seconds timeout(8);
    std::cout << "ONE" << std::endl;
    std::this_thread::sleep_for(timeout);
    std::cout << "TWO" << std::endl;
    pool.SetNumOfThreads(1);
    std::cout << "THREE" << std::endl;

    std::this_thread::sleep_for(timeout);
    pool.SetNumOfThreads(2);
    std::cout << "FOUR" << std::endl;
    std::this_thread::sleep_for(timeout);
    std::cout << "FIVE" << std::endl;
    pool.Pause();
    pool.SetNumOfThreads(3);
    pool.SetNumOfThreads(2);
    std::this_thread::sleep_for(timeout);
    pool.Run();
    std::this_thread::sleep_for(std::chrono::seconds(30));
   
    return 0;
}