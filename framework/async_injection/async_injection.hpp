#ifndef ILRD_RD141_ASYNC_INJECTION_HPP
#define ILRD_RD141_ASYNC_INJECTION_HPP

#include <chrono>//nanoseconds
#include <functional>//function<bool()>
#include "../thread_pool/callable.hpp"
#include <memory>

namespace ilrd_rd141
{ 

class Asyncinjection
{

public:
    Asyncinjection(const Asyncinjection& other) = delete;
    Asyncinjection& operator = (const Asyncinjection& other) = delete;
    Asyncinjection(Asyncinjection &&other) = delete;
    Asyncinjection &operator=(Asyncinjection &&other) = delete;

    static void CreateInstance(std::function<bool()> functor, std::chrono::nanoseconds interval);  

private:   
    ~Asyncinjection() = default;
    Asyncinjection(std::function<bool()> functor, std::chrono::nanoseconds interval);
    static void Execute(Asyncinjection *obj);

    std::function<bool()> m_toExecute;
    std::chrono::nanoseconds m_interval;
    std::shared_ptr<Callable> m_task;
};

}

#endif // ILRD_RD141_ASYNC_INJECTION_HPP