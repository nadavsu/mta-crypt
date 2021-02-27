#include "include.h"
#include "launcher.h"

int main (int argc, char *argv[]) {

    if(argc < 4){
       printf("[ERROR]      too few arguments [# clients] -l [# password length] [-r optional] [# rounds]\n");
       exit(-1); 
    }
        
    int num_decrypters = atoi(argv[1]);
    char* rounds = NULL;
    if (argc == 6) {
    	rounds = argv[5];
    }
    

    if (vfork() == 0) {
    	char *server_argv[] = {SERVER_PROG, "-l", argv[3], 0};
    	execv(SERVER_PROG, server_argv);
    }

   wait(NULL);
    
    char buffer[10];
    
    for (int i = 0; i < num_decrypters; ++i) {
    	if (vfork() == 0) {
    	        sprintf(buffer, "%d",i+1);
    		char *decrypter_argv[] = {DECRYPTER_PROG, buffer, "-n", rounds, 0};
    		execv(DECRYPTER_PROG, decrypter_argv);
    	}
    }
    
    while(wait(NULL) != -1);
    return 0;
}
