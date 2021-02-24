#include "launcher.h"

int main (int argc, char *argv[]) {
    int num_of_decrypters = atoi(argv[1]);
    char *num_of_round = NULL;
    if (argc == 4) {
    	num_of_round = argv[3];
    }

    /*
     * Creating server.
     */
    if (vfork() == 0) {
    	char *server_argv[2] = {SERVER_NAME, 0};
    	execv(SERVER_NAME, server_argv);
    }

    for (int i = 0; i < num_of_decrypters; ++i) {
    	if (vfork() == 0) {
    		char *decrypter_argv[] = {DECRYPTER_NAME, "-n", num_of_round};
    		execv(DECRYPTER_NAME, decrypter_argv);
    	}
    }
    return 0;
}
