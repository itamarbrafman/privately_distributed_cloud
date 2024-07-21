#include "dispatcher.hpp"
#include <iostream>

using namespace ilrd_rd141;
/*
class DispatcherDestructor 
{
public:
    DispatcherDestructor(Dispatcher<>* dispatcher) : m_dispatcher(dispatcher) {}

    void operator()() 
    {
        if (m_dispatcher) 
        {
            std::cout << "Deleting Dispatcher" << std::endl;
            delete m_dispatcher;
            m_dispatcher = nullptr;
        }
    }

private:
    Dispatcher<>* m_dispatcher;
};
*/

void test1()
{
    Dispatcher<int, double> dispatcher;

    Callback<int, double> callback1(
        [](int x, double y) { std::cout << "Callback 1: " << x << ", " << y << std::endl; },
        []() { std::cout << "Callback 1 is notified." << std::endl; }
    );
    callback1.SetDispatcher(dispatcher);

    Callback<int, double> callback2(
        [](int x, double y) { std::cout << "Callback 2: " << x << ", " << y << std::endl; },
        []() { std::cout << "Callback 2 is notified." << std::endl; }
    );
    callback2.SetDispatcher(dispatcher);

    {
        
        Callback<int, double> callback3(
            [](int x, double y) { std::cout << "Callback 3: " << x << ", " << y << std::endl; },
            []() { std::cout << "Callback 3 is notified." << std::endl; }
        );
        callback3.SetDispatcher(dispatcher);
        
        dispatcher.Notify(42, 3.14);
    }
        dispatcher.Notify(20, 5.55);

}

/*
void test2()
{
    Dispatcher<>* dispatcher = new Dispatcher<>;
    DispatcherDestructor killer(dispatcher);

    Callback<> callback1(
        *dispatcher,
        [&killer]() { killer(); },
        []() { std::cout << "Callback 1 is notified." << std::endl; }
    );

    dispatcher->Notify();
}
*/


int main()
{
    test1();

    return 0;

}