#include <iostream>

//extern int i;
//void foo();

#include "../../singleton/singleton.hpp"

namespace ilrd_rd141
{
class Class2;

extern template class Singleton<int>;

void __attribute__((constructor)) Func();

void Func()
{
    /*
    ++i;
    ++i;
    foo();
    */
    int& instance = Singleton<int>::getInstance();
    std::printf("LIB: addr: %p\n", (void *)&instance);

}
}