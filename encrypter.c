#include "encrypter.h"

PASSWORD_QUEUE_T password_queue;
volatile int created_passwords_counter = 0;
volatile PASSWORD_T encrypted_password;

void get_printable_data(char *buffer, int length);

char *original_password;
unsigned int password_length;
char *key;
unsigned int key_length;
unsigned int timeout_time;
struct timespec timeout_timer;

void *encrypt(void *arg) {
    ENCRYPTER_ARGS_T *encrypter_args = (ENCRYPTER_ARGS_T *) arg;
    timeout_time = encrypter_args->timeout;
    password_length = encrypter_args->password_length;
    key_length = password_length / 8;

    encrypted_password.length = password_length;
    encrypted_password.password = (char *) malloc_c(password_length * sizeof(char) + 1);
    original_password = (char *) malloc_c(password_length * sizeof(char) + 1);
    key = (char *) malloc_c(key_length * sizeof(char) + 1);

    DECRYPTED_PASSWORD_T decrypted_password;

    while (1) {
         
        int is_decrypted = 0;
        pthread_mutex_lock(&mutex);
        init_password_queue(&password_queue);
        generate_password();
        
        if (created_passwords_counter == 1)
             pthread_cond_broadcast(&decrypter_cond);
             
        clock_gettime(CLOCK_REALTIME, &timeout_timer);
        timeout_timer.tv_sec += timeout_time;
        
        while(!is_decrypted) {
            int res = pthread_cond_timedwait(&encrypter_cond, &mutex, &timeout_timer);
            if (res == EINVAL) {
                pthread_mutex_lock(&print_mutex);
                message_stamp(ENCRYPTER_NAME, MESSAGE_TYPE_ERR);
                printf("One of the values in pthread_cond_timedwait is invalid. Exiting...\n");
                pthread_mutex_unlock(&print_mutex);
                pthread_mutex_unlock(&mutex);
                exit(-1);
            } else if (res == ETIMEDOUT) {
                pthread_mutex_lock(&print_mutex);
                message_stamp(ENCRYPTER_NAME, MESSAGE_TYPE_ERR);
                printf("Timeout occurred (%ds)! Generating new password...\n", timeout_time);
                pthread_mutex_unlock(&print_mutex);
                pthread_mutex_unlock(&mutex);
                break;
            } else {
                while (password_queue.checked_passwords < password_queue.sent_passwords) {
                    decrypted_password = get_password(&password_queue);
                    pthread_mutex_unlock(&mutex);
                    is_decrypted = check_password(decrypted_password);
                    if (is_decrypted) {
                        break;
                    }
                    pthread_mutex_lock(&mutex);
                }
            }
        }
    }
    pthread_mutex_unlock(&mutex);

}

 
 void get_printable_data(char *buffer, int length){
    int i=0;
    while(i < length){
    	char ch;
    	while(!isprint(ch = MTA_get_rand_char())){}
    	buffer[i] = ch;
    	i++; 
    }
 }
 
char *generate_password() {
    get_printable_data(original_password, password_length);
    MTA_get_rand_data(key, key_length);
    
    srand(ENCRYPTER_RAND_SEED);
    MTA_CRYPT_RET_STATUS status = MTA_encrypt(key, key_length, original_password, password_length,
                                              encrypted_password.password,
                                              &password_length);
    if (status != MTA_CRYPT_RET_OK) {
        printf("ERROR at encrypter -- at MTA_CRYPT");
        return NULL;
    }
    created_passwords_counter++;
    pthread_mutex_lock(&print_mutex);
    message_stamp(ENCRYPTER_NAME, MESSAGE_TYPE_INFO);
    printf("New password generated: %s - key: %s - encrypted password: %s\n", original_password, key,
           encrypted_password.password);
    pthread_mutex_unlock(&print_mutex);
}

int check_password(DECRYPTED_PASSWORD_T decrypted_password) {
    if (decrypted_password.password) {
        if (strcmp(original_password, decrypted_password.password) == 0) {
            pthread_mutex_lock(&print_mutex);
            message_stamp(ENCRYPTER_NAME, MESSAGE_TYPE_SUCC);
            printf("Password decrypted successfully by client #%d, received - %s original - %s\n", decrypted_password.thread_id,
                   decrypted_password.password, original_password);
           pthread_mutex_unlock(&print_mutex);
            return 1;
        } else {
            pthread_mutex_lock(&print_mutex);
            message_stamp(ENCRYPTER_NAME, MESSAGE_TYPE_ERR);
            printf("Wrong password received from client #%d, received - %s original - %s\n", decrypted_password.thread_id, decrypted_password.password,
                   original_password);
            pthread_mutex_unlock(&print_mutex);
            return 0;
        }
    }
    return 0;
}
