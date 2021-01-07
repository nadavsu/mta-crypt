#include "encrypter.h"

password_queue_t password_queue;
volatile int created_passwords_counter;
volatile password_t encrypted_password;

char *original_password;
unsigned int password_length;
char *key;
unsigned int key_length;

unsigned int timeout_time;
struct timespec timeout_timer;

void *encrypt(void *arg) {
    encrypter_args_t *encrypter_args = (encrypter_args_t *) arg;
    timeout_time = encrypter_args->timeout;
    password_length = encrypter_args->password_length;
    key_length = password_length / 8;

    encrypted_password.length = password_length;
    encrypted_password.password = (char *) malloc_c(password_length * sizeof(char) + 1);
    original_password = (char *) malloc_c(password_length * sizeof(char) + 1);
    key = (char *) malloc_c(key_length * sizeof(char) + 1);
    created_passwords_counter = 0;

    while (1) {
        pthread_mutex_lock(&mutex);
        init_password_queue(&password_queue);
        generate_password();

        clock_gettime(CLOCK_REALTIME, &timeout_timer);
        timeout_timer.tv_sec += timeout_time;
        pthread_cond_broadcast(&decrypter_cond);

        while (1) {
            char *last_password_guess = NULL;
            int res = pthread_cond_timedwait(&encrypter_cond, &mutex, &timeout_timer);
            if (res == EINVAL) {
                message_stamp(ENCRYPTER_NAME, MESSAGE_TYPE_ERR);
                printf("One of the values in pthread_cond_timedwait is invalid. Exiting...\n");
                pthread_mutex_unlock(&mutex);
                exit(-1);
            } else if (res == ETIMEDOUT) {
                message_stamp(ENCRYPTER_NAME, MESSAGE_TYPE_ERR);
                printf("Timeout occurred (%ds)! Generating new password...\n", timeout_time);
                pthread_mutex_unlock(&mutex);
                break;
            } else {
                while (password_queue.checked_passwords < password_queue.sent_passwords) {
                    last_password_guess = get_password(&password_queue);
                    pthread_mutex_unlock(&mutex);
                    if (check_password(last_password_guess)) {
                        break;
                    }
                }
            }
        }
    }
    pthread_mutex_unlock(&mutex);

}

/*
 * Generates a new password and puts it in encrypted_password global volatile variable.
 */
char *generate_password() {
    do {
        MTA_get_rand_data(original_password, password_length);
    } while (!printable_pass(original_password, password_length));
    do {
        MTA_get_rand_data(key, key_length);
    } while (!printable_pass(key, key_length));

    MTA_CRYPT_RET_STATUS status = MTA_encrypt(key, key_length, original_password, password_length,
                                              encrypted_password.password,
                                              &password_length);
    if (status != MTA_CRYPT_RET_OK) {
        printf("ERROR at encrypter -- at MTA_CRYPT");
        return NULL;
    }
    created_passwords_counter++;
    message_stamp(ENCRYPTER_NAME, MESSAGE_TYPE_INFO);
    printf("New password generated: %s - key: %s - encrypted password: %s\n", original_password, key,
           encrypted_password.password);
}

int check_password(char *decrypted_password) {
    if (decrypted_password) {
        if (strcmp(original_password, decrypted_password) == 0) {
            message_stamp(ENCRYPTER_NAME, MESSAGE_TYPE_SUCC);
            printf("Password decrypted successfully by client #%d, received - %s original - %s\n", 0,
                   decrypted_password, original_password);
            return 1;
        } else {
            message_stamp(ENCRYPTER_NAME, MESSAGE_TYPE_ERR);
            printf("Wrong password received from client #%d, received - %s original - %s\n", 0, decrypted_password,
                   original_password);
            return 0;
        }
    }
    return 0;
}


