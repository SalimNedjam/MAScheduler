#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/syscall.h>
#include <time.h>

#define N	11

int 	NB_OTHERS;
int	NB_CHILDS;
pthread_t childs[100];
pthread_t others[100];
pthread_mutex_t lock[100];
int cpt = 0;
int waiting_rang;

void get_cofactor(int mat[N][N], int temp[N][N], int p, int q, int n)
{
	int i = 0, j = 0;

	// Looping for each element of the matrix
	for (int row = 0; row < n; row++) {
		for (int col = 0; col < n; col++) {
			//  Copying into temporary matrix only those element
			//  which are not in given row and column
			if (row != p && col != q) {
				temp[i][j++] = mat[row][col];
				// Row is filled, so increase row index and
				// reset col index
				if (j == n - 1) {
					j = 0;
					i++;
				}
			}
		}
	}
}


void get_cofactor_more(int mat[N+2][N+2], int temp[N+2][N+2], int p, int q, int n)
{
	int i = 0, j = 0;

	// Looping for each element of the matrix
	for (int row = 0; row < n; row++) {
		for (int col = 0; col < n; col++) {
			//  Copying into temporary matrix only those element
			//  which are not in given row and column
			if (row != p && col != q) {
				temp[i][j++] = mat[row][col];

				// Row is filled, so increase row index and
				// reset col index
				if (j == n - 1) {
					j = 0;
					i++;
				}
			}
		}
	}
}

int determinant_of_matrix(int mat[N][N], int n)
{
	int D = 0; // Initialize result

	//  Base case : if matrix contains single element
	if (n == 1)
		return mat[0][0];

	int temp[N][N]; // To store cofactors

	int sign = 1;  // To store sign multiplier

	 // Iterate for each element of first row
	for (int f = 0; f < n; f++) {
		// Getting Cofactor of mat[0][f]
		get_cofactor(mat, temp, 0, f, n);
		D += sign * mat[0][f] * determinant_of_matrix(temp, n - 1);

		// terms are to be added with alternate sign
		sign = -sign;
	}

	return D;
}

int determinant_of_matrix_more(int mat[N+2][N+2], int n)
{
	int d = 0; // Initialize result

	//  Base case : if matrix contains single element
	if (n == 1)
		return mat[0][0];

	int temp[N+2][N+2]; // To store cofactors

	int sign = 1;  // To store sign multiplier

	 // Iterate for each element of first row
	for (int f = 0; f < n; f++) {10
		// Getting Cofactor of mat[0][f]
		get_cofactor_more(mat, temp, 0, f, n);
		d += sign * mat[0][f] * determinant_of_matrix_more(temp, n - 1);

		// terms are to be added with alternate sign
		sign = -sign;
	}

	return d;
}

int job()
{
	int mat[N][N];
	int i, j;

	srand(time(NULL));
	for (i = 0; i < N; i++) {
		for (j = 0; j < N; j++) {
			mat[i][j] = rand() % 100;
		}
	}

	return determinant_of_matrix(mat, N);
}

int job_more()
{
	int mat[N+2][N+2];
	int i, j;

	srand(time(NULL));
	for (i = 0; i < N+2; i++) {
		for (j = 0; j < N+2; j++) {
			mat[i][j] = rand() % 100;
		}
	}

	return determinant_of_matrix_more(mat, N+2);
}
void *child_job(void *arg)
{
	pthread_attr_t attr_thread;
	int i = 0, err;
	int rang = *((int *)arg);

	int child_rang = rang+1;


	while(waiting_rang < rang)
		;

	err = pthread_create(&(childs[i]), &attr_thread, &child_job, &child_rang);
	if (err != 0)
		printf("\ncan't create thread :[%s]", strerror(err));

	pthread_mutex_lock(&lock[rang]);
	pthread_mutex_lock(&lock[(rang)-1]);

	pthread_mutex_unlock(&lock[(rang)-1]);
	pthread_mutex_unlock(&lock[rang]);

	return NULL;
}

void *parent_job(void *arg)
{
	pthread_attr_t attr_thread;
	int i = 0, err;
	float start_time, end_time;
	
	pthread_mutex_lock(&lock[0]);

	pthread_attr_init(&attr_thread);

	int rang = 1;
	err = pthread_create(&(childs[i]), &attr_thread, &child_job, &rang);
	if (err != 0)
	 	printf("\ncan't create thread :[%s]", strerror(err));

	for(i=0; i<NB_CHILDS; i++) {
		waiting_rang+=1;
		start_time = (float) clock()/CLOCKS_PER_SEC;
		job();
		end_time = (float) clock()/CLOCKS_PER_SEC;
		printf("With %d child: %f\n", i, end_time-start_time );

	}

	exit(0);

	pthread_mutex_unlock(&lock[0]);

	for (i = 0; i < NB_CHILDS; i++)
		pthread_join(childs[i], NULL);

	return NULL;
}

void *other_job(void *arg)
{
	job_more();

	return NULL;
}


void bench(const pthread_mutexattr_t *attr)
{
	pthread_attr_t attr_thread;
	int i = 0, err;

	for(int i=0 ;i<= NB_CHILDS; i++)
		if (pthread_mutex_init(&lock[i], attr) != 0) {
			printf("\n mutex init failed\n");
			return;
		}

	pthread_attr_init(&attr_thread);
	while (i < NB_OTHERS) {
		err = pthread_create(&(others[i]), &attr_thread, &other_job, NULL);
		if (err != 0)
			printf("\ncan't create thread :[%s]", strerror(err));
		i++;
	}

	pthread_t *parent = (pthread_t*) malloc(sizeof(pthread_t));
	err = pthread_create(parent, &attr_thread, &parent_job, NULL);

	if (err != 0)
		printf("\ncan't create thread :[%s]", strerror(err));

	pthread_join(*parent, NULL);

}

int main(int argc, const char **argv)
{
	pthread_mutexattr_t attr;
	NB_CHILDS =  atoi(argv[1]);
	NB_OTHERS =  atoi(argv[2]);

	if (pthread_mutexattr_init(&attr) == -1) {
		perror("mutexattr_init error");
		return -1;
	}

	pthread_mutexattr_setprotocol(&attr, PTHREAD_PRIO_INHERIT);

	bench(&attr);

	return 0;
}
