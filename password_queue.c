#include "password_queue.h"

void init_password_queue(PASSWORD_QUEUE_T *queue) {
	int i;

	queue->sent_passwords = 0;
	queue->checked_passwords = 0;

	for (i = 0; i < QUEUE_SIZE; i++) {
		queue->passwords[i].thread_id = -1;
	}

}

DECRYPTED_PASSWORD_T get_password(PASSWORD_QUEUE_T *queue) {
	DECRYPTED_PASSWORD_T password = queue->passwords[queue->checked_passwords];
	queue->checked_passwords = (queue->checked_passwords + 1) % QUEUE_SIZE;
	return password;
}

void put_password(PASSWORD_QUEUE_T *queue, char *password, int thread_id) {
	free(queue->passwords[queue->sent_passwords].password);
	queue->passwords[queue->sent_passwords].password = strdup(password);
	queue->passwords[queue->sent_passwords].thread_id = thread_id;
	queue->sent_passwords = (queue->sent_passwords + 1) % QUEUE_SIZE;
}