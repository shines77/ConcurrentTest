#include <iostream>
#include <thread>

#include "Singleton.hpp"

#pragma warning(disable:4996)

class help
{
public:
	help();
	help(int x, float f) :i(x), f(f)
	{
		this->i = x;
		this->f = f;
	}
	~help();

public:
	int geti()
	{
		i = 100;
		std::cout << i << std::endl;
		return i;
	}
	void getif()
	{
		std::cout << i << std::endl;
		std::cout << f << std::endl;
	}

	void setif(int x, float f)
	{
		this->i = x;
		this->f = f;
	}

private:
	int i;
	float f;

private:

};

help::help() :i(10), f(18.63f)
{
}

help::~help()
{
}

void ThreadFunc() 
{
	auto& p4 = Singleton<help>::GetInstance(1001, 5.999);
	p4.getif();

	std::cout << "==========================================" << std::endl;
}


int main()
{
	auto& pr = Singleton<help>::GetInstance();
	auto& prs = Singleton<help>::GetInstance(10, 4.5f);

	help *ptr = Singleton<help>::GetInstancePointer(10000, 4.5f);
	help *ptx = Singleton<help>::GetInstancePointer(101, 4.05f);

	std::cout << ptr << std::endl;
	std::cout << ptx << std::endl;


	std::cout << ptr << std::endl;
	std::cout << ptx << std::endl;
	std::cout << &pr << std::endl;
	std::cout << &prs << std::endl;


	std::cout << ptr << std::endl;
	std::cout << ptx << std::endl;

	ptr->getif();


	std::cout << "==========================================" << std::endl;

	ptx->setif(17, 33.333f);
	ptx->getif();

	std::cout << "==========================================" << std::endl;

	const int threadCount = 20; //线程数目 可更改
	std::thread threads[threadCount];


	for (int i = 0; i < threadCount; i++)  	// 多线程执行函数
	{
		std::cout << i << ':';
		threads[i] = std::thread(ThreadFunc);
		_sleep(1000);
	}


	for (auto& th : threads) 	// join() 等待
		th.join();

	system("pause");
	return 0;
}
