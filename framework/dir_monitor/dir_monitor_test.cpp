#include "dir_monitor.hpp"
#include "../dll_loader/dll_loader.hpp"
#include "../factory/factory.hpp"

namespace ilrd_rd141
{
enum CommandType 
{
    READ,
    WRITE
};

class Command
{
public:
    Command(CommandType type);
    virtual void printCommand() = 0;

private:
    CommandType m_type;
};


   extern template class Singleton<Factory::Factory<Command, CommandType, CommandType>>;
}

using namespace ilrd_rd141;

int main()
{
std::cout << "1" << std::endl;    
    Factory::Factory<Command, CommandType, CommandType>& instance = Singleton<Factory::Factory<Command, CommandType, CommandType>>::getInstance();
std::cout << "2" << std::endl;    

    std::string path = "/home/itamar/git/projects/final_project/framework/dll_loader/";
    DirMonitor monitor("../dll_loader");
    DllLoader loader;
    Callback<std::string> toDoOnNotify(
    [&loader](std::string path) {loader.load(path);},
    []() { std::cout << "Callback toDoOnNotify death." << std::endl; }
    );
    std::cout << "3" << std::endl;    

    monitor.Subscribe(toDoOnNotify, DirMonitor::WhatToMonitor::modify);
std::cout << "4" << std::endl;    
    std::this_thread::sleep_for(std::chrono::seconds(10));

    std::shared_ptr<Command> read_com = instance.Create(READ, READ);
    std::shared_ptr<Command> write_com = instance.Create(WRITE, WRITE);
std::cout << "5" << std::endl;    

    read_com->printCommand();
    write_com->printCommand(); 
   std::cout << "6" << std::endl;    
 
    return 0;
}