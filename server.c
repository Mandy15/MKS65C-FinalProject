#include "networking.h"
#define clear() printf("\033[H\033[J")
#define MAX_PLAYERS 5
#define MAX_POINTS 10

char * keys[6] = {"cat", "window", "Mr. K", "chair", "door" , "Stuy"
                  };
char * values[6][4] = { {"cat","feline","kitty","tabby"}, {"window","glass","wall","look"},
                        {"Mr. K","teacher","systems","computer"},
                        {"chair","seat","butt","sit"}, {"door","entrance","doorway","exit"}, {"Stuy","school","here","hell"},
                    };

struct taboo hint;
char ** usernames;
int * clients;
int * points;
char ** guesses;
int hinter;
int next_hinter;
int * corrects;

static void sighandler(int signo) {
  if (signo == SIGINT) {
    printf("\n");
    exit(EXIT_SUCCESS);
  }
}

void setup(char * ip){
  int i,j;
  signal(SIGINT, sighandler);

  clear();
  clients = calloc(sizeof(int), MAX_PLAYERS);
  usernames = calloc(sizeof(char*), MAX_PLAYERS);
  points = calloc(sizeof(int), MAX_PLAYERS);
  guesses = calloc(sizeof(char*), MAX_PLAYERS-1);
  corrects = calloc(sizeof(int), MAX_PLAYERS-1);
  for (i = 0; i < MAX_PLAYERS; i++) {
    points[i] = 0;
  }

  int listen_socket = server_setup();
  int p = 0;
  while (p < MAX_PLAYERS) {
    printf("Waiting for other players...\n");
    clients[p] = server_connect(listen_socket);
    p++;
  }
  close(listen_socket);
  printf("START GAME\n");

  for (i = 0; i < MAX_PLAYERS; i++) {
    char* t = calloc(sizeof(char), 2);
    sprintf(t, "%d", i);
    write(clients[i], t, 2);
    free(t);
  }

  usernames = calloc(sizeof(char*), MAX_PLAYERS);
  for (i = 0; i < MAX_PLAYERS; i++) {
    char * name = calloc(sizeof(char), 64);
    read(clients[i], name, 64);
    usernames[i] = name;
  }

  for (i = 0; i < MAX_PLAYERS; i++) {
    for (j = 0; j < MAX_PLAYERS; j++) {
      write(clients[i], usernames[j], 64);
    }
  }
}

//HINTER
void roles() {
  hinter = (hinter+1) % MAX_PLAYERS;
  char * h = calloc(sizeof(char), 2);
  sprintf(h, "%d", hinter);
  for (int i = 0; i < MAX_PLAYERS; i++) {
    write(clients[i], h, 2);
  }
  free(h);
}

void random_word(){
  int i = rand() % 6;
  strcpy(hint.answer, keys[i]);
  for(int x = 0; x < 4; x++){
    hint.tabo[x] = values[i][x];
  }
}

void send_word(){
  random_word();
  char * buffer = calloc(sizeof(char), 64);
  buffer = hint.answer;
  printf("buffer: %s\n", buffer);
  write(clients[hinter], buffer, sizeof(buffer));
  for(int i = 0; i < 4; i++){
    char * buf = calloc(sizeof(char), 64);
    buf = hint.tabo[i];
    printf("buf: %s\n", buf);
    write(clients[hinter], buf, sizeof(buf));
  }
}

int receive_hint(){
  char * buffer = calloc(sizeof(char), 20);
  read(clients[hinter], buffer, sizeof(buffer));
  for(int i = 0; i < 4; i++){
    if(strstr(buffer, hint.tabo[i])){
      char * buf1 = calloc(sizeof(char), 2);
      buf1 = "X";
      write(clients[hinter], buf1, sizeof(buf1));
      return 1;
    }
  }
  for (int i = 0; i < MAX_PLAYERS; i++) {
    if(i == hinter){
      write(clients[hinter], buffer, sizeof(buffer));
    }else{
      write(clients[i], buffer, sizeof(buffer));
    }
  }
  return 0;
}

//GUESSER
void get_guesses(){
  int x = 0;
  for(int i = 0; i < MAX_PLAYERS; i++){
    if(i != hinter){
      char * buffer = calloc(sizeof(char), 64);
      read(clients[i], buffer, sizeof(buffer));
      guesses[x] = buffer;
      x++;
    }
  }
  x = 0;
  for(int i = 0; i < MAX_PLAYERS; i++){
    if(i != hinter){
      printf("%s: %s\n", usernames[i], guesses[x]);
      x++;
    }
  }
  for(int i = 0; i < MAX_PLAYERS; i++){
    for(int j = 0; j < MAX_PLAYERS-1; j++){
      char * buffer = calloc(sizeof(char), 64);
      buffer = guesses[j];
      write(clients[i], buffer, sizeof(buffer));
    }
  }
}

void correct_guesser(){
  char * buffer = calloc(sizeof(char), 64);
  int index = 0;
  int c = 0;
  for(int i = 0; i < MAX_PLAYERS; i++){
    if(i != hinter){
      if(strcmp(hint.answer, guesses[index]) == 0){
        points[i] += 1;
        corrects[c] = i;
        index++;
        c++;
      }
    }
  }
  points[hinter] += c;
  sprintf(buffer, "%d", c);
  for(int i = 0; i < MAX_PLAYERS; i++){
    write(clients[i], buffer, 2);
    for(int j = 0; j < c; j++){
      char* c_num = calloc(sizeof(char), 2);
      sprintf(c_num, "%d", corrects[j]);
      write(clients[i], c_num, 2);
    }
  }
}

//WINNER
void end_round(){
  int winner = -1;
  int i;
  for(i = 0; i < MAX_PLAYERS; i++){
    if(points[winner] < points[i]){
      winner = i;
    }
  }
  if(points[winner] >= 10){
    for(i = 0; i < MAX_PLAYERS; i++) {
      char * w = calloc(sizeof(char), 2);
      sprintf(w, "%d", winner);
      write(clients[i], w, 2);
      printf("Game finished.\n");
      exit(EXIT_SUCCESS);
    }
  }else{
    winner = -1;
    char * w = calloc(sizeof(char), 2);
    sprintf(w, "%d", winner);
    write(clients[i], w, 2);
  }
}

void start(){
  while(1){
    roles();
    send_word();
    if(receive_hint()==1){
      continue;
      continue;
    }
    get_guesses();
    correct_guesser();
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
    setup(NULL);
  }
  srand(time(NULL));
  start();
}
