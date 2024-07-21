#ifndef __Function_H__
#define __Function_H__

#include <stddef.h>    /*size_t*/
#include <memory> //shared_ptr

namespace ilrd_rd141
{

template <typename>
class Function;

template <typename RET>
class Function<RET(void)>
{
public:
    template <typename ARG>
    explicit Function(RET(*func_ptr)(ARG), ARG arg);

    template <typename ARG, typename OBJ>
    explicit Function(RET(OBJ::*func_ptr)(ARG), OBJ *obj, ARG arg);

    RET operator()();

private:
    class IFunction
    {
    public:
        virtual RET operator()() = 0;
    };

    template <typename ARG>
    class FreeFunction : public IFunction
    {
    public:
        FreeFunction(RET(*func_ptr)(ARG), ARG arg);

        virtual RET operator()() override;

    private:
        RET (*m_func_ptr)(ARG);
        ARG m_arg;
    };

    template <typename ARG, typename OBJ>
    class MemberFunction : public IFunction
    {
    public:
        MemberFunction(RET(OBJ::*func_ptr)(ARG), OBJ *obj, ARG arg);

        virtual RET operator()() override;

    private:
        RET (OBJ::*m_func_ptr)(ARG);
        OBJ *m_obj;
        ARG m_arg;
    };
    
    std::shared_ptr<IFunction> m_shared_ptr;
};

#endif /* __Function_H__*/


// Implementations

template <typename RET>
template <typename ARG>
Function<RET(void)>::Function(RET (*func_ptr)(ARG), ARG arg) : m_shared_ptr(new FreeFunction<ARG>(func_ptr, arg))
{
    //empty
}

template <typename RET>
template <typename ARG, typename OBJ>
Function<RET(void)>::Function(RET (OBJ::*func_ptr)(ARG), OBJ *obj, ARG arg) : m_shared_ptr(new MemberFunction<ARG, OBJ>(func_ptr, obj, arg))
{
    //empty
}

template <typename RET>
RET Function<RET(void)>::operator()()
{
    return (*m_shared_ptr)();
}

template <typename RET>
template <typename ARG>
Function<RET(void)>::FreeFunction<ARG>::FreeFunction(RET (*func_ptr)(ARG), ARG arg) : m_func_ptr(func_ptr), m_arg(arg)
{
    //empty
}

template <typename RET>
template <typename ARG>
RET Function<RET(void)>::FreeFunction<ARG>::operator()()
{
    return m_func_ptr(m_arg);
}

template <typename RET>
template <typename ARG, typename OBJ>
Function<RET(void)>::MemberFunction<ARG, OBJ>::MemberFunction(RET (OBJ::*func_ptr)(ARG), OBJ *obj, ARG arg) : m_func_ptr(func_ptr), m_obj(obj), m_arg(arg)
{
    //empty
}

template <typename RET>
template <typename ARG, typename OBJ>
RET Function<RET(void)>::MemberFunction<ARG, OBJ>::operator()()
{
    return (m_obj->*m_func_ptr)(m_arg);
}

} //namespace ilrd_rd141

