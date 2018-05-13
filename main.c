#include "eggshell.h"
#include "./add-on/linenoise.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <unistd.h>

int main(int argc, char *argv[]){
  setbuf(stdout, 0);

  printf("Before init...\n");
  initEggshell(argc, argv);

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

  // Introduction screen, for the first time eggshell is booted up //
  printf("\t\t-----=====<<<<!!!!!!!!!!!!!!!!!!!!!!!!!!!>>>>=====-----\n");
  printf("\t\t-----=====<<<<!!!                     !!!>>>>=====-----\n");
  printf("\t\t-----=====<<<<!!!       Welcome       !!!>>>>=====-----\n");
  printf("\t\t-----=====<<<<!!!         t o         !!!>>>>=====-----\n");
  printf("\t\t-----=====<<<<!!!   E G G S H E L L   !!!>>>>=====-----\n");
  printf("\t\t-----=====<<<<!!!                     !!!>>>>=====-----\n");
  printf("\t\t-----=====<<<<!!!!!!!!!!!!!!!!!!!!!!!!!!!>>>>=====-----\n");

  printf("\n\n");

  while((line = linenoise(prompt)) != NULL) {
    char *history_line = malloc(300);
    strcpy(history_line, line);

    execute(line);

    linenoiseHistoryAdd(history_line);

    updatePrompt();

    linenoiseFree(line);
    free(history_line);
  }

  runLine("clear", "");
  exit(EXIT_SUCCESS);
}
