#include "singleton.hpp"

using namespace ilrd_rd141;


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

class MyClass 
{
public:
    void doSomething() 
    {
        std::cout << "MyClass is doing something." << std::endl;
    }
    ~MyClass()
    {
        std::cout << "MyClass dtor called" << std::endl; 
        Class2& myInstance = Singleton<Class2>::getInstance(); 
        myInstance.doSomething();  
    }
};

/*
void cleanup2()
{
    std::cout << "cleanup2 called" << std::endl; 

    Singleton<MyClass>& myInstance = Singleton<MyClass>::getInstance(); 
    myInstance.getObject().doSomething();     
}
*/

int main() 
{
    MyClass c1;
    Class2& myInstance = Singleton<Class2>::getInstance();
    //myInstance.getObject().doSomething();
    //Class2 c;
    return 0;
}