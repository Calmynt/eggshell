#pragma once

#define FOREGROUND 1
#define BACKGROUND 0

// Handles signals that are received.
// Handles SIGINT, SIGTSTP and SIGCHLD.
void signal_handler(int signo);

// Checks whether a process is suspended, and resumes it.
// Does this by calling a function that sends a SIGCONT signal to the suspended process.
void resumeProcessSignal(int state);