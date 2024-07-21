#include <iostream>
#include <unistd.h> /*STDIN_FILENO*/

#include "reactor.hpp"
#include "imonitor.hpp"

using namespace ilrd_rd141;

void Bar(const std::string str)
{
    const size_t BUF_SIZE = 100;
    char buffer[BUF_SIZE] = {0};
    std::cout << str << std::endl;
    std::string stdin_str = fgets(buffer, BUF_SIZE, stdin);

    std::cout << stdin_str << std::endl;
}

void test()
{
    const std::string str = "I'm in Bar";
    Function<void(void)> f1(&Bar, str);

    Reactor<SelectMonitor> r1(new SelectMonitor);
    r1.RegisterReadEventHandler(STDIN_FILENO, f1);
    r1.Run();

}

int main()
{
    test();

    return 0;
}