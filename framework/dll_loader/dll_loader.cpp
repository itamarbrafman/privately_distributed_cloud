#include <dlfcn.h> //dlopen, dlclose
#include <stdexcept> //std::runtime_error
#include "dll_loader.hpp"

void ilrd_rd141::DllLoader::load(const std::string &path)
{
    void *handle = dlopen(path.c_str(), RTLD_LAZY);
    if (handle == nullptr) 
    {
        throw std::runtime_error("Error loading library: " + std::string(dlerror()));
    }

    m_handles[path] = handle;
}

void ilrd_rd141::DllLoader::unload(const std::string &path)
{
    auto it = m_handles.find(path);
    if (it != m_handles.end()) 
    {
        dlclose(it->second);
        m_handles.erase(it);
    }
}

ilrd_rd141::DllLoader::~DllLoader()
{
    for (auto iter : m_handles) 
    {
        dlclose(iter.second);
    }
}

