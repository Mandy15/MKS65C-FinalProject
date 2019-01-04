#include "pipe_networking.h"


/*=========================
  server_handshake
  args: int * to_client

  Performs the client side pipe 3 way handshake.
  Sets *to_client to the file descriptor to the downstream pipe.

  returns the file descriptor for the upstream pipe.
  =========================*/
int server_handshake(int *to_client) {
  remove("server");
  int fd;
  if(mkfifo("server", 0644) == -1){
    printf("mkfifo error: %s\n", strerror(errno));
    exit(0);
  }
  printf("WKP server FIFO created...\n");

  printf("Server waiting for client...\n");
  fd = open("server", O_RDONLY);
  char buf[HANDSHAKE_BUFFER_SIZE];
  read(fd, buf, sizeof(buf));
  printf("Server has received message from client...\n");
  printf("\"%s\"\n", buf);

  int frk = fork();
  if(!f){
    printf("Remove WKP.\n");
    remove("server");
  }else{
    *to_client = open(buf, O_WRONLY);
    printf("Server has connected to client...\n");

    write(*to_client, ACK, sizeof(ACK));
    printf("Server has sent initial acknowledgement message to client...\n");

    char buf2[HANDSHAKE_BUFFER_SIZE];
    read(fd, buf2, sizeof(buf2));
    printf("Server has received response from client...\n");
    printf("\"%s\"\n", buf2);

    return fd;
  }
  return 0;
}


/*=========================
  client_handshake
  args: int * to_server

  Performs the client side pipe 3 way handshake.
  Sets *to_server to the file descriptor for the upstream pipe.

  returns the file descriptor for the downstream pipe.
  =========================*/
int client_handshake(int *to_server) {
  int fd;
  if(mkfifo("client", 0644) == -1){
    printf("mkfifo error: %s\n", strerror(errno));
    exit(0);
  }
  printf("Private client FIFO created...\n");

  *to_server = open("server", O_WRONLY);
  write(*to_server, "client", sizeof("client"));
  printf("Client has connected to server and sent private FIFO name...\n");

  fd = open("client", O_RDONLY);
  char buf[HANDSHAKE_BUFFER_SIZE];
  read(fd, buf, sizeof(buf));
  printf("Client has received message from server and removed its private FIFO...\n");
  printf("\"%s\"\n", buf);
  remove("client");

  write(*to_server, ACK, sizeof(ACK));
  printf("Client has sent response to server...\n");

  return fd;
}
