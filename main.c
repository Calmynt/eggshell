#include "eggshell.h"
#include "./add-on/linenoise.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <unistd.h>

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

  linenoiseHistorySetMaxLen(10);
  
  runLine("clear", "");

  printf("\t\t-----=====<<<<!!! Welcome to Eggshell !!!>>>>=====-----\n\n");

  while((line = linenoise(prompt)) != NULL) {
    char *history_line = malloc(300);
    strcpy(history_line, line);

    execute(line);

    linenoiseHistoryAdd(history_line);

    updatePrompt();

    linenoiseFree(line);
  }

  runLine("clear", "");
  exit(EXIT_SUCCESS);
}
