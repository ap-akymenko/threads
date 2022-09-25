#include <iostream>
#include <ctime>
#include <cstdlib>
#include <chrono>
#include <thread>
#include <string>
#include <mutex>

using namespace std;
using namespace std::chrono;
mutex mtx1, mtx2;

class FirstClass
{
	double value;
	public:
		FirstClass()
		{value = 0;}
		
		FirstClass(double number)
		{value = number;}
		
		FirstClass(const FirstClass& obj)
		{value = obj.value;}
		
		~FirstClass(){}
		
		double getValue()
		{return value;}
		
		FirstClass& setValue(double number)
		{value = number; return *this;}
		
		FirstClass& addValue(double number)
		{value += number; return *this;}
};

class SecondClass
{
	double value;
	public:
		SecondClass()
		{value = 0;}
		
		SecondClass(double number)
		{value = number;}
		
		SecondClass(const SecondClass& obj)
		{value = obj.value;}
		
		~SecondClass(){}
		
		double getValue()
		{return value;}
		
		SecondClass& setValue(double number)
		{value = number; return *this;}
		
		SecondClass& addValue(double number)
		{value += number; return *this;}
};

void func1 (FirstClass& obj1, SecondClass& obj2, int iterations)
{
	double R;
	// You can make in double rand(), but there are two problems:
	//     1.I don't know why, but two R = rand() generated seperately here
	//       	are very cloze to each other. Almost the same number, despite the
	//        fact I've made srand(time(NULL))
	//     2. It is easier to see correctness of code (and mutex) if R = const
	
	mtx1.lock();
	R = 1; // <-- here it can be R = rand() ...
	for (int i=0; i<iterations; i++)
	{
		obj1.addValue(R);
	}
	mtx1.unlock();
	
	mtx2.lock();
	R = 2; // <-- and here...
	for (int i=0; i<iterations; i++)
	{
		obj2.addValue(R);
	}
	mtx2.unlock();
	
}

void func2 (FirstClass& obj1, SecondClass& obj2, int iterations)
{
	double R;
	
	mtx2.lock();
	R = 3;
	for (int i=0; i<iterations; i++)
	{
		obj2.addValue(R);
	}
	mtx2.unlock();
	
	mtx1.lock();
	R = 4;
	for (int i=0; i<iterations; i++) 
	{
		obj1.addValue(R);
	}
	mtx1.unlock();
}

void threads1_process(FirstClass& obj1, SecondClass& obj2, int N1, int K1)
{
	thread* myThreadsOne = new thread[N1];
	
	for(int i=0; i<N1; i++)
	{
		myThreadsOne[i] = thread (func1, ref(obj1), ref(obj2), K1);
		myThreadsOne[i].join(); // it is unnecessary here due to further reason mentiond after cout <<
		
		cout << "thread " + to_string(i+1) + ": \t myFirst = " + to_string(obj1.getValue()) + ", \t mySecond = " + to_string(obj2.getValue()) + "\n";
		
		/** current myFirst and mySecond will show wrong values (because of parallel computing of another threads), but at the end the result would be correct !!! **/
	}
}

void threads2_process(FirstClass& obj1, SecondClass& obj2, int N, int K2)
{
	int N2;
	N % 2 == 0 ? N2 = N/2 : N2 = N/2 + 1;
	int Delta = N/2;
	
	thread* myThreadsTwo = new thread[N2];
	
	for(int i=0; i<N2; i++)
	{
		myThreadsTwo[i] = thread (func2, ref(obj1), ref(obj2), K2);
		myThreadsTwo[i].join();
		
		cout << "thread " + to_string(i+Delta+1) + ": \t myFirst = " + to_string(obj1.getValue()) + ", \t mySecond = " + to_string(obj2.getValue()) + "\n";
		
	}
}

int main()
{
	int i;
	int startN = 10;
	int finishN = 20;
	
	int startK = 10000;
	int finishK = 20000;
	
	srand(time(NULL));
	//cout.precision(16);
	
	FirstClass myFirst;
	SecondClass mySecond;
	cout << "Initial values: myFirst = " << myFirst.getValue() << ", mySecond = " << mySecond.getValue() << endl;
	
	int N  = rand() % (finishN - startN + 1) + startN;
	int K1 = rand() % (finishK - startK + 1) + startK;
	int K2 = rand() % (finishK - startK + 1) + startK;
	
	int N1, N2;
	N1 = N/2;
	N % 2 == 0 ? N2=N/2 : N2=N/2+1;
	
	cout << "Number of threads generated: " << N << endl;
	cout << "Tasks for " << N1 << " threads: " << K1 << " - myFirst.addValue(rand1), mySecond.addValue(rand2)\n";
	cout << "Tasks for " << N2 << " threads: " << K2 << " - mySecond.addValue(rand3), myFirst.addValue(rand4)\n\n";
	
	auto start = high_resolution_clock::now();
	//processing of threads
	thread nt1(threads1_process, ref(myFirst), ref(mySecond), N1, K1);
	thread nt2(threads2_process, ref(myFirst), ref(mySecond), N, K2);
	
	nt1.join();
	nt2.join();
	
	cout << "\n\n \t Total: myFirst = " + to_string(myFirst.getValue()) + ", mySecond = " + to_string(mySecond.getValue()) + "\n";
	auto finish = high_resolution_clock::now();
	auto duration = duration_cast<microseconds>(finish - start);
	
	cout << "\ntime taken: " << duration.count() << " microseconds" << endl;
	
	fflush(stdin);
	getchar();
	return 0;
}