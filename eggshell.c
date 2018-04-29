#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <limits.h>
#include <sys/wait.h>
#include "eggshell.h"
#include "linenoise.h"

#define ARRLEN(x) (sizeof(x)/sizeof(x[0]))

typedef struct variable{
  char* varname;
  char* value;
}Var;

typedef struct variables{
  Var** vars;
  int amount;
}Vars;

enum bool{false, true};

char *line;

Vars *variables;

/* Static method declarations */
static Var* retrieveVar(char*);
static char* setExitcode(); //TODO: Implement setting of exit code

void getExecPath(char *PATH){
  char p[128];
  pid_t pid = getpid();
  sprintf(p, "/proc/%d/exe", pid); // reads path from file
  if (readlink(p, PATH, PATH_MAX) == -1)
    perror("readlink"); // returns error if reading aborts
}

void showShellVars(){
   printf("CWD=%s\n", value("CWD"));
   printf("PATH=%s\n", value("PATH"));
   printf("PROMPT=%s\n", value("PROMPT"));
   printf("USER=%s\n", value("USER"));
   printf("HOME=%s\n", value("HOME"));
   printf("TERMINAL=%s\n", value("TERMINAL"));
   printf("EXEC=%s \n", value("EXEC"));
   printf("EXITCODE=%s \n", value("EXITCODE"));
}

void initEggshell(){
  // Sets execution path and current working directory
  char *exec = malloc(1024 * sizeof(char));
  getExecPath(exec);
  char cwd[1024];
  getcwd(cwd, sizeof(cwd));

  // Initialises variables array
  variables = (Vars *) malloc(sizeof(Vars));
  variables->vars = (Var **) calloc(1, sizeof(Var));
  variables->vars[0] = (Var *) malloc(sizeof(Var));
  variables->amount = 0;

  // Initialises shell variable-injection strings
  char *a, *b, *c, *d, *e, *f, *g, *h;
  a = malloc(1024 * sizeof(char));
  b = malloc(1024 * sizeof(char));
  c = malloc(1024 * sizeof(char));
  d = malloc(1024 * sizeof(char));
  e = malloc(1024 * sizeof(char));
  f = malloc(1024 * sizeof(char));
  g = malloc(1024 * sizeof(char));
  h = malloc(1024 * sizeof(char));

  // Creates strings to set shell variables
  sprintf(a,"PATH=%s", getenv("PATH"));
  sprintf(b,"USER=%s", getenv("USER"));
  sprintf(c,"HOME=%s", getenv("HOME"));
  sprintf(d,"PROMPT=%s", "eggshell-1.0 > ");
  sprintf(e,"CWD=%s", cwd);
  sprintf(f,"TERMINAL=%s", ttyname(STDIN_FILENO));
  sprintf(g,"EXEC=%s", exec);
  sprintf(h,"EXITCODE=%s", "(none)");

  // Sets shell variables using above strings
  createVar(a);
  createVar(b);
  createVar(c);
  createVar(d);
  createVar(e);
  createVar(f);
  createVar(g);
  createVar(h);

  // Frees now-useless injection strings
  free(a); free(b); free(c); free(d); free(e); free(f); free(g); free(h);
}

static char* setExitcode(pid_t p){
  int status;
  char *exitcode;

  if ( waitpid(p, &status, 0) == -1 ) {
    perror("Failure in function 'getExitcode'");
    sprintf(exitcode, "%d", -1);
    return exitcode;
  }

  if ( WIFEXITED(status) ) {
    int es = WEXITSTATUS(status);
    sprintf(exitcode, "%d", es);
    return exitcode;
  }
}

