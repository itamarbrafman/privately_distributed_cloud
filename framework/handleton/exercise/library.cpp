#include <iostream>
#include "../../singleton/singleton.hpp"

namespace ilrd_rd141
{

/*
int i = 0;
void foo()
{
    std::printf("LIB\t addr: %p, i: %d\n", &i, i);
}
*/

class Class2
{
    public:
        void doSomething() 
        {
            std::cout << "Class2 is doing something." << std::endl;
        }
        ~Class2()
        {
            std::cout << "Class2 dtor called" << std::endl; 
        }
};

template class Singleton<int>;
}

