#include <iostream>
#include<unistd.h>

#include "async_injection.hpp"

int count = 0;
int test2 = 222;
bool Test1()
{
    std::cout << count << std::endl;
    ++count;
    if(count == 3)
    {
        std::cout << "ended" << std::endl;
        return true;
    }
    return false;
}

bool Test2()
{
    std::cout << test2 << std::endl;
    return true;
}

int main()
{
    std::chrono::nanoseconds execution_time_one = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::seconds(2));
    std::chrono::nanoseconds execution_time_two = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::seconds(1));

    ilrd_rd141::Asyncinjection::CreateInstance(Test1,execution_time_one);
    ilrd_rd141::Asyncinjection::CreateInstance(Test2,execution_time_two);

    sleep(8);

    std::cout << count << std::endl;


    return 0;
}