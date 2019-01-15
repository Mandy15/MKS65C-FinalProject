#include "networking.h"

char * process(char *s);
char * random_word();
char * word;
void subserver(int from_client);

// struct pair{
//   char words[32];
//   char defs[64];
// };
char *answers[6] = {"tump", "notionate", "rewild", "nugacity", "interloper" , "applesauce",
};
char *tablist[6] = {"a small mound, hill, or rise of ground",
                    "strong-willed or stubborn",
                    "to return (land) to a more natural state",
                    "triviality; insignificance",
                    "a person who interferes or meddles in the affairs of others",
                    "(Slang) nonsense; bunk",
};

char * random_word()
{
    srand( time(NULL));
    int i = rand() % 6;
    return answers[i];
}


char * process(char * s) {
  if(strcmp(s,word)){
    return "wrong";
  }else{
    return "correct";
  }
}

// void turns(){
//   int fd = open("clients.txt", O_RDONLY);
//   char * buf = malloc(6);
//   // onesec = clock()+(60*CLOCKS_PER_SEC);
//   //makes a client the hintgiver every turn (make a list of connected clients n go thru them)
// }

int wait_players(){
  int fd = open("clients.txt", O_RDONLY);
  char * buf = malloc(30);
  if(read(fd, buf, 30) == 30){
    close(fd);
    return 0;
  }
  close(fd);
  return 1;
}

void add_client(int pid){
  int fd = open("clients.txt", O_CREAT, 0600);
  close(fd);

  FILE *fp = fopen("clients.txt", "a");
  fprintf(fp, "%d\n", pid);
  fclose(fp);
}

int main() {
  int listen_socket;
  int f;
  listen_socket = server_setup();

  word = random_word();

  while (1) {

    int client_socket = server_connect(listen_socket);
    f = fork();
    if(f == 0){
      subserver(client_socket);
    }else{
      close(client_socket);
    }
  }
}

void subserver(int client_socket) {
  char buffer[BUFFER_SIZE];
  char wait[BUFFER_SIZE];

  add_client(getpid());
  int i = wait_players();
  while(i){
    strcpy(wait,"Waiting for other players...");
    write(client_socket, wait, sizeof(wait));
    i = wait_players();
  }

  strcpy(wait,"START");
  write(client_socket, wait, sizeof(wait));
  while (read(client_socket, buffer, sizeof(buffer))) {

    printf("[subserver %d] received: [%s]\n", getpid(), buffer);
    char * buf = process(buffer);
    write(client_socket, buf, sizeof(buf));
  }//end read loop
  close(client_socket);
  exit(0);
}
