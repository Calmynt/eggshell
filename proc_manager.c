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
  char **args = (char**) calloc(1, 80);
  int argc = 1;
  char *arg;
  char arg_delimiter[2] = " ";

  int BG = 0;

  char **envp = environEGG();

  pid_t pid = fork();

  int pathn;
  char **paths = pathsToCommArr(&pathn, command);

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

  // printf("PID = %d\n", pid);

  if(pid == 0){ // Child
    // printf("WITHIN CHILD...\n");
    for(int i = 0; i < pathn; i++){
      args[0] = (char*) malloc(80);
      args[0] = paths[i];

      // printf("WITHIN LOOP FOR EXECUTION --- LOOP %d\n", i);
      // printf("PATHN : %d, and PATH : %s\n", pathn, paths[i]);

      // Executes command path[i] with arguments args with environment envp
      execve(paths[i], args, envp);
    }

    exit(0);
  }
  else if(pid > 0){ //Parent
    // printf("WITHIN PARENT...\n");
    current_pid = pid;

    if(BACKGROUND == 0){
      if(signal(SIGCHLD, signal_handler) == SIG_ERR)
        printf("Couldn't catch SIGCHLD --- Child Exit Signal\n");
      pause(); // Pauses until signal is recieved
      //
        if(WIFEXITED(status)){
          setExitcode(WEXITSTATUS(status));
        }
        else if(WIFSIGNALED(status)){
          printf("Process received SIGNAL %d\n", WTERMSIG(status));
        }
    }
  }
  else{
    perror("waitpid()");
  }
}

char** pathsToCommArr(int *pathn, char *program){
  char *pathORIG = value("PATH");

  char *paths = malloc(VARSIZE);

  strcpy(paths, pathORIG); // Done to keep original paths intact

  char **patharr = (char**) calloc(1,100);
  char delimiter[2] = ":";

  int pathnL = 0;

  while(paths!=0){
    char *path = strsep(&paths, delimiter);
    patharr[pathnL] = (char*) malloc(100);
    strcpy(patharr[pathnL], path);
    strcat(patharr[pathnL], "/");
    strcat(patharr[pathnL], program);
    patharr = (char**) realloc(patharr, pathnL+1 * 100);
    pathnL++;
  }

  *pathn = pathnL;

  return patharr;
}

pid_t currentpid(){
  return current_pid;
}

void process_status(int sigstat){
  status = sigstat;
}

int resumeProcess(int state, pid_t process){
  resuspended = 0;
  int wakestatus = kill(process, SIGCONT);
  if (wakestatus != 0){
    fprintf(stderr, "Failed to resume process.\n"); 
    return 1;
  }
  
  current_pid = process;

  if(state == FOREGROUND){
    if(signal(SIGCHLD, signal_handler) == SIG_ERR)
      printf("Couldn't catch SIGCHLD --- Child Exit Signal\n");
    pause();

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