char *line;

#include "variables.h"
#include "printer.h"
#include "externalcmd.h"

/* Initialises the eggshell */
void initEggshell();

/* Tests the shell using a testfile called "testinput.txt" */
void runScript(char* filename);

// Changes the directory of the eggshell
void changeDirectory(char* directory);

// The main execution core of the eggshell.
// This is what the user will use to execute any and all commands.
// This method parses a line entered by the user, and runs the appropriate command.
void parseLine(char* line);
