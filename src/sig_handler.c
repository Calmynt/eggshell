#include <signal.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdio.h>

#include "proc_manager.h"

pid_t suspended_process[10];
int last_suspended = -1;

void signal_handler(int signo){
    pid_t process = currentpid();

    // Catches interrupt signal
    if(signo == SIGINT){
        int success = kill(process, SIGINT);
    }

    // Catches suspend signal
    else if(signo == SIGTSTP){
        int success = kill(process, SIGTSTP);
        resuspended = 1;
        suspended_process[last_suspended+1] = process;
        last_suspended++;
    }
}

void resumeProcessSignal(int state){
    if(last_suspended < 0){
        fprintf(stderr, "No process currently suspended.\n");
        return;
    }
    else{
        int resumed = resumeProcess(state, suspended_process[last_suspended]);
        if(resumed == 0){
            suspended_process[last_suspended] = 0;
            last_suspended--;
            setExitcode(0);
        }
        else{
            if(resuspended == 0){
                setExitcode(-1);
            }
            else{
                setExitcode(18);
            }
        }
    }
}