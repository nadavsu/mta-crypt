#ifndef UTILS_H
#define UTILS_H

#define MALLOC_ERROR_MSG 	"Unable to allocate memory.\n"
#define MALLOC_ERROR_CODE	-1

#define THREAD_ERROR_MSG	"Unable to start thread.\n"
#define THREAD_ERROR_CODE	-2

#define MUTEX_ERROR_MSG		"Unable to initialize mutex.\n"
#define MUTEX_ERROR_CODE	-3

#define SYNCHRONIZED(MUTEX, COMMAND)	pthread_mutex_lock(MUTEX); COMMAND; pthread_mutex_unlock(MUTEX);

void create_n_threads(pthread_t *threads, int n, void *(*routine)(void*));
void join_n_threads(pthread_t *threads, int n);
void *malloc_c(size_t size);
void pthread_mutex_init_c(pthread_mutex_t *t, const pthread_mutexattr_t *attr);

#endif