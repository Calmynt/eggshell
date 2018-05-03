char *line;

#include "variables.h"

/* Initialises the eggshell */
void initEggshell();

/* Tests the shell using a testfile called "testinput.txt" */
void runScript(char*);

/* Prints a line, incl. variable support */
char* printLine(char*);

/* Returns a 'Parse Code' that deciphers what the line is trying to achieve.
   1    = 'variable'
   2    = 'print'
   3    = 'all'
   4    = 'vars'
   -1   = 'Error in parsing'
   -2  = 'External command'

   There are also additional factors that determine whether a line contains
   any redirection or piping.
   101 - 104 = Output redirection
   201 - 204 = Input redirection
   300       = Piping

   Piping has a single number, as it might contain several commands. */
int parseLine();
