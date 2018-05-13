/* to fill */

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>

#include "variables.h"
#include "proc_manager.h"
#include "pipe_manager.h"
#include "printer.h"
#include "../eggshell.h"

int pipeAmnt; // stores the amount of pipes present in the line
int cmdAmnt; // stores the amount of commands

int pipe_parser(char *line){
    if(strstr(line, "|") == 0){return 1;}

    pipeAmnt = 0;

    char *linecpy = strdup(line);
    void *toFree = linecpy;

    for(int i = 0; i < strlen(line); i++){
        if(linecpy[i] == '|') pipeAmnt++;
    }

    char *commands[pipeAmnt];

    cmdAmnt = 0;

    while(linecpy != 0){
        char *command1 = strsep(&linecpy, "|");

        while(*command1 == ' '){
            command1++;
        }

        while(command1[strlen(command1)-1] == ' '){
            command1[strlen(command1)-1] = 0;
        }

        commands[cmdAmnt] = command1;
        cmdAmnt++;
    }

    pipe_executer(commands);

    free(toFree);

    return 0;
}



int pipe_executer(char **commands){
    int pipefd[pipeAmnt*2];

    // Creates pipes out of the array of integers.
    for(int i = 0; i < pipeAmnt; i++){
        if(pipe(pipefd + i*2) < 0){
            perror("piping");
            _exit(-1);   
        }
    }

    int j = 0; // iterative variable for different commands
    int p = 0; // iterative variable for different pipes
    pid_t pid; // pid of processes

    for(j = 0; j < cmdAmnt; j++){

        int pathlen = 0;
        char *command = strsep(&commands[j], " ");

        char *printline = malloc(1024);
        void *tofree = printline;

        if(commands[j] != 0){
            strcpy(printline, commands[j]);
        }

        char **env = environEGG();
        char **paths = pathsToCommArr(&pathlen, command);
        char **args = calloc(1, 80);
        char *arg;
        int argc = 1;

        /* Break up all the arguments and place in **args */
        while(commands[j] != 0){
            args[argc] = (char*) malloc(80);
            arg = strsep(&commands[j], " ");
            strcpy(args[argc], arg);
            argc++;
            args = realloc(args, (argc+1) * 80);
        }

        // Initialise first argument [to be set to the program]
        args[0] = (char*) malloc(80);

        pid = fork();

        if(pid == 0){
            /* If it isn't the last command,
               then replace the current output pipe with stdout */
            if(j != cmdAmnt-1){
                if(dup2(pipefd[p+1], 1) < 0){
                    perror("dup2 piping output");
                    _exit(-1);
                }
            }

            /* If it isn't the first command, 
               then replace the last input pipe with stdin */
            if(j != 0){
                if(dup2(pipefd[p-2], 0) < 0){
                    perror("dup2 piping input");
                    _exit(-1);
                }
            }

            /* Close all current pipes */
            for(int i = 0; i < 2*pipeAmnt; i++){
                close(pipefd[i]);
            }
            
            /* Execute the command with its arguments
               and the eggshell's environment variables 
               Contains support for eggshell commands w/ output. */
            if(strcmp(command, "print") == 0) {printLine(printline); exit(-1);} // checks for print command
            else if(strcmp(command, "all") == 0) {showShellVars(); exit(-1);} // checks for all command
            else if(strcmp(command, "vars") == 0) {displayUserVars(); exit(-1);} // checks for debug vars command
            else if(strcmp(command, "source") == 0) {runScript(printline); exit(-1);}  // checks for source command
            else{
                for(int i = 0; i < pathlen; i++){
                    strcpy(args[0], paths[i]); // sets first arg to program
                    execve(*args, args, env);
                }

                perror("execve");
                _exit(-1);
            }
        }

            for(int i = 0; i < argc; i++){
                free(args[i]);
            }
            free(args);
            
            for(int i = 0; i < pathlen; i++){
                free(paths[i]);
            }
            free(paths);

            free(tofree);

            p+=2; // GO to next pipe
        }

    /* Close all open pipes */
    for(int i = 0; i < 2*pipeAmnt; i++){
        close(pipefd[i]);
    }

    /* Wait for all the children to finish */
    int status;
    for(int i = 0; i < pipeAmnt+1; i++){
        wait(&status);
    }

    return 0;
}