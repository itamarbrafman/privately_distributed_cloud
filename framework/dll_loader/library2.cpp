#include <iostream>
#include <memory>

#include "../factory/factory.hpp"

namespace ilrd_rd141
{


void __attribute__((constructor)) Func();

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


Command::Command(CommandType type) : m_type(type)
{
  //empty  
}

extern template class Singleton<Factory::Factory<Command, CommandType, CommandType>>;

class ReadCommand : public Command 
{
public:
    ReadCommand() : Command(READ) {}

    void printCommand() 
    {
        std::cout << "This is a READ command." << std::endl;
    }
};

class WriteCommand : public Command 
{
public:
    WriteCommand() : Command(WRITE) {}

    void printCommand()  
    {
        std::cout << "This is a WRITE command." << std::endl;
    }
};

std::shared_ptr<Command> createCommand(CommandType type) 
{
    switch (type) 
    {
        case READ:
            return std::make_shared<ReadCommand>();
        case WRITE:
            return std::make_shared<WriteCommand>();
        default:
            return nullptr;
    }

}

void Func()
{
    Factory::Factory<Command, CommandType, CommandType>& instance = Singleton<Factory::Factory<Command, CommandType, CommandType>>::getInstance();

    instance.Add(READ, &createCommand);
    instance.Add(WRITE, &createCommand);
    std::cout << "Added to Factory" << std::endl;
}
}