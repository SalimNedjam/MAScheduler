#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/syscall.h>
#include <time.h>

int NB_CHILDS;
int NB_OTHERS;
#define N 			11

pthread_t childs[100];
pthread_t others[100];
pthread_mutex_t lock;

int cpt = 0;


// Function to get cofactor of mat[p][q] in temp[][]. n is current 
// dimension of mat[][] 
void getCofactor(int mat[N][N], int temp[N][N], int p, int q, int n) 
{ 
    int i = 0, j = 0; 
  
    // Looping for each element of the matrix 
    for (int row = 0; row < n; row++) 
    { 
        for (int col = 0; col < n; col++) 
        { 
            //  Copying into temporary matrix only those element 
            //  which are not in given row and column 
            if (row != p && col != q) 
            { 
                temp[i][j++] = mat[row][col]; 
  
                // Row is filled, so increase row index and 
                // reset col index 
                if (j == n - 1) 
                { 
                    j = 0; 
                    i++; 
                } 
            } 
        } 
    } 
} 
  
/* Recursive function for finding determinant of matrix. 
   n is current dimension of mat[][]. */
int determinantOfMatrix(int mat[N][N], int n) 
{ 
    int D = 0; // Initialize result 
  
    //  Base case : if matrix contains single element 
    if (n == 1) 
        return mat[0][0]; 
  
    int temp[N][N]; // To store cofactors 
  
    int sign = 1;  // To store sign multiplier 
  
     // Iterate for each element of first row 
    for (int f = 0; f < n; f++) 
    { 
        // Getting Cofactor of mat[0][f] 
        getCofactor(mat, temp, 0, f, n); 
        D += sign * mat[0][f] * determinantOfMatrix(temp, n - 1); 
  
        // terms are to be added with alternate sign 
        sign = -sign; 
    } 
  
    return D; 
} 

int job()
{
	srand(time(NULL));
	int mat[N][N];
	int i, j;
	for (i = 0; i < N; i++) {
		for (j = 0; j < N; j++) {
			mat[i][j] = rand() % 100;
		}
	}	
	return determinantOfMatrix(mat, N);
}

void *child_job(void *arg)
{
	cpt++;
	//printf("child %d/%d\n", cpt, NB_CHILDS);
	pthread_mutex_lock(&lock);

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

	job();


	exit(0);

	pthread_mutex_unlock(&lock);

	for (i = 0; i < NB_CHILDS; i++)
		pthread_join(childs[i], NULL);

	return NULL;
}

void *other_job(void *arg)
{	
	job();

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
