#include "networking.h"
#define clear() printf("\033[H\033[J")
#define MAX_PLAYERS 5
#define MAX_POINTS 10

struct tab{
  char answer[20];
  char * tabo[3];
};
char * keys[6] = {"cat", "window", "Mr. K", "chair", "door" , "Stuyvesant"
                  };
char * values[6][3] = { {"feline","kitty","tabby"}, {"glass","wall","look"},
                        {"teacher","systems","konstantinovich"},
                        {"seat","butt","sit"}, {"entrance","doorway","exit"}, {"school","here","hell"},
                    };

struct tab hint;
char ** usernames;
int * clients;
int * points;
char ** guesses;
int hinter;
int next_hinter;
int rounds;
int * corrects;

static void sighandler(int signo) {
  if (signo == SIGINT) {
    printf("\n");
    exit(EXIT_SUCCESS);
  }
}

void setup(char * ip){
  int i,j;
  srand(time(NULL));
  signal(SIGINT, sighandler);
  rounds = 0;

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
  hinter = rounds % MAX_PLAYERS;
  char* h = calloc(sizeof(char), 2);
  sprintf(h, "%d", hinter);
  for (int i = 0; i < MAX_PLAYERS; i++) {
    write(clients[i], h, 2);
  }
  free(h);
}

void random_word(){
  int i = rand() % 6;
  strcpy(hint.answer, keys[i]);
  for(int x = 0; x < 3; x++){
    hint.tabo[x] = values[i][x];
  }
}

void send_word(){
  char * buffer = calloc(sizeof(char), 64);
  buffer = hint.answer;
  write(clients[hinter], buffer, sizeof(buffer));
  for(int i = 0; i < 3; i++){
    char * buf = calloc(sizeof(char), 64);
    buf = hint.tabo[i];
    write(clients[hinter], buf, sizeof(buf));
  }
}

int receive_hint(){
  char * buffer = calloc(sizeof(char), 64);
  read(clients[hinter], buffer, sizeof(buffer));
  char * buf = calloc(sizeof(char), 10);
  buf = strsep( &buffer, " " );
  for(int i = 0; i < 10; i++){
    for(int j = 0; j < 3; j++){
      if(strcmp(hint.tabo[j], &buf[i]) == 0){
        char * buf1 = calloc(sizeof(char), 2);
        buf1 = "X";
        write(clients[hinter], buf1, sizeof(buf1));
        return 1;
      }
    }
  }
  for (int i = 0; i < MAX_PLAYERS-1; i++) {
    if(i != hinter){
      write(clients[i], buffer, sizeof(buffer));
    }
  }
  return 0;
}

//GUESSER
void get_guesses(){
  int index = 0;
  for(int i = 0; i < MAX_PLAYERS; i++){
    if(i != hinter){
      char buffer[BUFFER_SIZE];
      read(clients[i], buffer, sizeof(buffer));
      guesses[index] = buffer;
      index++;
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
  char buffer[BUFFER_SIZE];
  int index = 0;
  int turn = 0;
  int c = 0;
  for(int i = 0; i < MAX_PLAYERS; i++){
    if(i != hinter){
      if(strcmp(hint.answer, guesses[index]) == 0){
        points[i] += 1;
        corrects[c] = i;
        index++;
        c++;
        turn = 1;
      }
    }
  }
  points[hinter] += c;
  rounds += turn;
  for(int i = 0; i < MAX_PLAYERS; i++){
    char* c_num = calloc(sizeof(char), 2);
    sprintf(c_num, "%d", c);
    write(clients[i], c_num, 2);
  }
  for(int i = 0; i < MAX_PLAYERS; i++){
    for(int j = 0; j < MAX_PLAYERS-1; j++){
      char* c_num = calloc(sizeof(char), 2);
      sprintf(c_num, "%d", corrects[j]);
      write(clients[i], c_num, 2);
    }
  }
}

//WINNER
void end(){
  int winner = 0;
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
    }
  }else{
    char * w = calloc(sizeof(char), 2);
    sprintf(w, "%d", -1);
    write(clients[i], w, 2);
    return;
  }

  printf("Game finished.\n");
  exit(EXIT_SUCCESS);
}

void start(){
  while(1){
    roles();
    random_word();
    send_word();
    while(receive_hint());
    get_guesses();
    correct_guesser();
    end();
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
  start();

  return 0;
}
