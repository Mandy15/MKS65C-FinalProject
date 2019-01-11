#include "networking.h"

char * process(char *s);
char * random_word();
char * word;
void subserver(int from_client);

char *answers[6] = {"tump", "notionate", "rewild", "nugacity", "interloper" , "applesauce",
                  };

char * random_word()
{
    srand( time(NULL));
    int i = rand() % 6;
    return answers[i];
}

int main() {

  int listen_socket;
  int f;
  listen_socket = server_setup();

  word = random_word();

  while (1) {

    int client_socket = server_connect(listen_socket);
    f = fork();
    if (f == 0)
      subserver(client_socket);
    else
      close(client_socket);
  }
}

void subserver(int client_socket) {
  char buffer[BUFFER_SIZE];

  while (read(client_socket, buffer, sizeof(buffer))) {

    printf("[subserver %d] received: [%s]\n", getpid(), buffer);
    char * buf = process(buffer);
    write(client_socket, buf, sizeof(buf));
  }//end read loop
  close(client_socket);
  exit(0);
}

char * process(char * s) {
  if(strcmp(s,word)){
    return "wrong";
  }else{
    return "correct";
  }
}
