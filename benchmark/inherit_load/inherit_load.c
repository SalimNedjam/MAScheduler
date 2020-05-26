#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/syscall.h>

pthread_mutex_t lock;
pthread_mutex_t lockA;
pthread_mutex_t lockB;

void job()
{
	pid_t tid = syscall(SYS_gettid);
	
	printf("pid %d\n", tid);
	
	for (size_t i = 0; i < 10000; i++)
		syscall(SYS_gettid);
}

void *jobA(void *arg)
{
	printf("jobA...\n");
	pthread_mutex_lock(&lockA);
	
	printf("jobA start\n");
	job();
	printf("jobA finish\n");
	
	pthread_mutex_unlock(&lockA);

	return NULL;
}

void *jobB(void *arg)
{
	printf("jobB...\n");
	pthread_mutex_lock(&lockA);
	pthread_mutex_lock(&lockB);
	
	printf("jobB start\n");
	job();
	printf("jobB finish\n");
	
	pthread_mutex_unlock(&lockB);
	pthread_mutex_unlock(&lockA);

	return NULL;
}

int create(pthread_t *thread, void *(*start_routine) (void *))
{
	pthread_attr_t attr_thread;
	struct sched_param param;
  int ret;
  
	ret = pthread_attr_init(&attr_thread);
	ret = pthread_attr_getschedparam(&attr_thread, &param);
  
  ret = pthread_create(thread, &attr_thread, start_routine, NULL);
  if (ret != 0)
		printf("cannot create thread %s\n", strerror(ret));
		
  return ret;
}

void bench(const pthread_mutexattr_t *attr)
{
	printf("start bench...\n");

	pthread_t A, B;

	pthread_mutex_lock(&lock);

	create(&A, &jobA);
	create(&B, &jobB);

  pthread_join(A, NULL);
  pthread_join(B, NULL);
  
  pthread_mutex_destroy(&lock);
  
  printf("finish bench...\n");
}

int main(int argc, const char **argv)
{
  pthread_mutexattr_t attr;

	if (pthread_mutexattr_init(&attr) == -1) {
		perror("mutexattr_init error");
		return -1;
	}
	
	pthread_mutexattr_setprotocol(&attr, PTHREAD_PRIO_INHERIT);
	
	if (pthread_mutex_init(&lock, &attr) != 0) {
		printf("mutex init failed\n");
		return -1;
	}
	
	if (pthread_mutex_init(&lockA, &attr) != 0) {
		printf("mutex init failed\n");
		return -1;
	}
	
	if (pthread_mutex_init(&lockB, &attr) != 0) {
		printf("mutex init failed\n");
		return -1;
	}
	
	
  bench(&attr);
  
  return 0;
}