int parseLine(char* line){
  int PARSECODE = 0;
  char delimiter[2] = " ";
  if(strcmp(line, "exit") == 0) exit(0);

  // Parse check for assignment
  for(int i = 1; i < strlen(line); i++){
    if(line[i] == '=' && line[i-1] != ' ' && line[i+1] != ' '){
      for(int j = i-1; j >= 0; j--){
        // Returns error code if variable name is invalid
        if(line[j] < 65 || line[j] > 90){
          fprintf(stderr, "\nInvalid assignment - Variable names should be capitalised and alphanumeric!\nEx. 'HOME=3'\n\n");
          return -1;
        }
      }

      return PARSECODE+=1;
    }

    // Returns error code if spaces surround =
    else if(line[i] == '=' && (line[i-1] == ' ' || line[i+1] == ' ')){
      fprintf(stderr, "\nInvalid assignment - No spaces surround '='\nEx. 'HOME=3'\n\n");
      return -1;
    }

  }

  char *command = strtok(line, delimiter);

  if(strcmp(command, "print") == 0) return PARSECODE+=2; // checks for print command
  if(strcmp(command, "all") == 0) return PARSECODE+=3; // checks for all command
  if(strcmp(command, "vars") == 0) return PARSECODE+=4; // checks for debug vars command

  return -2;
}

void createVar(char* line){
  char* name = malloc(50 * sizeof(char));
  char* str = malloc(200 * sizeof(char));
  char* token;

  int i = 0;
  int before;
  char delimiter[2] = "=";

  // Retrieves name of variable and value using tokenization
  token = strtok(line, delimiter);
  strcpy(name, token);
  token = strtok(NULL, delimiter);
  strcpy(str, token);

  // Checks whether variable with name already exists
  Var *variable = retrieveVar(name);

  // Checks for existing variable.
  // If variable exists, it is overwritten.
  if(variable != 0){
    // Checks whether assignment uses variable.
    // Does this by checking whether the first character is '$'
    if(str[0] != '$'){
      variable->varname = name;
      variable->value = str;
    }
    // Retrieves variable from list.
    // If no variable exists, assignment is aborted and error is printed.
    // Else, the value of the mentioned variable overwrites the variable.
    else{
      memmove(str, str+1, strlen(str));
      Var *var = retrieveVar(str);
      if(var == 0){fprintf(stderr, "Variable does not exist!\n"); return;}
      variable->varname = name;
      strcpy(variable->value, var->value);
    }
    return;
  }

  // Checks whether the right side of the assignment mentions a variable.
  // If so, the variable is retrieved.
  // If no such variable exists, an error is printed and the assignment aborts.
  // Else, the new variable is assigned.
  if(str[0] == '$'){
    memmove(str, str+1, strlen(str));
    Var *var = retrieveVar(str);
    if(var == 0){fprintf(stderr, "Variable does not exist!\n"); return;}
    strcpy(str, var->value);
  }

  int a = variables->amount;

  // Adds new variable to list
  variables->vars[a] = (Var*)malloc(sizeof(Var));
  variables->vars[a]->value = str;
  variables->vars[a]->varname = name;

  // Increments amount of variables
  variables->amount++;
  a = variables->amount;

  // Reallocates memory to list of variables, preparing for new variable.
  Var** vars = (Var**) realloc(variables->vars, a*sizeof(Var));
  variables->vars = vars;
  variables->vars[a] = (Var *) malloc(sizeof(Var));
}

static Var* retrieveVar(char* varname){
  for(int i = 0; i < variables->amount; i++){
    if(strcmp(variables->vars[i]->varname, varname) == 0){
      return variables->vars[i];
    }
  }
  return 0;
}

char* value(char* varname){
  Var *variable = retrieveVar(varname);
  return variable->value;
}

void displayUserVars(){
  printf("--- VARIABLES ---\n");
  for(int x = 8; x < variables->amount; x++){
    if(variables->vars[x] == 0){ printf("ERR\n"); return;}
    printf("%s=%s\n", variables->vars[x]->varname, variables->vars[x]->value);
  }
}

void runScript(char* filename){
  FILE *testfile = fopen(filename, "r");
  if(testfile == NULL){
    fprintf(stderr, "--- No test file found, aborting... ---");
    exit(-1);
  }

  line = malloc(1024 * sizeof(char));

  int lineNo = 0;

  while(fgets(line, 1024, testfile) != NULL){
    printf("%s%s", value("PROMPT"), line);

    //Terminates line at right place to simulate input
    line[strlen(line)-2] = '\0';

    int parsed = parseLine(line);
    if(parsed == 1){
      createVar(line);
    }
    else if(parsed == 3){
      showShellVars();
    }
    else if(parsed == 4){
      displayUserVars();
    }
    lineNo++;
  }
}
