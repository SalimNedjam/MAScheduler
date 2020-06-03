#include <pthread.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/syscall.h>

#ifndef BENCH_H
#define BENCH_H

#define VNAME(varname) #varname

pthread_mutexattr_t attr;
struct table *lock_table;

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
    for (i=0;i<size;i++)
        t->list[i] = NULL;

    return t;
}

int hashCode(struct table *t, int key)
{
    if (key<0)
        return -(key%t->size);

    return key%t->size;
}

void insert(struct table *t , int key, pthread_mutex_t *val)
{
    int pos = hashCode(t,key);
    struct node *list = t->list[pos];
    struct node *newNode = (struct node*) malloc(sizeof(struct node));
    struct node *temp = list;

    while(temp){
        if(temp->key == key){
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
    
    while(temp){
        if(temp->key==key){
            return temp->val;
        }
        temp = temp->next;
    }
    return NULL;
}

void job(char *name, int load)
{
  int pid = syscall(SYS_gettid);
  printf("JOB %s (%d) start\n", name, pid);

	for (int i = 0; i < (10000 * load); i++)
		syscall(SYS_gettid);
  
  printf("JOB %s (%d) finish\n", name, pid);
}

int create(pthread_t *thread, int delay, void *(*routine) (void *))
{
	pthread_attr_t attr_thread;
  int ret;

	ret = pthread_attr_init(&attr_thread);
  
	if (delay > 0)
		usleep(delay);

  ret = pthread_create(thread, &attr_thread, routine, NULL);
  if (ret != 0)
		printf("cannot create thread %s\n", strerror(ret));
		
  return ret;
}

int add_lock(int key)
{
  pthread_mutex_t *lock = (pthread_mutex_t*) malloc(sizeof(pthread_mutex_t));
  if (pthread_mutex_init(lock, &attr) != 0) {
		printf("mutex init failed\n");
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

int bench_start(int protocol, int nb_lock, ...)
{
  va_list ll;
  int lock, i;

  if (pthread_mutexattr_init(&attr) == -1) {
		perror("mutexattr_init error");
		return -1;
	}
	
	pthread_mutexattr_setprotocol(&attr, protocol);
  
  lock_table = create_table(nb_lock);

  va_start(ll, nb_lock);
  for (i = 0; i < nb_lock; i++) {
    lock = va_arg(ll, int);
    add_lock(lock);
  }
  va_end(ll);

  printf("*** Start benchmark\n");

  return 0;
}


int bench_exit()
{
  printf("*** Exit benchmark\n");

  return 0;
}


#endif