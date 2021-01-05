#include "encrypter.h"

volatile int created_passwords_counter;
password_t password = {0};

void encrypt(void *arg) {
	encrypter_args_t encrypter_args = (encrypter_args_t) arg;
	int timeout = encrypter_args.timeout;
	int password_length = encrypter_args.password_length;
	int key_length = password_length / 8;
	
	char *password = (char *) malloc_c(password_length * sizeof(char) + 1);
	char *encrypted_password = (char *) malloc_c(password_length * sizeof(char) + 1);
	char *key = (char *) malloc_c(key_length * sizeof(char) + 1);

	struct timespec timeout_timer;
	created_passwords_counter = 0;

	pthread_mutex_lock(&mutex);
	while (true) {
		generate_password(key, key_length, password, password_length, encrypted_password, password_length);
		created_passwords_counter++;
		update_current_password(encrypted_password, encrypted_password_length);	
		pthread_cond_broadcast(&decrypter_cond);
		wait_for_guesses();
	}
	pthread_mutex_unlock(&mutex);

}

void generate_password(char *key, int *key_length, char *password, int password_length, char *encrypterd_password, int encrypted_password_length) {
	MTA_get_rand_data(password, password_length);
	MTA_get_rand_data(key, key_length);
	MTA_CRYPT_RET_STATUS encrypt_ret_status = MTA_encrypt(key, key_length, password, password_length, encrypted_password, encrypted_password_length);
	assert(encrypt_ret_status == 0);
}

void update_current_password(char *encrypted_password, int encrypted_password_length) {
	password.encrypted_password = encrypted_password;
	password.password_length = password_length;
}

void wait_for_guesses() {
	clock_gettime(CLOCK_REALTIME, &timeout_timer);
	timeout.tv_sec += TIME_TO_WAIT;

	while (true) {
			res = pthread_cond_timedwait(&encrypter_cond, &mutex, &timeout_timer);
			if (res == EINVAL) {
				fprintf(stderr, "One of the values in pthread_cond_timedwait is invalid.\n");
				exit(-1);
			} else if (res == ETIMEDOUT) {
				printf("Timeout occured! Generating new password...\n");
				return;
			} else { 
				consume();
			}
		}
}