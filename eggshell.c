#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <limits.h>
#include <sys/wait.h>
#include "eggshell.h"
#include "variables.h"
#include "linenoise.h"

#define ARRLEN(x) (sizeof(x)/sizeof(x[0]))

enum bool{false, true};

char *line;

void initEggshell(){
  initShellVars();
}

int parseLine(char* line){
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
          return -1;
        }
      }

      return PARSECODE+=1;
    }

    // Returns error code if spaces surround =
    else if(line[i] == '=' && (line[i-1] == ' ' || line[i+1] == ' ')){
      fprintf(stderr, "\nInvalid assignment - No spaces surround '='\nEx. 'HOME=3'\n\n");
      setExitcode(-1);
      return -1;
    }

  }

  char *pline = strpbrk(line, " ");
  char *command = malloc(sizeof(char) * 256);

  if(pline != NULL){
    *pline = '\0';
    strcpy(command, line);
    strcpy(line, pline+1);
  }
  else{
    strcpy(command, line);
  }

  if(strcmp(command, "print") == 0) return PARSECODE+=2; // checks for print command
  if(strcmp(command, "all") == 0) return PARSECODE+=3; // checks for all command
  if(strcmp(command, "vars") == 0) return PARSECODE+=4; // checks for debug vars command

  return -2;
}

char* printLine(char* line){
  // The typical line is split into words, using ' '
  // The word is then split into segments, using "\""
  // Everytime there is an additional "segment" token, the escape is switched
  // Behaviour then follows accordingly.

  // char wordDelimiter[2] = " ";
  // char segmDelimiter[2] = "\"";
  //
  // char *word;
  // char *segment;
  //
  // int escaped = 0;
  //
  // word = strtok(line, wordDelimiter);
  //
  // while(word != 0){
  //   segment = strtok(word, segmDelimiter);
  //
  //
  // }


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

    int parsed = parseLine(line);
    if(parsed == 1){
      createVar(line);
    }
    else if(parsed == 3){
      showShellVars();
    }
    else if(parsed == 4){
      displayUserVars();
    }
    lineNo++;
  }

  setExitcode(0);
}
