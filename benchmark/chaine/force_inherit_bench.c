#include<stdio.h>
#include<string.h>
#include<pthread.h>
#include<stdlib.h>
#include<unistd.h>
#include<benchmark/benchmark.h>
#include <sys/types.h>
#include <sys/syscall.h>
#define NB_THREAD 100
pthread_t tid[NB_THREAD];
int counter;
pthread_mutex_t lock;

void *doSomeThing(void *arg)
{
	pthread_mutex_lock(&lock);
	counter += 1;
	pid_t tid;

	for (size_t i = 0; i < 10000; i++)
		tid = syscall(SYS_gettid);

	pthread_mutex_unlock(&lock);

	return NULL;
}


void bench(const pthread_mutexattr_t *attr)
{
	int i = 0;
	int err;
	pthread_attr_t attr_thread;
	struct sched_param param;

	counter = 0;
	err = pthread_attr_init(&attr_thread);
	err = pthread_attr_getschedparam(&attr_thread, &param);


	if (pthread_mutex_init(&lock, attr) != 0) {
		printf("\n mutex init failed\n");
		return;
	}
	pthread_mutex_lock(&lock);

	while (i < NB_THREAD) {

		param.sched_priority =(NB_THREAD-1)/10 - i/10;
		err = pthread_attr_setschedparam(&attr_thread, &param);

		err = pthread_create(&(tid[i]), &attr_thread,
					&doSomeThing, NULL);
		if (err != 0)
			printf("\ncan't create thread :[%s]", strerror(err));
		i++;
	}
	pthread_mutex_unlock(&lock);

	for (i = 0; i < NB_THREAD; i++)
		pthread_join(tid[i], NULL);

	pthread_mutex_destroy(&lock);
}


static void BM_StringCreation(benchmark::State & state)
{

	for (auto _ : state)
		bench(NULL);
}
static void BM_StringCreation_bis(benchmark::State & state)
{
	pthread_mutexattr_t     attr;

	if (pthread_mutexattr_init(&attr) == -1) {
		perror("mutexattr_init error");
		return;
	}
	pthread_mutexattr_setprotocol(&attr, PTHREAD_PRIO_INHERIT);

	for (auto _ : state)
		bench(&attr);


}




// Register the function as a benchmark
BENCHMARK(BM_StringCreation);
BENCHMARK(BM_StringCreation_bis);
BENCHMARK_MAIN();
