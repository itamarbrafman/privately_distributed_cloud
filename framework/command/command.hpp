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
    virtual ~Command() = default;
    virtual void operator()() = 0;  

private:
    CommandType m_type;
};


Command::Command(CommandType type) : m_type(type)
{
  //empty  
}

template class Singleton<Factory::Factory<Command, CommandType, CommandType>>;
}
