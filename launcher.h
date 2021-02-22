#ifndef LAUNCHER_H
#define LAUNCHER_H

#include <unistd.h>
#include <stdlib.h>
#include <errno.h>

#define SERVER_NAME		"server.out"
#define DECRYPTER_NAME	"decrypter.oust"

int create_process(const char *process_name, char *argv[]);

#endif