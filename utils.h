#ifndef UTILS_H
#define UTILS_H

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

#define ENCRYPTER_NAME		    "[SERVER]"
#define MESSAGE_TYPE_INFO	    "[INFO]"
#define MESSAGE_TYPE_SUCC	    "[OK]"
#define MESSAGE_TYPE_ERR        "[ERROR]"

#define MALLOC_ERROR_MSG 	"Unable to allocate memory.\n"
#define MALLOC_ERROR_CODE	-1

#define THREAD_ERROR_MSG	"Unable to start thread.\n"
#define THREAD_ERROR_CODE	-2

#define MUTEX_ERROR_MSG		"Unable to initialize mutex.\n"
#define MUTEX_ERROR_CODE	-3

#define SYNCHRONIZED(MUTEX, COMMAND)	pthread_mutex_lock(MUTEX); COMMAND; pthread_mutex_unlock(MUTEX);

#define ENCRYPTER_RAND_SEED	time(NULL) / 2
#define DECRYPTER_RAND_SEED time(NULL) * 2

void create_n_threads(pthread_t *threads, int n, void *(*routine)(void*), void *arg);
void join_n_threads(pthread_t *threads, int n);
void *malloc_c(size_t size);
void pthread_mutex_init_c(pthread_mutex_t *t, const pthread_mutexattr_t *attr);
void message_stamp(const char* author, const char *message_type);
int printable_pass(char* decrypt_pass, int len);

#endif