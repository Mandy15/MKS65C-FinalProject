#include "pipe_networking.h"

static void sighandler(int signo){
  if(signo == SIGINT){
    remove("server");
    printf("\nServer has exited.\n");
    exit(0);
  }
}

int main() {
  signal(SIGINT, sighandler);

  int to_client;
  int from_client;

  int i;
  char buf[BUFFER_SIZE];

  while(1){
    from_client = server_handshake( &to_client );
    if(from_client){
      while(read(from_client, buf, BUFFER_SIZE)){
        printf("Server has received input from client...processing data...\n");
        printf("\"%s\"\n", buf);
        for(i = 0; i < strlen(buf); i++){
          buf[i] += 1;
        }
        write(to_client, buf, BUFFER_SIZE);
        printf("Server has sent data back to client...\n");
      }
      printf("Client has disconnected. Reset for new handshake.\n");
    }
  }
  return 0;
}
