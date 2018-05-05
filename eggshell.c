#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <limits.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <signal.h>
#include "eggshell.h"
#include "linenoise.h"

#define ARRLEN(x) (sizeof(x)/sizeof(x[0]))

enum bool{false, true};

char *line;

void initEggshell(){
  initShellVars();
}

void execute(char* line){
  int PARSECODE = 0;
  char delimiter[2] = " ";
  char rest[2] = "\0";
  if(strcmp(line, "exit") == 0){runLine("clear", ""); exit(0);}

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
        // printf("VARNAME non CAPITAL\nVARNAME : %s\nVARTEST : %s\nSTRSTR : %s\n", varname, vartest, strstr(varname, " "));
        return;
      }
    }
          else{
        // printf("INVALID = PLACEMENT\nVARNAME : %s\nVARTEST : %s\nSTRSTR : %s\n", varname, vartest, strstr(varname, " "));
        return;
      }
  }

  char *filename;
  char* redirect_to_file = strstr(line, ">");
  char* append_to_file   = strstr(line, ">>");

  int redirect = 0, append = 0;

  if(append_to_file != 0 || redirect_to_file != 0){
    if(append_to_file != 0){
      filename = append_to_file+2;
      append = 1;
    }
    else{
      filename = redirect_to_file+1;
      redirect = 1;
    }
    line = strsep(&line, ">");
    line[strlen(line)-1] = 0;

    while(filename[0] == ' '){
      filename++;
    }
  }

  char *command = strsep(&line, delimiter);

  int filefd;

  if(append == 1){
    filefd = open(filename, O_WRONLY|O_CREAT|O_APPEND, 0666);
  }
  else if(redirect == 1){
    filefd = open(filename, O_WRONLY|O_CREAT|O_TRUNC, 0666);
  }
  else{
    filename = 0;
  }

  if(filename != 0){
    int save_out = dup(fileno(stdout));

    if(dup2(filefd, fileno(stdout)) == -1){
      perror("Failed in redirecting stdout");
      setExitcode(255);
      return;
    }

    runLine(command, line);

    fflush(stdout);
    close(filefd);

    dup2(save_out, fileno(stdout));
    close(save_out);
  }
  else{
    runLine(command, line);
  }
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

    execute(line);

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
    setExitcode(exitcode);
  }
}

void runLine(char *command, char *line){
  if(strcmp(command, "print") == 0) {printLine(line); return;} // checks for print command
  else if(strcmp(command, "all") == 0) {showShellVars(); return;} // checks for all command
  else if(strcmp(command, "vars") == 0) {displayUserVars(); return;}  // checks for debug vars command
  else if(strcmp(command, "chdir") == 0) {changeDirectory(line); return;} 
  else if(strcmp(command, "source") == 0) {runScript(line); return;}
  else if(strcmp(command, "fg") == 0) {resumeProcessSignal(FOREGROUND); return;}
  else if(strcmp(command, "bg") == 0) {resumeProcessSignal(BACKGROUND); return;}

  if(signal(SIGINT, signal_handler) == SIG_ERR)
    printf("Couldn't catch SIGINT - Interrupt Signal\n");
  if(signal(SIGTSTP, signal_handler) == SIG_ERR)
    printf("Couldn't catch SIGTSTP - Suspension Signal\n");

  else externalCommand(command, line);
}