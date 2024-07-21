#ifndef __ILRD_RD141_SINGLETON_HPP__
#define __ILRD_RD141_SINGLETON_HPP__

#include <iostream>
#include <mutex>
#include <cstdlib> //atexit

namespace ilrd_rd141
{

template <typename T>
class Singleton {
public:
    static T& getInstance();     
    Singleton(const Singleton&) = delete;
    Singleton& operator=(const Singleton&) = delete;
    ~Singleton() = default;

private:
    Singleton(){}
    static T *s_obj;
    static std::mutex s_mutex;
    static bool s_isReady;
    static void cleanup();
};

#endif // __ILRD_RD141_SINGLETON_HPP__

template <typename T>
T* Singleton<T>::s_obj = nullptr;

template <typename T>
std::mutex Singleton<T>::s_mutex;

template <typename T>
bool Singleton<T>::s_isReady = false;

template <typename T>
T& Singleton<T>::getInstance()
{
    if(false == s_isReady)
    {
        const std::lock_guard<std::mutex> lock(s_mutex);
        if(false == s_isReady)
        {
            atexit(cleanup);
            s_obj = new T; 
            s_isReady = true;       
        }    
    }

    return *s_obj;
}

template <typename T>
void Singleton<T>::cleanup()
{
    std::cout << "Cleanup function called." << std::endl;

    T* temp = s_obj;
    s_obj = reinterpret_cast<T*>(0xDEADBEEF); 

    delete temp;
}

} // ilrd_rd141