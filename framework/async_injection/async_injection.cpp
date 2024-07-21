#include "async_injection.hpp"
#include "../thread_pool/callable_function.hpp"
#include "../scheduler/scheduler.hpp"

template class ilrd_rd141::Singleton<ilrd_rd141::Scheduler>;

namespace ilrd_rd141
{
void Asyncinjection::CreateInstance
(std::function<bool()> functor, std::chrono::nanoseconds interval)
{
	new Asyncinjection(functor, interval);
}

Asyncinjection::Asyncinjection(std::function<bool()> functor, 
std::chrono::nanoseconds interval) : m_toExecute(functor), m_interval(interval),
									m_task(new CallableFunction(Execute, this))
{
	Execute(this);
}


void Asyncinjection::Execute(Asyncinjection *obj)
{
	if(obj->m_toExecute())
	{
		delete obj;
	}
	else
	{
		Scheduler& instance = Singleton<Scheduler>::getInstance();
        instance.schedule(obj->m_task, obj->m_interval);
	}
}
} //ilrd_rd141