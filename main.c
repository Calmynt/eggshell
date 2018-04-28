#include "eggshell.h"
#include "linenoise.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int main(int argc, char *argv[]){
  initEggshell();

  if(argc == 2){
    if(strcmp(argv[1], "test") == 0){
      testShell();
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
    int parsed = parseLine(line);
    if(parsed == 1){
      printf("--- Assignment Detected ---\n");
      createVar(line);
    }
    else if(parsed == 3){
      showShellVars();
    }
    else if(parsed == 4){
      displayUserVars();
    }

    linenoiseFree(line);
  }
}
