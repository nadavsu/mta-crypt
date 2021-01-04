#include "mta_crypt.h"

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;


int main(int argc, char *argv[]) {
	int num_of_decypters = atoi(argv[2]);
	int password_length = atoi(argv[4]);
	int timeout = (argv[6] != NULL ? atoi(argv[6]) : INT_MAX);

	pthread_t encrypter_thread;
	pthread_t decrypter_threads[num_of_decypters];

	create_n_threads(decrypter_threads, num_of_decypters, decrypt);
	create_n_threads(&encrypter_thread, NUM_OF_ENCRYPTERS, encrypt);

	//create timer

	while (true) {
		//generate a key.
		//generate a password.
		//encrypt password.
		//add it to the struct.
		//broadcast to decryptors about a new password and start timer.
		//wait for a decryptor to update the found_password field in the password or timeout.
	}

	return 0;
}