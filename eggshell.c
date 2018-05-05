#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <limits.h>
#include <sys/wait.h>
#include <signal.h>
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

  int assign_check = 0;

  char *vardelimiter = "=";

  char *vartest = malloc(1000);
  strcpy(vartest, line);

  char *varname = strsep(&vartest, vardelimiter);

  // Checks for variable assignment
  if(vartest != 0){
    if(vartest[0] != ' ' && strstr(varname, " ") == 0){
      assign_check = 1;
      for(int i = 0; i < strlen(varname); i++){
        if(varname[i] < 65 || varname[i] > 90){
          assign_check = 0;
          break;
        }
      }

      if(assign_check == 1){
        createVar(line);
        setExitcode(0);
        return;
      }
      else{
        printf("VARNAME non CAPITAL\nVARNAME : %s\nVARTEST : %s\nSTRSTR : %s\n", varname, vartest, strstr(varname, " "));
        return;
      }
    }
          else{
        printf("INVALID = PLACEMENT\nVARNAME : %s\nVARTEST : %s\nSTRSTR : %s\n", varname, vartest, strstr(varname, " "));
        return;
      }
  }

  char *command = strsep(&line, delimiter);

  if(strcmp(command, "print") == 0) printLine(line); // checks for print command
  else if(strcmp(command, "all") == 0) showShellVars(); // checks for all command
  else if(strcmp(command, "vars") == 0) displayUserVars(); // checks for debug vars command
  else if(strcmp(command, "chdir") == 0) changeDirectory(line);
  else if(strcmp(command, "source") == 0) runScript(line);

  if(signal(SIGINT, signal_handler) == SIG_ERR)
    printf("error in catching signal");

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

void signal_handler(int signo){
    pid_t process = currentpid();

    if(signo == SIGINT){
        printf("\n----------------------------\n");
        printf("Interrupting Process [%d] with Signal [%d]...\n", process, SIGINT);
        int success = kill(currentpid(), SIGTERM);
        (success == 0)
          ? printf("Interrupt was successful!\n")
          : printf("Interrupt failed.\n");
        printf("-----------------------------\n");
    }
}