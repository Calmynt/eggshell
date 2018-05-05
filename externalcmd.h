#include <unistd.h>

// Executes an external command, not present in eggshell.
// command contains the command to be executed.
// line contains the arguments for the command.
void externalCommand(char* command, char* line);

// Breaks up the $PATH variable into an array of paths.
// pathn is meant to contain the length of the array.
// program is the program meant to be executed.
// MEANT to be used in conjunction with execve
char** pathsToCommArr(int *pathn, char* program);

pid_t currentpid();
