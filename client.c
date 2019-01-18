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
  tlist = calloc(sizeof(char*), 4);
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
  if(player == hinter){
    printf("You are the hinter.\n");
  }else{
    printf("%s is the hinter.\n", usernames[hinter]);
  }
}

void receive_words(){
  char * buffer = calloc(sizeof(char), 64);
  read(server, buffer, sizeof(buffer));
  word = buffer;
  printf("Word: %s\nTaboo List:\n", word);
  for(int i = 0; i < 4; i++){
    char * buf = calloc(sizeof(char), 64);
    read(server, buf, sizeof(buf));
    tlist[i] = buf;
    printf("%s\n", tlist[i]);
  }
}

void give_hint(){
  printf("Enter hint (limit of 20 characters): ");
  char * buf = calloc(sizeof(char), 21);
  fgets(buf, 21, stdin);
  buf = strsep( &buf, "\n" );
  // printf("[HINT]%s: %s\n", usernames[hinter], buf);
  write(server, buf, sizeof(buf));
}

int checked_hint(){
  char buffer[BUFFER_SIZE];
  read(server, buffer, sizeof(buffer));
  if(strcmp(buffer, "X") == 0){
    printf("Invalid hint. Skipped turn.\n");
    return 1;
  }
  return 0;
}

//GUESSER
void receive_hint(){
  char * buffer = calloc(sizeof(char), 20);
  read(server, buffer, sizeof(buffer));
  printf("[HINT]%s: %s\n", usernames[hinter], buffer);
}

void guess(){
  char * buffer = calloc(sizeof(char), 64);
  printf("Enter guess: ");
  fgets(buffer, sizeof(buffer), stdin);
  *strchr(buffer, '\n') = '\0';
  write(server, buffer, sizeof(buffer));
}

void display(){
  for(int i = 0; i < MAX_PLAYERS-1; i++){
    char * buffer = calloc(sizeof(char), 64);
    read(server, buffer, sizeof(buffer));
    if(i == hinter){
      continue;
    }
    printf("%s: %s\n", usernames[i], buffer);
  }
}

void correct_guessers(){
  char * buffer = calloc(sizeof(char), 2);
  read(server, buffer, 2);
  int c = atoi(buffer);
  for(int i = 0; i < c; i++){
    char* buffer = calloc(sizeof(char), 2);
    read(server, buffer, 2);
    corrects[i] = atoi(buffer);
  }
  int index = 0;
  for(int i = 0; i < c; i++){
    if(i != hinter){
      printf("%s guessed correctly!\n", usernames[corrects[index]]);
      points[corrects[index]] += 1;
      index++;
    }
  }
}

//WINNER
void end_round(){
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
      if(checked_hint()){
        continue;
        continue;
      }
    }else{
      receive_hint();
      guess();
    }
    display();
    correct_guessers();
    end_round();
    int i;
    for (i = 0; i < MAX_PLAYERS; i++) {
      printf("%s: %d\n", usernames[i], points[i]);
    }
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
}
