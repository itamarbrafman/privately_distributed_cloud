#include "logger.hpp"

using namespace ilrd_rd141;

int main()
{
    Logger& instance = Singleton<Logger>::getInstance();

    instance.Log("hi");
    
    std::less<std::string> less_than; // Create a std::less object for integers

    std::string a = "24102023:14:03:29";
    std::string b = "24102023:15:03:28";

    if (less_than(a, b)) 
    {
        std::cout << "a is less than b" << std::endl;
    } else 
    {
        std::cout << "b is less than a" << std::endl;
    }
}