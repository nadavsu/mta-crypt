#include "utils.h"

/* 
 * non multi-threaded function that creates n threads from array of threads 'threads' with routine 'routine'.
 * Updates the number of threads created using thread_counter.
 * Exits with THREAD_ERROR_CODE if unable to create one of the threads.
 */
void create_n_threads(pthread_t *threads, int n, void *(*routine)(void*)) {
	int rc;
	for (int i = 0; i < n; i++) {
		rc = pthread_create(&threads[i], NULL, routine, (void *) i);
		if (rc != 0) {
			fprintf(stderr, "%s\n", THREAD_ERROR_MSG);
			exit(THREAD_ERROR_CODE);
		}
	}
}

//Joins n threads in thread array 'threads'
void join_n_threads(pthread_t *threads, int n) {
	for (int i = 0; i < n; i++) {
		pthread_join(threads[i], NULL);
	}
}


//Checked version of malloc.
void *malloc_c(size_t size) {
	void *mem = malloc(size);
	if (!mem) {
		fprintf(stderr, MALLOC_ERROR_MSG);
		exit(MALLOC_ERROR_CODE);
	}
	return mem;
}


//Checked version of pthread_mutex_init
void pthread_mutex_init_c(pthread_mutex_t *t, const pthread_mutexattr_t *attr) {
	int rc = pthread_mutex_init(mutex, NULL);
	if (rc != 0) {
		fprintf(stderr, MUTEX_ERROR_MSG);
		exit(MUTEX_ERROR_CODE);
	}
}