// #include <iostream>
// #include <chrono>
// #include "thread_pool.hpp"
// #include "callable.hpp"

// using namespace ilrd_rd141;


// void FreeFunc(int stab)
// {
//     (void)stab;
//     std::cout << "Func" << std::endl;
// }

// int main()
// {
//     ilrd_rd141::ThreadPool threadPool(0, 4);

//     std::shared_ptr<Callable> task = std::make_shared<CallableFunction>(FreeFunc, 0);
//     for (int i = 0; i < 8; ++i)
//     {
//         threadPool.AddTask(task, ThreadPool::Priority::medium);
//     }

//     threadPool.SetNumOfThreads(8);


//     std::cout << "Run" << std::endl;

//     threadPool.Run();


//     threadPool.Pause();


//     threadPool.SetNumOfThreads(4);



//     threadPool.SetNumOfThreads(3);

//     threadPool.Pause();

//     threadPool.Run();


//     return 0;
// }



/*

	Authur: David Haver
	Reviewer:
	Date:
	
*/
#include <iostream>
#include <unistd.h>
#include "my_assert.hpp"
#include "callable_function.hpp"
#include "thread_pool.hpp"/*header*/                                                     

void BasicTest(void);
void SetTest(void);
void Change(int num[]);

using namespace ilrd_rd141;
int main()
{
	try
	{
        // for( int i = 0 ; i < 1; i++)
        // {
		    BasicTest();
        // }
        SetTest();
	}
	catch(const std::exception& e)
	{
		std::cout << e.what() << '\n';
	}
	
	SUMMARY;
	return 0;
}

void BasicTest(void)
{
	ThreadPool pool;
	int arr[15] = {0};
	pool.AddTask(std::shared_ptr<Callable>
	(new Function<void(void)>(Change, arr)));
	pool.Run();
	pool.AddTask(std::shared_ptr<Callable>
	(new Function<void(void)>(Change, arr + 5)));
	sleep(2);
	for (size_t i = 0; i < 10; i++)
	{
		TEST(1 == arr[i]);
	}
	pool.Pause();
	pool.AddTask(std::shared_ptr<Callable>
	(new Function<void(void)>(Change, arr + 10)));
	sleep(2);
	for (size_t i = 10; i < 15; i++)
	{
		TEST(0 == arr[i]);
	}
}

void SetTest(void)
{
	ThreadPool pool(5, 4);

	pool.SetNumOfThreads(6);
	int arr[15] = {0};
	pool.AddTask(std::shared_ptr<Callable>
	(new Function<void(void)>(Change, arr)));
	pool.AddTask(std::shared_ptr<Callable>
	(new Function<void(void)>(Change, arr + 5)));
	pool.Run();
	sleep(2);
	for (size_t i = 0; i < 10; i++)
	{
		TEST(1 == arr[i]);
	}
	pool.Pause();
	pool.SetNumOfThreads(4);
	pool.SetNumOfThreads(1);
	pool.AddTask(std::shared_ptr<Callable>
	(new Function<void(void)>(Change, arr + 10)));
	sleep(2);
	for (size_t i = 10; i < 15; i++)
	{
		TEST(0 == arr[i]);
	}
	pool.Run();
	sleep(2);
	for (size_t i = 10; i < 15; i++)
	{
		TEST(1 == arr[i]);
	}
}

void Change(int arr[])
{
	for (size_t i = 0; i < 5; i++)
	{
		arr[i] = 1;
	}	
}

