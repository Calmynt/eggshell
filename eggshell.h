char *line;

typedef struct variable Var;
typedef struct variables Vars;

/* Sets parameter to the path of the program */
void getExecPath(char*);

/* Initialises the eggshell */
void initEggshell();

/* Creates a variable using an assignment line */
void createVar(char*);

/* Tests the shell using a testfile called "testinput.txt" */
void runScript(char*);

/* Displays all user variables initialised within the shell */
void displayUserVars();

/* Displays all shell variables */
void showShellVars();

/* Retrieves the value of a variable by using the parameter */
char* value(char*);

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
