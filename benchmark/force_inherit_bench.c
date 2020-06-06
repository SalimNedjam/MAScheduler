#include<stdio.h>
#include<string.h>
#include<pthread.h>
#include<stdlib.h>
#include<unistd.h>
#include <sys/types.h>
#include <sys/syscall.h>

#define NB_CHILDS 30
#define NB_OTHERS 20

pthread_t childs[NB_CHILDS];
pthread_t others[NB_OTHERS];
pthread_mutex_t lock;

void *child_job(void *arg)
{
	pthread_mutex_lock(&lock);

	printf("child\n");

	for (size_t i = 0; i < 10000; i++)
		syscall(SYS_gettid);

	pthread_mutex_unlock(&lock);

	return NULL;
}

void *parent_job(void *arg)
{
	pthread_attr_t attr_thread;
	int i = 0, err;

	pthread_mutex_lock(&lock);

	pthread_attr_init(&attr_thread);
	
	while (i < NB_CHILDS) {
		err = pthread_create(&(childs[i]), &attr_thread, &child_job, NULL);
		if (err != 0)
			printf("\ncan't create thread :[%s]", strerror(err));
		i++;
	}

	printf("parent start\n");

	for (size_t i = 0; i < 10000 * 100; i++)
		syscall(SYS_gettid);

	printf("parent end\n");

	pthread_mutex_unlock(&lock);

	for (i = 0; i < NB_CHILDS; i++)
		pthread_join(childs[i], NULL);

	return NULL;
}

void *other_job(void *arg)
{	
	printf("other start\n");

	for (size_t i = 0; i < 10000 * 100; i++)
		syscall(SYS_gettid);

	printf("other end\n");

	return NULL;
}


void bench(const pthread_mutexattr_t *attr)
{
	pthread_attr_t attr_thread;
	int i = 0, err;

	if (pthread_mutex_init(&lock, attr) != 0) {
		printf("\n mutex init failed\n");
		return;
	}
	pthread_attr_init(&attr_thread);

	pthread_t *parent = (pthread_t*) malloc(sizeof(pthread_t));
	err = pthread_create(parent, &attr_thread, &parent_job, NULL);
	
	if (err != 0)
			printf("\ncan't create thread :[%s]", strerror(err));


	while (i < NB_OTHERS) {
		err = pthread_create(&(others[i]), &attr_thread, &other_job, NULL);
		if (err != 0)
			printf("\ncan't create thread :[%s]", strerror(err));
		i++;
	}

	pthread_join(*parent, NULL);

}

int main(int argc, const char **argv)
{
	pthread_mutexattr_t attr;

	if (pthread_mutexattr_init(&attr) == -1) {
		perror("mutexattr_init error");
		return -1;
	}
	pthread_mutexattr_setprotocol(&attr, PTHREAD_PRIO_INHERIT);

	bench(&attr);

	return 0;
}
