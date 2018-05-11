#include <sys/types.h>
#include <sys/wait.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include "proc_manager.h"

pid_t current_pid;
int status;

void externalCommand(char *command, char *varargs){
  char **args = (char**) calloc(1, 80); // Array of arguments
  char *arg; // Individual argument used to add to array

  int argc = 1; // Counts how many arguments are present
  char arg_delimiter[2] = " "; // To delimit the varargs

  int BG = 0; // Runs process in background or foreground

  char **envp = environEGG(); // Retrieves env variables from eggshell

  pid_t pid = fork();

  // Splits the path variable into an array of paths and stores them in 'paths'
  int pathn;
  char **paths = pathsToCommArr(&pathn, command);

  // Loop for storing varargs into **args
  while(varargs != 0){
    args[argc] = (char*) malloc(80);
    arg = strsep(&varargs, arg_delimiter);

    if(strcmp(arg, "&") == 0){
      BG = 1;
    }

    else{
      strcpy(args[argc], arg);
      argc++;
      args = realloc(args, (argc+1) * 80);
    }
  }

  // Conditional block for fork-exec pattern
  if(pid == 0){ // Child

    // For loop for using all paths
    for(int i = 0; i < pathn; i++){
      args[0] = (char*) malloc(80);
      args[0] = paths[i];

      // Executes command path[i] with arguments args with environment envp
      execve(paths[i], args, envp);
    }

    perror("execve");
    exit(-1);
  }
  else if(pid > 0){ //Parent
    current_pid = pid;

    if(setpgid(pid, pid) != 0) perror("setpid");

    // Waits if background flag not activated.
    if(BG == 0){
      // WUNTRACED used to stop waiting when suspended
      waitpid(current_pid, &status, WUNTRACED);

        if(WIFEXITED(status)){
          setExitcode(WEXITSTATUS(status));
        }
        else if(WIFSIGNALED(status)){
          printf("Process received SIGNAL %d\n", WTERMSIG(status));
        }
    }
  }
  else{
    perror("fork()");
  }
}

char** pathsToCommArr(int *pathn, char *program){
  char *pathORIG = value("PATH"); // stores $PATH string

  char *paths = malloc(VARSIZE);

  strcpy(paths, pathORIG); // Done to keep original paths intact

  char **patharr = (char**) calloc(1,100);
  char delimiter[2] = ":";

  int pathnL = 0; // used for path indexing

  while(paths!=0){
    char *path = strsep(&paths, delimiter);
    patharr[pathnL] = (char*) malloc(100);
    strcpy(patharr[pathnL], path);

    // Appends program to be run to the individual PATH
    strcat(patharr[pathnL], "/");
    strcat(patharr[pathnL], program);

    patharr = (char**) realloc(patharr, pathnL+1 * 100);
    pathnL++;
  }

  *pathn = pathnL; // stores length of path to pathn pointer

  return patharr;
}

pid_t currentpid(){
  return current_pid;
}

int resumeProcess(int state, pid_t process){
  resuspended = 0;

  // Sends SIGCONT signal to suspended process
  int wakestatus = kill(process, SIGCONT);

  // If signal failed - abort function
  if (wakestatus != 0){
    perror("Failed to resume process"); 
    return 1;
  }
  
  current_pid = process;

  if(state == FOREGROUND){
    waitpid(current_pid, &status, WUNTRACED);

    if(WIFEXITED(status)){
      setExitcode(WEXITSTATUS(status));
    }
    else if(WIFSIGNALED(status)){
      printf("Process received SIGNAL %d\n", WTERMSIG(status));
    }
  }

  if(resuspended == 0){
    return 0;
  }
  else{
    return 18;
  }
}