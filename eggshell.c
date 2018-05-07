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
      }
      else{
        printf("Assignment detected but line is invalid\n");
        printf("[The variable name can only contain CAPITAL LETTERS]\n");
        setExitcode(-1);
      }
    }
    else{;
      printf("Assignment detected but line is invalid\n");
      printf("[The '=' should not be seperated with spaces]\n");
      setExitcode(-1);
    }
    return;
  }

  // Filename only used for redirection purposes
  char *filename;

  // Checks for different output redirection symbols
  char* redirect_to_file  = strstr(line, ">");
  char* append_to_file    = strstr(line, ">>");
  char* input_from_file   = strstr(line, "<");
  char* input_here_string = strstr(line, "<<<");

  int redirect = 0, append = 0; // variables for output redirection
  int inputfile = 0, inputhere = 0; // variables for input redirection
  int out = 0, in = 0; // flag variables for redirection

  // If condition for seperating filename from the command [output redirect]
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

    out = 1;
  }
  // If condition for seperating filename from the command [input redirect]
  else if(input_here_string != 0 || input_from_file != 0){
    if(input_here_string != 0){
      filename = input_here_string+3;
      inputhere = 1;
    }
    else{
      filename = input_from_file+1;
      inputfile = 1;
    }

    line = strsep(&line, "<");
    line[strlen(line)-1] = 0;

    while(filename[0] == ' '){
      filename++;
    }

    in = 1;
  }


  // Seperates the command from the arguments
  char *command = strsep(&line, delimiter);

  int filefd;

  // Opens an output stream depending on the flags triggered
  // If no output flag has been triggered, no stream is created.
  if(append == 1){
    filefd = open(filename, O_WRONLY|O_CREAT|O_APPEND, 0666);
  }
  else if(redirect == 1){
    filefd = open(filename, O_WRONLY|O_CREAT|O_TRUNC, 0666);
  }
  else if(inputfile == 1){
    filefd = open(filename, O_RDONLY);
  }
  else if(inputhere == 1){
    FILE *f = fopen("stdin.tmp", "w+");
    fprintf(f, "%s", filename);

    filefd = open("stdin.tmp", O_RDONLY);
    fclose(f);
  }
  else{
    filename = 0;
  }

  // Conditional block for redirecting output
  if(out == 1){
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
  // Conditional block for redirecting input
  else if(in == 1){
    int save_in = dup(fileno(stdin));

    if(dup2(filefd, fileno(stdin)) == -1){
      perror("Failed in redirecting stdin");
      setExitcode(255);
      return;
    }

    runLine(command, line);

    fflush(stdin);
    close(filefd);

    dup2(save_in, fileno(stdin));
    close(save_in);

    if(inputhere == 1){
      remove("stdin.tmp");
    }

  }
  else{
    runLine(command, line);
  }
}

void runScript(char* filename){
  FILE *testfile = fopen(filename, "r");

  // Special condition for test execution
  if(testfile == NULL && strcmp(filename, "testinput.txt") == 0){
    fprintf(stderr, "--- No test file found, aborting... ---");
    exit(-1);
  }
  // Condition for nonexistent file
  else if(testfile == NULL){
    fprintf(stderr, "No file '%s' found!", filename);
    setExitcode(-1);
    return;
  }

  line = malloc(1024 * sizeof(char));

  int lineNo = 0;

  while(fgets(line, 1024, testfile) != NULL){
    printf("%s%s", value("PROMPT"), line); // Emulates the prompt

    //Terminates line at right place to simulate input
    line[strlen(line)-2] = '\0';

    execute(line);

    lineNo++; 
  }

  setExitcode(0);
}

void changeDirectory(char* directory){
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
  else if(strcmp(command, "chdir") == 0) {changeDirectory(line); return;} // checks for chdir command
  else if(strcmp(command, "source") == 0) {runScript(line); return;} // checks for source command
  else if(strcmp(command, "fg") == 0) {resumeProcessSignal(FOREGROUND); return;} // recovers suspended process to fg
  else if(strcmp(command, "bg") == 0) {resumeProcessSignal(BACKGROUND); return;} // recovers suspended process to bg

  // If conditions for signal handling.
  // Also creates 2 signal handlers in memory for the SIGINT and SIGTSTP
  if(signal(SIGINT, signal_handler) == SIG_ERR)
    printf("Couldn't catch SIGINT - Interrupt Signal\n");
  if(signal(SIGTSTP, signal_handler) == SIG_ERR)
    printf("Couldn't catch SIGTSTP - Suspension Signal\n");

  // Runs external command seperate from the eggshell
  else externalCommand(command, line);
}