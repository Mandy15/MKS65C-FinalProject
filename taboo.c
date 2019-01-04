#include "taboo.h"
#define clear() printf("\033[H\033[J")

void setup(){
  clear();
  printf("                                   TABOO!!!\n\n");
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

int main(){
  char *input = malloc(64*sizeof(char));

  setup();
  printf("Join game? (y/n) ");
  input = read_line();
  if(strcmp(input,"y") == 0){
    // while(1){
      printf("lol\n");
    // }
  }else{
    printf("OK, goodbye.\n");
  }

  return 0;
}
//Group #78
//game: word association; like taboo; hint giver that tries to get the guessers to correctly guess their word
//player needs to be able to join game w ppl
  //mechanics: time limit per turn; players can take turns being the hint giver (describes words w/o using certain taboo words);
  //           ppl who guess right get a pt n the hint giver gets pts per person that guessed; winner is one who has most points
