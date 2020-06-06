
#include "test.h"

#define PROTOCOL 		PTHREAD_PRIO_INHERIT
#define NB_THREADS 	3
#define NB_LOCKS 		1
#define LOCKS 			'A'

void *jobA(void *arg)
{
	printf("JOB A ...\n");
	lock('A');
	job("A", 1);
	unlock('A');

	return NULL;
}

void *jobB(void *arg)
{
	printf("JOB B ...\n");
	lock('A');
	job("B", 1);
	unlock('A');

	return NULL;
}

void *jobC(void *arg)
{
	printf("JOB C ...\n");
	lock('A');
	job("C", 1);
	unlock('A');

	return NULL;
}

void test()
{
	start_job(0, &jobA);
	start_job(100, &jobB);
	start_job(100, &jobC);
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
