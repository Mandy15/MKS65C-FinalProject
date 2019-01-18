#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <signal.h>
#include <limits.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <string.h>
#include <time.h>
#include <errno.h>

#ifndef NETWORKING_H
#define NETWORKING_H

#define BUFFER_SIZE 256

#define PORT "54321"
#define TEST_IP "127.0.0.1"

struct taboo {
  char answer[20];
  char * tabo[4];
};

void error_check(int i, char *s);
int server_setup();
int server_connect(int sd);
int client_setup(char * server);


#endif
