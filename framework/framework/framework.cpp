#include "framework.hpp"

namespace ilrd_rd141
{

void MonitorAndLoadCommandFactory()
{
    Factory::Factory<Command, CommandType, CommandType>& instance = Singleton<Factory::Factory<Command, CommandType, CommandType>>::getInstance();

    std::string path = "/home/itamar/git/projects/final_project/framework/command/";
    DirMonitor monitor("../../framework/command");
    DllLoader loader;
    Callback<std::string> toDoOnNotify(
    [&loader](std::string path) {loader.load(path);},
    []() { std::cout << "Callback toDoOnNotify death." << std::endl; }
    );

    monitor.Subscribe(toDoOnNotify, DirMonitor::WhatToMonitor::modify);
    std::string compileCommand = "g++ -std=c++11 -shared -fPIC -o " + path + "libcommand_library.so " + path + "command_library.cpp";
    int compileResult = system(compileCommand.c_str());
    if (compileResult == 0) 
    {
        std::cout << "Dynamic library compilation successful." << std::endl;
    } 
    else 
    {
        std::cerr << "Dynamic library compilation failed." << std::endl;
    }

    size_t retryCount = 0;
    size_t maxRetryAttempts = 10;

    std::shared_ptr<Command> readCommand;
    std::shared_ptr<Command> writeCommand;

    while (retryCount < maxRetryAttempts) 
    {
        try 
        {
            readCommand = instance.Create(READ, READ);
            writeCommand = instance.Create(WRITE, WRITE);
            
            break;
        } 
        catch (const std::out_of_range &ex) 
        {
            ++retryCount;
            std::this_thread::sleep_for(std::chrono::seconds(1));            
        }
    }

   // (*readCommand)();
    //(*writeCommand)(); 
}


Framework::Framework(const std::list<CauseAndEffect> &toMonitor, int niceness, size_t numOfThreads) :
                     m_reactor(new SelectMonitor), m_threadPool(niceness, numOfThreads)
{
    for (const auto& causeAndEffect : toMonitor)
    {
        int fd = causeAndEffect.first;
        Function<std::shared_ptr<Callable>(void)> handler = causeAndEffect.second;

        m_reactor.RegisterReadEventHandler(fd, Function<void(void)>(&Framework::AddTask, this, handler));
    }
}

void Framework::Run()
{
    MonitorAndLoadCommandFactory();
    m_threadPool.Run();
    m_reactor.Run();
}

void Framework::Add(const CauseAndEffect &toMonitor)
{
    m_reactor.RegisterReadEventHandler(toMonitor.first, Function<void(void)>(&Framework::AddTask, this, toMonitor.second)); 
}

void Framework::Remove(fileDescriptor fd) noexcept
{
    m_reactor.DeregisterEventHandler(fd);
}

void Framework::AddTask(Function<std::shared_ptr<Callable>(void)> handler)
{
    TaskAndKey taskAndKey;
    taskAndKey = handler.operator()();
    m_threadPool.AddTask(taskAndKey.taskToDo, ThreadPool::Priority::highest);
}
}