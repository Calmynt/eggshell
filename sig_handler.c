#include <signal.h>
#include <unistd.h>
#include <sys/wait.h>

#include "proc_manager.h"

pid_t suspended_process;

void signal_handler(int signo){
    pid_t process = currentpid();

    if(signo == SIGINT){
        int success = kill(process, SIGINT);
    }

    else if(signo == SIGTSTP){
        int success = kill(process, SIGTSTP);
        resuspended = 1;
        suspended_process = process;
    }

    else if(signo == SIGCHLD){
        usr_interrupt = 1;
    }
}

void resumeProcessSignal(int state){
    if(suspended_process < 0){
        return;
    }
    else{
        int resumed = resumeProcess(state, suspended_process);
        if(resumed == 0){
            suspended_process = -1;
            setExitcode(0);
        }
        else{
            setExitcode(0);
        }
    }
}