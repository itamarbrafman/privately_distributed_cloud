#ifndef ILRD_RD141_MY_ASSERT_HPP
#define ILRD_RD141_MY_ASSERT_HPP

#ifndef NDEBUG
#include <iostream>

#define TEST(X) (static_cast <bool>(X) ?\
myAssert::Success() : myAssert::PrintFailure(__LINE__))

#define SUMMARY (std::cout << "Summary:\n"\
<< (myAssert::counter_fail + myAssert::counter_sucsess) << " tests\n\
fails - " << myAssert::counter_fail \
<< "\nsucsess - " << myAssert::counter_sucsess << "\n")
namespace myAssert
{
size_t counter_sucsess = 0, counter_fail = 0;
void PrintFailure(size_t line)
{
    ++counter_fail;
    std::cout << "test in line " << line << " failed" << std::endl;
}

void Success(void)
{
    ++counter_sucsess;
}

}
#else 
#define TEST(X) (void(X))
#endif /*NDEBUG*/



#endif /* ILRD_RD141_MY_ASSERT_HPP*/