#include <signal.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdio.h>

#include "externalcmd.h"

void signal_handler(int signo){
    pid_t process = currentpid();

    if(signo == SIGINT){
        printf("\n----------------------------\n");
        printf("Interrupting Process [%d] with Signal [%d]...\n", process, SIGINT);
        int success = kill(currentpid(), SIGINT);
        (success == 0)
          ? printf("Interrupt was successful!\n")
          : printf("Interrupt failed.\n");
        printf("-----------------------------\n");
    }
    else if(signo == SIGTSTP){
        printf("\n----------------------------\n");
        printf("Suspending Process [%d] with Signal [%d]...\n", process, SIGTSTP);
        int success = kill(currentpid(), SIGTSTP);
        (success == 0)
          ? printf("Suspension was successful!\n")
          : printf("Suspension failed.\n");
        printf("-----------------------------\n");
    }
    else if(signo == SIGCHLD){
        int status;
        waitpid(currentpid(), &status, WNOHANG);
        process_status(status);
    }
}