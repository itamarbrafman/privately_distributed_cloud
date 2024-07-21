#ifndef ILRD_RD141_CALLABLE_HPP
#define ILRD_RD141_CALLABLE_HPP

namespace ilrd_rd141
{
class Callable
{
public:
virtual ~Callable() = default;

virtual void operator()() = 0;
};
} // ilrd_rd141

#endif /* ILRD_RD141_CALLABLE_HPP*/
