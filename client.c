#include "networking.h"
#define clear() printf("\033[H\033[J")

void setup(){
  clear();
  printf("                                  T A B O O\n\n");
}

char * read_line(){
  char *buf = malloc(64*sizeof(char));
  fgets(buf, 64, stdin);
  if(strlen(buf) != 0){
    int len = strlen(buf);
    if(buf[len-1] == '\n'){
      buf[len-1] = '\0';
    }
    return buf;
  }else{
    return NULL;
  }
}

int main(int argc, char **argv) {
  char *input = malloc(64*sizeof(char));

  setup();
  printf("Join game? (y/n) ");
  input = read_line();
  if(strcmp(input,"y") == 0){
    int server_socket;
    char buffer[BUFFER_SIZE];
    int points = 0;

    if (argc == 2){
      server_socket = client_setup( argv[1]);
    }else{
      server_socket = client_setup( TEST_IP );
    }

    printf("enter username: ");
    char *buf = malloc(64*sizeof(char));
    fgets(buf, 64, stdin);
    buf = strsep( &buf, "\n" );

    char *buf2 = malloc(300*sizeof(char));
    read(server_socket, buf2, sizeof(buf));
    printf("%s\n", buf2);
    while(strcmp(buf2, "START") != 0){
      read(server_socket, buf2, sizeof(buf));
    }
    printf("%s\n", buf2);

    while (1) {
      printf("%s: %d\nenter guess: ",buf, points);
      fgets(buffer, sizeof(buffer), stdin);
      *strchr(buffer, '\n') = 0;
      write(server_socket, buffer, sizeof(buffer));
      read(server_socket, buffer, sizeof(buffer));
      printf("[%s]\n", buffer);
      if(strcmp(buffer,"correct") == 0){
        points++;
      }
    }
  }else{
    printf("OK, goodbye.\n");
  }
  return 0;
}
