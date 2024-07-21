#include <iostream>
#include <dlfcn.h> //dlopen
#include "../../singleton/singleton.hpp"

//extern int i;
//void foo();
class Class2;

using namespace ilrd_rd141;

namespace ilrd_rd141
{
    extern template class Singleton<int>;
}

int main()
{
    void *handle;      
    
    //std::printf("MAIN START\taddr: %p, i: %d\n", &i, i);

    handle = dlopen ("liblibrary2.so", RTLD_LAZY);
    if (NULL == handle) 
    { 
        printf ("Cannot find library libex3.so\n"); 
        exit(1); 
    } 

    //++i;
    //foo();
    //std::printf("MAIN END\taddr: %p, i: %d\n", &i, i);
    int& instance = Singleton<int>::getInstance();
    std::printf("MAIN: addr: %p\n", (void *)&instance);
	dlclose(handle);
	return 0;
}