#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <limits.h>
#include <sys/wait.h>
#include <signal.h>

#include "eggshell.h"
#include "./add-on/linenoise.h"
#include "./src/redirection.h"
#include "./src/pipe_manager.h"

#define ARRLEN(x) (sizeof(x)/sizeof(x[0]))

char *line;

void initEggshell(){
  initShellVars();
}

void execute(char* line){
  int PARSECODE = 0;
  char delimiter[2] = " ";
  char rest[2] = "\0";
  if(strcmp(line, "exit") == 0){runLine("clear", ""); exit(0);}

  // Check for variable assignment
  if(parse_var(line) == 0){return;}

  if(pipe_parser(line) == 0){return;}
  // TODO: finish piping

  char *filename; // only used for redirection purposes

  // Checks for different output redirection symbols
  char* redirect_to_file  = strstr(line, ">");
  char* append_to_file    = strstr(line, ">>");
  char* input_from_file   = strstr(line, "<");
  char* input_here_string = strstr(line, "<<<");

  int out = 0, in = 0; // flag variables for redirection

  // Checks for redirection
  if(append_to_file != 0 || redirect_to_file != 0){
    filename = out_redirect_parse(append_to_file, redirect_to_file, line);
    out = 1;
  }
  else if(input_here_string != 0 || input_from_file != 0){
    filename = in_redirect_parse(input_from_file, input_here_string, line);
    in = 1;
  }

  // Seperates the command from the arguments
  char *command = strsep(&line, delimiter);

  int filefd = init_redirect(filename);

  if(out == 1){
    redirect_out(filefd);
    runLine(command, line);
    close_redirects(OUT, filefd);
  }
  else if(in == 1){
    redirect_in(filefd);
    runLine(command, line);
    close_redirects(IN, filefd);
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
    if(strlen(line) < 3){continue;}
    if(line[0] == '#'){continue;}

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