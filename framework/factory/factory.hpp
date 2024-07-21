#ifndef ILRD_RD141_FACTORY_HPP
#define ILRD_RD141_FACTORY_HPP

#include <unordered_map>//unordered_map
#include <memory>//shared_ptr
#include "../singleton/singleton.hpp"//my own implementation

namespace ilrd_rd141
{
namespace Factory
{
class DoubleAdd : public std::exception
{
        const char* what() const noexcept;
};


template<class Base,typename Key, typename... Types>
class Factory
{
public:
    Factory(const Factory &other) = delete;
    Factory &operator=(const Factory &lhs) = delete;
    //shared_ptr don't have to get a new pointer, you can choose a Deleter 
    //Add throw DoubleAdd if key is already in use
    void Add(const Key &key, std::shared_ptr<Base>(*creator)(Types...));
    //overrides if key is already in use
    void Set(const Key &key, std::shared_ptr<Base>(*creator)(Types...));
    std::shared_ptr<Base> Create(const Key &key,Types... args) const;
private:
    Factory() = default;
    std::unordered_map<Key, std::shared_ptr<Base>(*)(Types...)> m_creators;
    friend Singleton<Factory>;
};

const char *ilrd_rd141::Factory::DoubleAdd::what() const noexcept
{
    return "Key already exists in the factory.";
}

template <class Base, typename Key, typename... Types>
void Factory<Base, Key, Types...>::Add(const Key &key, std::shared_ptr<Base> (*creator)(Types...))
{
    if (m_creators.find(key) != m_creators.end()) 
    {
        throw DoubleAdd();
    }

    m_creators[key] = creator;
}

template<class Base, typename Key, typename ...Types>
void Factory<Base, Key, Types...>::Set(const Key &key, std::shared_ptr<Base>(*creator)(Types...))
{
    if (m_creators.find(key) == m_creators.end())
    {
        throw std::runtime_error("Key does not exist in the factory.");
    }
    m_creators[key] = creator;    
}

template <class Base, typename Key, typename... Types>
std::shared_ptr<Base> Factory<Base, Key, Types...>::Create(const Key &key, Types... args) const
{
    return m_creators.at(key)(args...);
}

}
} // ilrd_rd141

#endif // ILRD_RD141_FACTORY_HPP