#ifndef TEST_H
#define TEST_H

#include <pthread.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/syscall.h>

pthread_mutexattr_t attr;
struct table *lock_table;
pthread_t **threads;
int nb_threads, max_threads;

struct node
{
	int key;
	pthread_mutex_t *val;
	struct node *next;
};

struct table
{
	int size;
	struct node **list;
};

struct table *create_table(int size)
{
	struct table *t = (struct table*) malloc(sizeof(struct table));
	t->size = size;
	t->list = (struct node**) malloc(sizeof(struct node*)*size);
	int i;
	for (i = 0; i < size; i++)
		t->list[i] = NULL;

	return t;
}

int hashCode(struct table *t, int key)
{
	if (key < 0)
		return -(key % t->size);

	return key % t->size;
}

void insert(struct table *t , int key, pthread_mutex_t *val)
{
	int pos = hashCode(t,key);
	struct node *list = t->list[pos];
	struct node *newNode = (struct node*) malloc(sizeof(struct node));
	struct node *temp = list;

	while(temp) {
		if(temp->key == key) {
			temp->val = val;
			return;
		}
		temp = temp->next;
	}

	newNode->key = key;
	newNode->val = val;
	newNode->next = list;
	t->list[pos] = newNode;
}

pthread_mutex_t *lookup(struct table *t, int key)
{
	int pos = hashCode(t,key);
	struct node *list = t->list[pos];
	struct node *temp = list;
	
	while(temp) {
		if (temp->key == key) {
			return temp->val;
		}
		temp = temp->next;
	}
	return NULL;
}

void job(char const *name, int load)
{
	int pid = syscall(SYS_gettid);
	printf("JOB %s (%d) start\n", name, pid);

		for (int i = 0; i < (10000 * load); i++)
			syscall(SYS_gettid);
	
	printf("JOB %s (%d) finish\n", name, pid);
}

pthread_t *start_job(int delay, void *(*routine) (void *))
{
	pthread_t *thread;
	pthread_attr_t attr_thread;

	int ret;

	if (nb_threads >= max_threads) {
		printf("*** max threads reached\n");
		return NULL;
	}


	thread = (pthread_t*) malloc(sizeof(pthread_t));
		pthread_attr_init(&attr_thread);
	
	threads[nb_threads] = thread;
	nb_threads++;


	if (delay > 0)
		usleep(delay);

	if ((ret = pthread_create(thread, &attr_thread, routine, NULL)) != 0) {
		printf("*** cannot create thread %s\n", strerror(ret));
		return NULL;
	}
			
	return thread;
}

int add_lock(int key)
{
	pthread_mutex_t *lock = (pthread_mutex_t*) malloc(sizeof(pthread_mutex_t));
	if (pthread_mutex_init(lock, &attr) != 0) {
		printf("*** mutex init failed\n");
		return -1;
	}

	insert(lock_table, key, lock);

	return 0;
}

int lock(int key)
{
	pthread_mutex_t *lock;

	lock = lookup(lock_table, key);
		pthread_mutex_lock(lock);

	return 0;
}

int unlock(int key)
{
	pthread_mutex_t *lock;

	lock = lookup(lock_table, key);
		pthread_mutex_unlock(lock);

	return 0;
}

int join()
{
	int i;

	for (i = 0; i < nb_threads; i++)
		pthread_join(*threads[i], NULL);
	
	return 0;
}

int set_protocol(int protocol)
{
	if (pthread_mutexattr_init(&attr) == -1) {
			perror("*** mutexattr_init error");
			return -1;
		}
		
		pthread_mutexattr_setprotocol(&attr, protocol);
	
	return 0;
}

int set_threads(int val)
{
	threads = (pthread_t**) malloc(sizeof(pthread_t*) * val);
	max_threads = val;
	nb_threads = 0;

	if (threads == NULL)
		return -1;

	return 0;
}

int set_locks(int nb_locks, ...)
{
	va_list ll;
	int lock, i;

	lock_table = create_table(nb_locks);

	va_start(ll, nb_locks);
	for (i = 0; i < nb_locks; i++) {
		lock = va_arg(ll, int);
		add_lock(lock);
	}
	va_end(ll);

	return 0;
}

#endif