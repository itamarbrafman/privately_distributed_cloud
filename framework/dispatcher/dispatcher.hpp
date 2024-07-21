#ifndef ILRD_RD141_DISPATCHER_HPP
#define ILRD_RD141_DISPATCHER_HPP

#include <functional>//function
#include <unordered_set>
#include <algorithm> //for_each
#include <iostream>

namespace ilrd_rd141
{
template<typename... ArgTypes>
class Callback;

template<typename... ArgTypes>
class Dispatcher
{
public:
    Dispatcher();
    ~Dispatcher() noexcept;
    Dispatcher(const Dispatcher &other) = delete;
    Dispatcher(Dispatcher &&other) = delete;
    Dispatcher &operator=(const Dispatcher &other) = delete;
    Dispatcher &operator=(Dispatcher &&other) = delete;

    void Notify(ArgTypes... args); 

private:
    void Subscribe(Callback<ArgTypes...> &callback);
    void UnSubscribe(Callback<ArgTypes...> &callback);
    friend Callback<ArgTypes...>;
    std::unordered_set<Callback<ArgTypes...>*> m_subscribers;
};

template<typename... ArgTypes>
class Callback
{
public:
    Callback(const std::function<void(ArgTypes...)> &toDoOnNotify,
    const std::function<void(void)> &OnDispatcherDeath);
    ~Callback() noexcept;
    void SetDispatcher(Dispatcher<ArgTypes...> &dispatcher);

    Callback(const Callback &other) = delete;
    Callback(Callback &&other) = delete;
    Callback &operator=(const Callback &other) = delete;
    Callback &operator=(Callback &&other) = delete;

private:
    void OnDispatcherDeath();
    void operator()(ArgTypes... args);
    friend Dispatcher<ArgTypes...>;
    Dispatcher<ArgTypes...> *m_dispatcher;
    const std::function<void(ArgTypes...)> m_toDoOnNotify;
    const std::function<void(void)> m_OnDispatcherDeath;
};

template <typename... ArgTypes>
Dispatcher<ArgTypes...>::Dispatcher(){}

template <typename... ArgTypes>
Dispatcher<ArgTypes...>::~Dispatcher() noexcept
{
    std::cout << "Dispatcher dtor is called" << std::endl;
    auto onDispatcherDeathFunction = [](Callback<ArgTypes...>* subscriber) {(subscriber->m_OnDispatcherDeath)();};
    std::for_each(m_subscribers.begin(), m_subscribers.end(), onDispatcherDeathFunction);
}

template <typename... ArgTypes>
void Dispatcher<ArgTypes...>::Notify(ArgTypes... args)
{
    auto notifyFunction = [args...](Callback<ArgTypes...>* callback) {(*callback)(args...);};

    std::for_each(m_subscribers.begin(), m_subscribers.end(), notifyFunction);
}

template <typename... ArgTypes>
void Dispatcher<ArgTypes...>::Subscribe(Callback<ArgTypes...> &callback)
{
    std::cout << "Subscribe func is called" << std::endl;

    m_subscribers.insert(&callback);

}

template <typename... ArgTypes>
void Dispatcher<ArgTypes...>::UnSubscribe(Callback<ArgTypes...> &callback)
{
    m_subscribers.erase(&callback);    
}

/****************************CALLBACK FUNCS************************************************************/

template <typename... ArgTypes>
Callback<ArgTypes...>::Callback(const std::function<void(ArgTypes...)> &toDoOnNotify,
                                const std::function<void(void)> &OnDispatcherDeath) :
                                m_dispatcher(nullptr),
                                m_toDoOnNotify(toDoOnNotify),
                                m_OnDispatcherDeath(OnDispatcherDeath)
{
}

template <typename... ArgTypes>
Callback<ArgTypes...>::~Callback() noexcept
{
    std::cout << "Callback dtor is called" << std::endl;
    /*
    if(!m_dispatcher.m_subscribers.empty())
    {
    }    
    */
    m_dispatcher->UnSubscribe(*this);
}

template <typename... ArgTypes>
void Callback<ArgTypes...>::SetDispatcher(Dispatcher<ArgTypes...> &dispatcher)
{
    m_dispatcher = &dispatcher;
    dispatcher.Subscribe(*this);
}

template <typename... ArgTypes>
void Callback<ArgTypes...>::OnDispatcherDeath()
{
    std::cout << "OnDispatcherDeath func is called" << std::endl;
    m_OnDispatcherDeath();  
}

template <typename... ArgTypes>
void Callback<ArgTypes...>::operator()(ArgTypes... args)
{
    m_toDoOnNotify(args...);
}
} // ilrd_rd141

#endif /* ILRD_RD141_DISPATCHER_HPP*/
