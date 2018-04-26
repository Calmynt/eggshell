#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <limits.h>
#include <sys/wait.h>
#include "linenoise/linenoise.h"

char *line;
char PATH[1024];
char PROMPT[128];
char CWD[1024];
char USER[128];
char HOME[128];
char TERMINAL[128];
char EXEC[1024];

// Works -- Retruns the path to the eggshell executable
void getExecPath(char *PATH){
  char p[128];
  pid_t pid = getpid();
  sprintf(p, "/proc/%d/exe", pid);
  if (readlink(p, PATH, PATH_MAX) == -1)
    perror("readlink");
}

// Works -- Displays all global variables for testing
void checkVars(){
   printf("Current Working Directory \t: %s\n", CWD);
   printf("Path Variable \t\t\t: %s\n", PATH);
   printf("Prompt message \t\t\t: %s\n", PROMPT);
   printf("Username \t\t\t: %s\n", USER);
   printf("Path of Home Folder \t\t: %s\n", HOME);
   printf("Location of current terminal \t:%s\n", TERMINAL);
   printf("Location of executable \t\t: %s \n", EXEC);
}

// Works -- Initialises the global variables of the eggshell
void initVars(){
  getExecPath(EXEC);

  strcpy(PATH, getenv("PATH"));
  strcpy(USER, getenv("USERNAME"));
  strcpy(HOME, getenv("HOME"));

  strcpy(PROMPT, "eggshell-1.0 > ");

  getcwd(CWD, sizeof(CWD));
  strcpy(TERMINAL, ttyname(STDIN_FILENO));
}

// TODO : Need to check
int getExitcode(pid_t p){
  int status;

  if ( waitpid(p, &status, 0) == -1 ) {
    perror("Failure in function - Waitpid");
    return -1;
  }

  if ( WIFEXITED(status) ) {
    int es = WEXITSTATUS(status);
    return es;
  }
}
