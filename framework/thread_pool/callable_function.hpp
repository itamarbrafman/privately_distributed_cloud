#ifndef __Callable_Function_H__
#define __Callable_Function_H__

#include <stddef.h>    /*size_t*/
#include <memory> //shared_ptr

#include "callable.hpp"//Callable

namespace ilrd_rd141
{
class CallableFunction: public Callable
{
public:
    template <typename Args>
    explicit CallableFunction(void (*function)(Args), Args argument);

    template <typename Object, typename Args>
    explicit CallableFunction(void (Object::*function)(Args), Object &object, Args argument);

	void operator()();

private:
	class Ifunction
    {
	public:
		virtual ~Ifunction() = default;
		virtual void Invoke() = 0;		
	};

    template <typename Args>
	class Ifunction_Free:  public Ifunction
    {
	public:
		Ifunction_Free(void (*function)(Args), Args& args);
		~Ifunction_Free() = default;
		void Invoke();

	private:
		void (*m_function)(Args);
	    Args m_argument;
	};

    template <typename Object, typename Args>
	class Ifunction_Object: public Ifunction
    {
	public:
		Ifunction_Object(void (Object::*function)(Args), Object& object, Args& args);

		~Ifunction_Object() override = default;

		void Invoke() override;

	private:
		void (Object::*m_function)(Args);
		Object &m_object;
        Args m_argument;
	};

	std::shared_ptr<Ifunction> m_function;
};

#endif /* ILRD_RD141_CALLABLE_HPP*/

template <typename Args>
CallableFunction::CallableFunction(void (*function)(Args), Args argument): 
m_function(new Ifunction_Free<Args>(function, argument)) 
{
	//empty
}

template <typename Object, typename Args>
CallableFunction::CallableFunction(void (Object::*function)(Args), Object &object, Args argument)
: m_function(new Ifunction_Object<Object,Args>(function, object ,argument))
{
	//empty
}

inline void CallableFunction:: operator()()
{
    m_function->Invoke();
}

template <typename Args>
CallableFunction::Ifunction_Free<Args>::Ifunction_Free(void (*function)(Args), Args& args)
:m_function(function), m_argument(args)
{
	//empty
}
template <typename Args>
void CallableFunction::Ifunction_Free<Args>::Invoke() 
{
	m_function(m_argument);
}

template <typename Object, typename Args>
CallableFunction::Ifunction_Object<Object,Args>::Ifunction_Object(void
 (Object::*function)(Args), Object& object, Args& args) : m_function(function), 
 m_object(object),m_argument(args)
{
	//empty
}
template <typename Object, typename Args>
void CallableFunction::Ifunction_Object<Object,Args>::Invoke() 
{
	(m_object.*m_function)(m_argument);
}

} // ilrd_rd141