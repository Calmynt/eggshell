#include <signal.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdio.h>

#include "proc_manager.h"

pid_t suspended_process;

void signal_handler(int signo){
    pid_t process = currentpid();

    // Catches interrupt signal
    if(signo == SIGINT){
        printf("\n------------------------------------------------------\n");
        printf("Interrupting Process [%d] with Signal [%d]...\n", process, SIGINT);
        int success = kill(process, SIGINT);
        (success == 0)
          ? printf("Interrupt was successful!\n")
          : printf("Interrupt failed.\n");
        printf("------------------------------------------------------\n");
    }

    // Catches suspend signal
    else if(signo == SIGTSTP){
        printf("\n------------------------------------------------------\n");
        printf("Suspending Process [%d] with Signal [%d]...\n", process, SIGTSTP);
        int success = kill(process, SIGTSTP);
        (success == 0)
          ? printf("Suspension was successful!\n")
          : printf("Suspension failed.\n");
        printf("------------------------------------------------------\n");

        resuspended = 1;

        suspended_process = process;
    }
}

void resumeProcessSignal(int state){
    if(suspended_process < 0){
        fprintf(stderr, "No process currently suspended.\n");
        return;
    }
    else{
        printf("\n------------------------------------------------------\n");
        printf("Resuming process [%d] with Signal [%d]...\n", suspended_process, SIGCONT);
        printf("------------------------------------------------------\n");
        int resumed = resumeProcess(state, suspended_process);
        if(resumed == 0){
            suspended_process = -1;
            setExitcode(0);
        }
        else{
            if(resuspended == 0){
                printf("Process [%d] couldn't be resumed.\n", suspended_process);
                printf("------------------------------------------------------\n");
                setExitcode(-1);
            }
            else{
                setExitcode(18);
            }
        }
    }
}