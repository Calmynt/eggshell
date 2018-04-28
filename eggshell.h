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
void testShell();

/* Displays all user variables initialised within the shell */
void displayUserVars();

/* Displays all shell variables */
void showShellVars();

/* Retrieves the value of a variable by using the parameter */
char* value(char*);

/* Returns a 'Parse Code' that deciphers what the line is trying to achieve.
   1    = 'Assignment'
   2    = 'Print'
   -1   = 'Error in parsing'
   100  = 'External command' */
int parseLine();
