#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <limits.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <signal.h>

#include "eggshell.h"
#include "./add-on/linenoise.h"
#include "./src/redirection.h"
#include "./src/pipe_manager.h"

#define ARRLEN(x) (sizeof(x)/sizeof(x[0]))

char *line;

void initEggshell(int argc, char *argv[]){
  initShellVars(argv[0]);
}

void execute(char* line){
  int PARSECODE = 0;
  char delimiter[2] = " ";

  char rest[2] = "\0";
  if(strcmp(line, "exit") == 0){runLine("clear", ""); exit(0);}

  // Check for variable assignment
  if(parse_var(line) == 0){return;}
  if(pipe_parser(line) == 0){return;}

  char *filename; // only used for redirection purposes


  // Checks for different redirection symbols
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

  // Condition for nonexistent file
  if(testfile == NULL){
    fprintf(stderr, "No file '%s' found!", filename);
    setExitcode(-1);
    return;
  }

  int lineNo = 0;

  char **script = calloc(1, 1024);

  int lines = 0;

  char *buf = malloc(1024);

  while(fgets(buf, 255, testfile) != 0){
    script[lines] = strdup(buf);
    script = realloc(script, (++lines) * 1024);
  }

  char *stop_inf_recursion = malloc(1024);
  sprintf(stop_inf_recursion, "source %s", filename);

  free(buf);

  fclose(testfile);

  while(lineNo < lines){
    line = strdup(script[lineNo]);
    lineNo++; 
    void *toFree = line;

    if(strlen(line) < 3){continue;}
    if(line[0] == '#'){continue;}
    if(line[0] == ' '){continue;}

    printf("%s%s", value("PROMPT"), line); // Emulates the prompt

    if(strstr(line, stop_inf_recursion) != 0){
      fprintf(stderr, "Detected recursive loop - skipping line...\n");
      continue;      
    }

    // Block of code to remove newlines and carriage returns from file.
    // This is because on Windows, a newline is \r\n, and on Unix, it is \n.
    // The loop uses pointer arithmetic to replace \r / \n with the next character.
    char *src, *dst;
    for (src = dst = line; *src != '\0'; src++){
      *dst = *src;
      if(*dst != '\r' && *dst != '\n') dst++;
    }
    *dst = '\0';

    execute(line);
    free(toFree);

    Var *prompt = retrieveVar("PROMPT");
    sprintf(prompt->value, "< Executing script... - [%s] > $ ", value("EXITCODE"));
  }

  for(int i = 0; i < lines; i++){
    free(script[i]);
  }
  free(script);
  free(stop_inf_recursion);

  setExitcode(0);
}

void changeDirectory(char* directory){
  int exitcode = chdir(directory);

  if(exitcode == 0){
    updateCWD();
    printf("Current directory is now '%s'\n", value("CWD"));
    setExitcode(exitcode);
  }
  else{
    perror("Changing directory");
    setExitcode(exitcode);
  }
}

void runLine(char *command, char *line){
  init_handler();

  if(strcmp(command, "print") == 0) printLine(line); // checks for print command
  else if(strcmp(command, "all") == 0) showShellVars(); // checks for all command
  else if(strcmp(command, "vars") == 0) displayUserVars();  // checks for debug vars command
  else if(strcmp(command, "chdir") == 0) changeDirectory(line); // checks for chdir command
  else if(strcmp(command, "source") == 0) runScript(line); // checks for source command
  else if(strcmp(command, "fg") == 0) resumeProcessSignal(FOREGROUND); // recovers suspended process to fg
  else if(strcmp(command, "bg") == 0) resumeProcessSignal(BACKGROUND);// recovers suspended process to bg

  // Runs external command seperate from the eggshell
  else externalCommand(command, line);
}