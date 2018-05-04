#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <limits.h>
#include <sys/wait.h>
#include "eggshell.h"
#include "linenoise.h"

#define ARRLEN(x) (sizeof(x)/sizeof(x[0]))

enum bool{false, true};

char *line;

void initEggshell(){
  initShellVars();
}

void parseLine(char* line){
  int PARSECODE = 0;
  char delimiter[2] = " ";
  char rest[2] = "\0";
  if(strcmp(line, "exit") == 0) exit(0);

  // Parse check for assignment
  for(int i = 1; i < strlen(line); i++){
    if(line[i] == '=' && line[i-1] != ' ' && line[i+1] != ' '){
      for(int j = i-1; j >= 0; j--){
        // Returns error code if variable name is invalid
        if(line[j] < 65 || line[j] > 90){
          fprintf(stderr, "\nInvalid assignment - Variable names should be capitalised and alphanumeric!\nEx. 'HOME=3'\n\n");
          setExitcode(-1);
          return;
        }
      }

      createVar(line);
    }

    // Returns error code if spaces surround =
    else if(line[i] == '=' && (line[i-1] == ' ' || line[i+1] == ' ')){
      fprintf(stderr, "\nInvalid assignment - No spaces surround '='\nEx. 'HOME=3'\n\n");
      setExitcode(-1);
      return;
    }

  }

  char *command = strsep(&line, delimiter);

  if(strcmp(command, "print") == 0) printLine(line); // checks for print command
  else if(strcmp(command, "all") == 0) showShellVars(); // checks for all command
  else if(strcmp(command, "vars") == 0) displayUserVars(); // checks for debug vars command
  else if(strcmp(command, "chdir") == 0) changeDirectory(line);
  else if(strcmp(command, "source") == 0) runScript(line);
  else externalCommand(command, line);
}

void runScript(char* filename){
  FILE *testfile = fopen(filename, "r");
  if(testfile == NULL && strcmp(filename, "testinput.txt") == 0){
    fprintf(stderr, "--- No test file found, aborting... ---");
    exit(-1);
  }
  else if(testfile == NULL){
    fprintf(stderr, "No file '%s' found!", filename);
    setExitcode(-1);
    return;
  }

  line = malloc(1024 * sizeof(char));

  int lineNo = 0;

  while(fgets(line, 1024, testfile) != NULL){
    printf("%s%s", value("PROMPT"), line);

    //Terminates line at right place to simulate input
    line[strlen(line)-2] = '\0';

    parseLine(line);

    lineNo++;
  }

  setExitcode(0);
}

void changeDirectory(char* directory){
  printf("DIRECTORY : %s\n", directory);
  char delimiter[2] = "/";

  int exitcode = chdir(directory);

  if(exitcode == 0){
    updateCWD();
    printf("Current directory is now '%s'\n", value("CWD"));
  }
  else{
    perror("Changing directory");
  }
}
