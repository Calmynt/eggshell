#include "eggshell.h"
#include "linenoise.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char *argv[]){
  printf("Before init...\n");
  initEggshell();

  if(argc == 2){
    if(strcmp(argv[1], "test") == 0){
      runScript("testinput.txt");
      exit(0);
    }
    else{
      fprintf(stderr, "Invalid argument, aborting...\n");
      exit(-1);
    }
  }
  else if(argc != 1){
    fprintf(stderr, "Invalid number of arguments, aborting... \n");
    exit(-1);
  }



  char *prompt = value("PROMPT");

  while((line = linenoise(prompt)) != NULL) {
    parseLine(line);

    linenoiseFree(line);
  }
}
