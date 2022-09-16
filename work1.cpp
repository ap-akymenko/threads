#include <iostream>
#include <thread>
#include <chrono>

using namespace std;
using namespace std::chrono;

void task1()
{
	sleep(2);
}

void task2()
{
	sleep(4);
}

void task3()
{
	sleep(1);
}

int main()
{
	cout << "processing tasks . . ." << endl;

	auto start = high_resolution_clock::now();
	{
		thread t1(task1);
		t1.join();
		thread t2(task2);
		t2.join();
		thread t3(task3);
		t3.join(); // join ensure that program won't return to main
                  //  until thread would be completed
	}
	auto finish = high_resolution_clock::now();
	auto duration = duration_cast<microseconds>(finish - start);

	cout << "time taken: "
		 << duration.count() << " microseconds" << endl;

	fflush(stdin);
	getchar();
	return 0;
}
