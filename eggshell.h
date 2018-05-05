char *line;

#include "variables.h"
#include "printer.h"
#include "proc_manager.h"
#include "sig_handler.h"

/* Initialises the eggshell */
void initEggshell();

/* Tests the shell using a testfile called "testinput.txt" */
void runScript(char* filename);

// Changes the directory of the eggshell
void changeDirectory(char* directory);

void runLine(char *command, char *line);

// The main execution core of the eggshell.
// This is what the user will use to execute any and all commands.
// This method parses a line entered by the user, and runs the appropriate command.
void execute(char* line);