#include "decrypter.h"

int printable_pass(char* decrypt_pass, int len);

void *decrypt(void *thread_id) {

    long id = (long) thread_id;
    int iterations = 0;

    pthread_mutex_lock(&mutex);
    while (created_passwords_counter < 1) {
        pthread_cond_wait(&decrypter_cond, &mutex);
    }
    pthread_mutex_unlock(&mutex);


    pthread_mutex_lock(&mutex);
    int num_pass_created = created_passwords_counter;
    pthread_mutex_unlock(&mutex);

    int pass_len = encrypted_password.length;
    int key_len = pass_len/8;

    char *encrypt_pass = malloc(sizeof(char) * pass_len + 1);

    pthread_mutex_lock(&mutex);
    memcpy(encrypt_pass, encrypted_password.password, pass_len + 1);
    pthread_mutex_unlock(&mutex);

    char *key = malloc(sizeof(char) * key_len + 1);
    char *decrypt_pass = malloc(sizeof(char) * pass_len + 1);


    while (1) {
        pthread_mutex_lock(&mutex);
        if (num_pass_created < created_passwords_counter) {
            num_pass_created = created_passwords_counter;
            memcpy(encrypt_pass, encrypted_password.password, pass_len + 1);
            iterations = 0;
        }
        pthread_mutex_unlock(&mutex);

        MTA_get_rand_data(key,key_len);
        MTA_CRYPT_RET_STATUS status = MTA_decrypt(key, key_len, encrypt_pass, pass_len, decrypt_pass, pass_len);

        if(status != MTA_CRYPT_RET_OK){
            printf("ERROR thread_id:%ld -- at MTA_CRYPT", id);
            continue;
        }

        if(printable_pass(decrypt_pass,pass_len)){
            pthread_mutex_lock(&mutex);
            message_stamp(DECRYPTER_NAME(id), MESSAGE_TYPE_INFO);
            printf("Decrypted password - %s key guessed -  %s Sending to server after %d iterations\n", decrypt_pass, key, iterations);
            put_password(&password_queue, decrypt_pass);
            pthread_cond_signal(&encrypter_cond);
            pthread_cond_wait(&decrypter_cond, &mutex);
            pthread_mutex_unlock(&mutex);
        }
        iterations++;
    }

}

int printable_pass(char* decrypt_pass, int len){
    for(int i=0;i<len;i++){
        if (isprint(decrypt_pass[i]) == 0)
            return 0;
    }
    return 1;
}