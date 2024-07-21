#ifndef ILRD_RD141_PQ_TO_Q_HPP
#define ILRD_RD141_PQ_TO_Q_HPP

#include <queue>

namespace ilrd_rd141
{

template<
    typename T,
    class CONTAINER = std::vector<T>,
    class COMPARE = std::less<typename CONTAINER::value_type>
>
class FrontablePQ
{
public:
    FrontablePQ()
    {
        //empty
    }
    void push(const T& value)
    {
        queue.push(value);
    }
    void pop()
    {
        queue.pop();
    }
    const T& front() const
    {
        return queue.top();
    }
    T front()
    {
        return queue.top();
    }
    bool empty() const
    {
        return queue.empty();
    };
private:
    std::priority_queue<T, CONTAINER, COMPARE> queue;
};

} // ilrd_rd141

#endif // ILRD_RD141_PQ_TO_Q_HPP