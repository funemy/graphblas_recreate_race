#include <stdlib.h>
#include <assert.h>
#include <pthread.h>
#include "../deps/GraphBLAS/Include/GraphBLAS.h"

pthread_mutex_t mutex;

int DIMENSIONS = 10;

void *RACETEST_THREAD(void *matrix_ptr) {
	GrB_Matrix B = *(GrB_Matrix *)matrix_ptr;

	pthread_mutex_lock(&mutex); // lock mutex

	// Shrink matrix
	GrB_Info res = GxB_Matrix_resize(B, DIMENSIONS, DIMENSIONS); // NOTE - 1 thread is here
	if(res != GrB_SUCCESS) {
		printf("%s\n", GrB_error());
	}

	// Flush pending if necessary
	bool pending = false;
	GxB_Matrix_Pending(B, &pending);
	if(pending) {
		GrB_Index nvals;
		GrB_Matrix_nvals(&nvals, B);
	}

	pthread_mutex_unlock(&mutex); // Unlock

	GrB_Matrix A; // Operand 1
	GrB_Matrix C; // Output matrix
	GrB_Matrix_new(&A, GrB_BOOL, DIMENSIONS, DIMENSIONS);
	GrB_Matrix_new(&C, GrB_BOOL, DIMENSIONS, DIMENSIONS);

	GrB_mxm(C, GrB_NULL, GrB_NULL, GxB_LOR_LAND_BOOL, A, B, GrB_NULL); // NOTE - 1 thread is here

	printf("Thread's execution completed.\n");
}

int main() {
	GrB_init(GrB_NONBLOCKING);

	assert(pthread_mutex_init(&mutex, NULL) == 0);

	GrB_Matrix B; // Operand 2
	GrB_Matrix_new(&B, GrB_BOOL, 16384, 16384);

	pthread_t thread1, thread2;
	pthread_create(&thread1, NULL, RACETEST_THREAD, &B);
	pthread_create(&thread2, NULL, RACETEST_THREAD, &B);

	pthread_join(thread1, NULL);
	pthread_join(thread2, NULL);

	return 0;
}
