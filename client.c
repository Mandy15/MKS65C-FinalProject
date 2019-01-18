#include "networking.h"
#define clear() printf("\033[H\033[J")
#define MAX_PLAYERS 5

int server;
char ** usernames;
int * points;
int player;
int hinter;
char * word;
char ** tlist;
int * corrects;

char * read_line(char * line){
  fgets(line, 64, stdin);
  if(strlen(line) != 0){
    int len = strlen(line);
    if(line[len-1] == '\n'){
      line[len-1] = '\0';
    }
    return line;
  }else{
    return NULL;
  }
}

void setup(char * ip){
  int i;
  clear();

  usernames = calloc(sizeof(char*), MAX_PLAYERS);
  points = calloc(sizeof(int), MAX_PLAYERS);
  corrects = calloc(sizeof(int), MAX_PLAYERS-1);
  for (i = 0; i < MAX_PLAYERS; i++) {
    points[i] = 0;
  }

  server = client_setup(ip);
  printf("Waiting for other players...\n");

  char * id = calloc(sizeof(char), 2);
  read(server, id, 2);
  player = atoi(id);
  free(id);

  clear();
  printf("Enter username: ");
  char * buf = malloc(64*sizeof(char));
  fgets(buf, 64, stdin);
  buf = strsep( &buf, "\n" );
  write(server, buf, 64);
  free(buf);

  for (i = 0; i < MAX_PLAYERS; i++) {
    char * name = calloc(sizeof(char), 64);
    read(server, name, 64);
    usernames[i] = name;
  }
}

//HINTER
void roles() {
  char* response = calloc(sizeof(char), 2);
  read(server, response, 2);
  hinter = atoi(response);
  printf("%s is the hinter.\n", usernames[hinter]);
}

void receive_words(){
  char buffer[BUFFER_SIZE];
  read(server, buffer, sizeof(buffer));
  word = buffer;
  for(int i = 0; i < 3; i++){
    char buf[BUFFER_SIZE];
    read(server, buf, sizeof(buf));
    tlist[i] = buf;
  }
  printf("Word: %s\nTaboo Words: %s, %s, %s", word, tlist[0], tlist[1], tlist[2]);
}

void give_hint(){
  char buffer[11];
  printf("Enter hint (limit of 10 characters): ");
  fgets(buffer, sizeof(buffer), stdin);
  *strchr(buffer, '\n') = 0;
  write(server, buffer, sizeof(buffer));
  printf("[HINT]%s: %s", usernames[hinter], buffer);
}

void checked_hint(){
  char buffer[BUFFER_SIZE];
  read(server, buffer, sizeof(buffer));
  if(strcmp(buffer, "X") == 0){
    printf("Invalid hint. Send a different one.\n");
    give_hint();
  }
}

//GUESSER
void receive_hint(){
  char buffer[BUFFER_SIZE];
  read(server, buffer, sizeof(buffer));
  printf("[HINT]%s: %s", usernames[hinter], buffer);
}

void guess(){
  char buffer[BUFFER_SIZE];
  printf("Enter guess: ");
  fgets(buffer, sizeof(buffer), stdin);
  *strchr(buffer, '\n') = 0;
  write(server, buffer, sizeof(buffer));
}

void display(){
  for(int i = 0; i < MAX_PLAYERS; i++){
    if(i != hinter){
      char buffer[BUFFER_SIZE];
      read(server, buffer, sizeof(buffer));
      printf("[GUESS]%s: %s", usernames[i], buffer);
    }
  }
}

void correct_guessers(){
  char * response = calloc(sizeof(char), 2);
  read(server, response, 2);
  int c = atoi(response);
  for(int i = 0; i < MAX_PLAYERS-1; i++){
    char * c_num = calloc(sizeof(char), 2);
    read(server, c_num, 2);
    corrects[i] = atoi(c_num);;
  }
  if(c == 0){
    printf("No one guessed [%s] correctly. New round.\n", word);
    return;
  }
  points[hinter] += c;
  int j = 0;
  while(c > 0){
    printf("%s ", usernames[corrects[j]]);
    points[corrects[j]]++;
    j++;
    c--;
  }
  printf("guessed [%s] correctly!\n", word);
}

//WINNER
void end(){
  int winner;
  char * w = calloc(sizeof(char), 2);
  read(server, w, 2);
  winner = atoi(w);
  free(w);
  if(winner != -1){
    clear();
    printf("Game Over\n");
    printf("WINNER: %s - %d points\n", usernames[winner], points[winner]);
    for(int i = 0; i < MAX_PLAYERS; i++){
      if(i != winner){
        printf("%s - %d points\n", usernames[i], points[i]);
      }
    }
    exit(EXIT_SUCCESS);
  }
}

void start(){
  while(1){
    roles();
    if(player == hinter){
      receive_words();
      give_hint();
      checked_hint();
    }else{
      receive_hint();
      guess();
    }
    display();
    correct_guessers();
    end();
  }
}

int main(int argc, char* argv[]){
  if(argc == 2){
    setup(argv[1]);
  }else{
    setup(TEST_IP);
  }
  printf("                                  T A B O O\n\n");
  start();

  return 0;
}
