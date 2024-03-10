#include <pthread.h>
#include <string.h>
#include <iostream>
#include <random>
#include <chrono>
#include "./skiplist.h"
using namespace std;


SkipList<int, string> skipList(20);
#define TEST_COUNT 100000



void *insertElement(void* arg) 
{
	int n = *((int *)arg);
	int cnt = TEST_COUNT * n;

	mt19937 rng;
    rng.seed(random_device()());
    uniform_int_distribution<mt19937::result_type> dist6(0, cnt - 1);

	for (int i=0; i < cnt; i++) {
		skipList.insert_element(dist6(rng), "a"); 
	}
	cout << "skiplist size()=" << skipList.size() << endl;
	return NULL;
}

void *getElement(void* arg) 
{
	int n = *((int *)arg);
	int cnt = TEST_COUNT * n;

	mt19937 rng;
    rng.seed(random_device()());
    uniform_int_distribution<mt19937::result_type> dist6(0, cnt - 1);

	for (int i=0; i<cnt; i++) {
		skipList.search_element(dist6(rng)); 
	}
    pthread_exit(NULL);
}

void stress_test_insert(int n = 1)
{
	pthread_t t_id;

	auto start = chrono::high_resolution_clock::now();

	if (pthread_create((&t_id), NULL, insertElement, (void*)&n)!=0)
	{
		cout << "pthread_create() error";
	}

	if (pthread_join(t_id, NULL) !=0)
	{
		cout << "pthread_join() error";
	}

	auto finish = chrono::high_resolution_clock::now(); 
	chrono::duration<double> elapsed = finish - start;
	cout << "insert elapsed:" << elapsed.count() << endl;
}

void stress_test_read(int n = 1)
{
	pthread_t t_id;
	insertElement((void *)&n);

	auto start = chrono::high_resolution_clock::now();

	if (pthread_create((&t_id), NULL, getElement, (void*)&n)!=0)
	{
		cout << "pthread_create() error";
	}

	if (pthread_join(t_id, NULL) !=0)
	{
		cout << "pthread_join() error";
	}

	auto finish = chrono::high_resolution_clock::now(); 
	chrono::duration<double> elapsed = finish - start;
	cout << "get elapsed:" << elapsed.count() << endl;
}

int main()
{
	//stress_test_insert(1);
	stress_test_read(10);
	return 0;
}
