#include "main.h"

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t decrypter_cond   = PTHREAD_COND_INITIALIZER;
pthread_cond_t encrypter_cond   = PTHREAD_COND_INITIALIZER;

int main(int argc, char *argv[]) {
	int num_of_decypters = atoi(argv[2]);
	int password_length = atoi(argv[4]);
	int timeout = (argv[6] != NULL ? atoi(argv[6]) : INT_MAX);

	pthread_t encrypter_thread;
	pthread_t decrypter_threads[num_of_decypters];

	encrypter_args_t args;
	args.timeout = timeout;
	args.password_length = password_length;

	create_n_threads(&encrypter_thread, NUM_OF_ENCRYPTERS, encrypt, (void *)args);
	create_n_threads(decrypter_threads, num_of_decypters, decrypt, NULL);

	pthread_join(encrypter, NULL);
	//join decrypters?

	return 0;
}