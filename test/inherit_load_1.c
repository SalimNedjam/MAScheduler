
#include "bench.h"

void *jobA(void *arg)
{
	printf("JOB A ...\n");
	lock('A');
	job("A");
	unlock('A');
	return NULL;
}

void *jobB(void *arg)
{
	printf("JOB B ...\n");
	lock('A');
	job("B");
	unlock('A');
	return NULL;
}

void *jobC(void *arg)
{
	printf("JOB C ...\n");
	lock('A');
	job("C");
	unlock('A');
	return NULL;
}

void bench()
{
	pthread_t A, B, C;

	create(&A, 0, &jobA);
	create(&B, 100, &jobB);
	create(&C, 100, &jobC);

  pthread_join(A, NULL);
  pthread_join(B, NULL);
  pthread_join(C, NULL);
}

int main(int argc, const char **argv)
{
  bench_start(PTHREAD_PRIO_INHERIT, 1, 'A');
  bench(); 
	return bench_exit();
}

