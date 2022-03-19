
#include "test.h"

#define PROTOCOL 	PTHREAD_PRIO_INHERIT
#define NB_THREADS	3
#define NB_LOCKS 	1
#define LOCKS 		'A'

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
	lock('A');
	job("B", 1);
	unlock('A');
}

void *job_c(void *arg)
{
	printf("JOB C ...\n");
	lock('A');
	job("C", 1);
	unlock('A');
}

void test()
{
	start_job(0, &job_a);
	start_job(100, &job_b);
	start_job(100, &job_c);
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
