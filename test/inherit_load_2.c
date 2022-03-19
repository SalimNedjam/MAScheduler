
#include "test.h"

#define PROTOCOL 	PTHREAD_PRIO_INHERIT
#define NB_THREADS 	3
#define NB_LOCKS 	2
#define LOCKS 		'A', 'B'

void *job_a(void *arg)
{
	printf("JOB A ...\n");
	lock('A');
	job("A", 1);
	unlock('A');
}

void *job_b(void *arg)
{
	printf("JOB B ...\n");
	lock('B');
	lock('A');
	job("B", 10);
	unlock('A');
	unlock('B');
}

void *job_c(void *arg)
{
	printf("JOB C ...\n");
	lock('B');
	job("C", 1);
	unlock('B');
}

void test()
{
	start_job(0, &job_a);
	start_job(10, &job_b);
	start_job(500, &job_c);
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
