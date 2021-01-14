#include "main.h"

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t print_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t decrypter_cond   = PTHREAD_COND_INITIALIZER;
pthread_cond_t encrypter_cond   = PTHREAD_COND_INITIALIZER;

int main(int argc, char *argv[]) {
	if (argc < 5) {
		fprintf(stderr, "Missing arguments.\n");
		exit(-1);
	}
	unsigned int num_of_decypters = atoi(argv[2]);
	unsigned int password_length = atoi(argv[4]);
	unsigned int timeout = INT_MAX;
	if (argc >= 6 && argv[6] != NULL) {
		timeout = atoi(argv[6]);
	}

	pthread_t encrypter_thread;
	pthread_t decrypter_threads[num_of_decypters];

	ENCRYPTER_ARGS_T args;
	args.timeout = timeout;
	args.password_length = password_length;

	pthread_create(&encrypter_thread, NULL, encrypt, (void *)&args);
	for (long i = 0; i < num_of_decypters; i++) {
		pthread_create(&decrypter_threads[i], NULL, decrypt , (void *) (i + 1)); 
	}
	pthread_join(encrypter_thread, NULL);

	return 0;
}
