#include "password_queue.h"

void init_password_queue(password_queue_t *queue) {
	queue->sent_passwords = 0;
	queue->checked_passwords = 0;
}

char *get_password(password_queue_t *queue) {
	char *last_password_guess = queue->passwords[queue->checked_passwords];
	queue->checked_passwords = (queue->checked_passwords + 1) % QUEUE_SIZE;
	return last_password_guess;
}

void put_password(password_queue_t *queue, char *password) {
	free(queue->passwords[queue->sent_passwords]);
	queue->passwords[queue->sent_passwords] = strdup(password);
	queue->sent_passwords = (queue->sent_passwords + 1) % QUEUE_SIZE;
}