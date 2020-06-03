
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

void bench()
{
	pthread_t A, B;

	create(&A, 0, &jobA);
	create(&B, 100, &jobB);

  pthread_join(A, NULL);
  pthread_join(B, NULL);
}

int main(int argc, const char **argv)
{
  bench_start(PTHREAD_PRIO_INHERIT, 1, 'A');
  bench(); 
	return bench_exit();
}

