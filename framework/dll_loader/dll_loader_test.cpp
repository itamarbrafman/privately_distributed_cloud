#include "../factory/factory.hpp"
#include "dll_loader.hpp"
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
    
   Factory::Factory<Command, CommandType, CommandType>& instance = Singleton<Factory::Factory<Command, CommandType, CommandType>>::getInstance();

    DllLoader loader;

    loader.load("liblibrary2.so");
    std::shared_ptr<Command> read_com = instance.Create(READ, READ);
    std::shared_ptr<Command> write_com = instance.Create(WRITE, WRITE);


    read_com->printCommand();
    write_com->printCommand(); 

    loader.unload("liblibrary2.so");


    return 0;
}

