
#include "test.h"

#define PROTOCOL 		PTHREAD_PRIO_INHERIT
#define NB_THREADS 	4
#define NB_LOCKS 		2
#define LOCKS 			'A', 'B'

void *jobA(void *arg)
{
	printf("JOB A ...\n");
	lock('A');
	job("A", 5);
	unlock('A');
	
	return NULL;
}

void *jobB(void *arg)
{
	printf("JOB B ...\n");
	lock('B');
	lock('A');
	job("B", 10);
	unlock('A');
	unlock('B');

	return NULL;
}

void *jobC(void *arg)
{
	printf("JOB C ...\n");
	lock('B');
	job("C", 1);
	unlock('B');

	return NULL;
}

void *jobD(void *arg)
{
	printf("JOB D ...\n");
	lock('A');
	job("D", 1);
	unlock('A');

	return NULL;
}

void test()
{
	start_job(0, &jobA);
	start_job(10, &jobB);
	start_job(500, &jobD);
	start_job(500, &jobC);
	join();
}

int main(int argc, const char **argv)
{
	set_protocol(PROTOCOL);
  set_locks(NB_LOCKS, LOCKS);
	set_threads(NB_THREADS);

  test();

	return 0;
}
