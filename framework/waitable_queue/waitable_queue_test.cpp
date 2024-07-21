#include <iostream>
#include <cassert>
#include <thread>
#include <chrono> 

#include "waitable_queue.hpp"
#include "pq_to_q.hpp"

using namespace std;
using namespace ilrd_rd141;

const int NUM_THREADS = 3;

const int NUM_ITEMS = 10;

void ProducerFunction1(WaitableQueue<int, FrontablePQ<int>>& queue) 
{
    for (size_t i = 0; i < NUM_ITEMS; ++i) 
    {
        static int data = 0;
        queue.Push(data++);
    }    
}

void ProducerFunction2(WaitableQueue<int>& queue) 
{
    for (size_t i = 0; i < NUM_ITEMS; ++i) 
    {
        static int data = 0;
        queue.Push(data++);
    }    
}

void ConsumerFunction1(WaitableQueue<int, FrontablePQ<int>>& queue) 
{
    for (int i = 0; i < NUM_ITEMS; ++i) 
    {
        int data;
        queue.Pop(data);
        std::cout << "Consumer consumed data: " << data << std::endl;
    }
}

void ConsumerFunction2(WaitableQueue<int>& queue) 
{
    for (int i = 0; i < NUM_ITEMS; ++i) 
    {
        int data;
        std::chrono::nanoseconds timeout(500);
        bool return_value = queue.Pop(data, timeout);
        //assert(true == return_value);
        std::cout <<"return_value: " << return_value <<  "\tConsumer consumed data: " << data << std::endl;
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
}

void test1()
{
    WaitableQueue<int, FrontablePQ<int>> pq;

    std::vector<std::thread> producers;
    std::vector<std::thread> consumers;
    assert(true == pq.IsEmpty());

    for (int i = 0; i < NUM_THREADS; ++i) 
    {
        producers.emplace_back([&pq]() { ProducerFunction1(pq); });
    }
    //assert(false == pq.IsEmpty());

    for (int i = 0; i < NUM_THREADS; ++i) 
    {
        consumers.emplace_back([&pq]() { ConsumerFunction1(pq);});
    }

    for (size_t i = 0; i < producers.size(); ++i) 
    {
        producers[i].join();
    }

    for (size_t i = 0; i < consumers.size(); ++i) 
    {
        consumers[i].join();
    }

}

void test2()
{
    WaitableQueue<int> queue;

    std::vector<std::thread> producers;
    std::vector<std::thread> consumers;

    for (int i = 0; i < NUM_THREADS; ++i) 
    {
        producers.emplace_back([&queue]() { ProducerFunction2(queue); });

        consumers.emplace_back([&queue]() { ConsumerFunction2(queue);});
    }

    for (size_t i = 0; i < producers.size(); ++i) 
    {
        producers[i].join();
    }

    for (size_t i = 0; i < consumers.size(); ++i) 
    {
        consumers[i].join();
    }

}

int main()
{
    //test1();
    test2();    

    return 0;
}