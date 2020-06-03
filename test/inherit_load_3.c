
#include "bench.h"

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

void bench()
{
	pthread_t A, B, C, D;

	create(&A, 0, &jobA);
	create(&B, 10, &jobB);
	create(&D, 500, &jobD);
	create(&C, 500, &jobC);

  pthread_join(A, NULL);
  pthread_join(B, NULL);
  pthread_join(C, NULL);
  pthread_join(D, NULL);
}

int main(int argc, const char **argv)
{
  bench_start(PTHREAD_PRIO_INHERIT, 2, 'A', 'B');
  bench(); 
	return bench_exit();
}

