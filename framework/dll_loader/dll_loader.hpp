#ifndef ILRD_RD141_DLLLOADER_HPP
#define ILRD_RD141_DLLLOADER_HPP

#include <string> //std::string
#include <unordered_map> //unordered_map

namespace ilrd_rd141 {
class DllLoader {
public:
    DllLoader() = default;
    ~DllLoader();
    DllLoader(const DllLoader&) = delete;
    DllLoader& operator=(const DllLoader&) = delete;
    void load(const std::string &path);
    void unload(const std::string &path);

private:
    std::unordered_map<std::string, void*> m_handles;
};
} // ilrd_rd141

#endif /* ILRD_RD141_DLLLOADER_HPP */

