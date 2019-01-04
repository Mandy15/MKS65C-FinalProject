#include "pipe_networking.h"

static void sighandler(int signo){
  if(signo == SIGINT){
    printf("\nClient has exited.\n");
    exit(0);
  }
}

int main() {
  signal(SIGINT, sighandler);

  int to_server;
  int from_server;

  from_server = client_handshake( &to_server );

  char buf[BUFFER_SIZE];
  while(1){
    printf("Enter input to send to server: ");
    fgets(buf, BUFFER_SIZE, stdin);
    buf[strlen(buf)-1] = '\0';
    write(to_server, buf, BUFFER_SIZE);

    read(from_server, buf, BUFFER_SIZE);
    printf("Client has received message from server...\n");
    printf("\"%s\"\n", buf);
  }
  return 0;
}
