#include "decrypter.h"

int printable_pass(char* decrypt_pass, len);

void *decrypt(void *thread_id) {

    long id = (long) thread_id;

    pthread_mutex_lock(&mutex);
    while (created_password_counter < 1) {
        pthread_cond_wait(&decrypter_cond, &mutex);
    }
    pthread_mutex_unlock(&mutex);


    pthread_mutex_lock(&mutex);
    int num_pass_created = created_password_counter;
    pthread_mutex_unlock(&mutex);

    int pass_len = password.password_len;
    int key_len = pass_len/8;

    char *encrypt_pass = malloc(sizeof(char) * pass_len + 1);

    pthread_mutex_lock(&mutex);
    memcpy(pass, password.encrypted_password, pass_len + 1);
    pthread_mutex_unlock(&mutex);

    char *key = malloc(sizeof(char) * key_len + 1);
    char *decrypt_pass = malloc(sizeof(char) * pass_len + 1);


    while (1) {

        pthread_mutex_lock(&mutex);
        if (num_pass_created < created_password_counter) {
            num_pass_created = created_password_counter;
            memcpy(encrypt_pass, password.encrypted_password, pass_len + 1);
        }
        pthread_mutex_unlock(&mutex);

        MTA_get_rand_data(key,key_len);
        MTA_CRYPT_RET_STATUS status = MTA_decrypt(key, key_len, encrypt_pass, pass_len, decrypt_pass, pass_len);

        if(status != MTA_CRYPT_RET_OK){
            printf("ERROR thread_id:%d -- at MTA_CRYPT",thread_id);
            continue;
        }

        if(printable_pass(decrypt_pass,pass_len)){
            pthread_mutex_lock(&mutex);
            printf("thread_id:%d , decrypt_pass:%s",thread_id,decrypt_pass);
            //push password to queue
            //increment pass sent
            pthread_cond_signal(encrypter_cond);
            pthread_cond_wait(&decrypter_cond, &mutex);
            pthread_mutex_unlock(&mutex);


        }
    }

}

int printable_pass(char* decrypt_pass, len){
    for(int i=0;i<len;i++){
        if (isprint(decrypt_pass[i]) == 0)
            return 0;
    }
    return 1;
}