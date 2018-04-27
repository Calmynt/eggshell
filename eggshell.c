#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <limits.h>
#include <sys/wait.h>
#include "eggshell.h"
#include "linenoise/linenoise.h"

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

// Works -- Retruns the path to the eggshell executable
void getExecPath(char *PATH){
  char p[128];
  pid_t pid = getpid();
  sprintf(p, "/proc/%d/exe", pid);
  if (readlink(p, PATH, PATH_MAX) == -1)
    perror("readlink");
}

// Works -- Displays all global variables for testing
void showShellVars(){
   printf("CWD=%s\n", variableValue("CWD"));
   printf("PATH=%s\n", variableValue("PATH"));
   printf("PROMPT=%s\n", variableValue("PROMPT"));
   printf("USER=%s\n", variableValue("USER"));
   printf("HOME=%s\n", variableValue("HOME"));
   printf("TERMINAL=%s\n", variableValue("TERMINAL"));
   printf("EXEC=%s \n", variableValue("EXEC"));
}

// Works -- Initialises the eggshell
void initEggshell(){
  char *exec = malloc(1024 * sizeof(char));
  getExecPath(exec);
  char cwd[1024];
  getcwd(cwd, sizeof(cwd));

  variables = (Vars *) malloc(sizeof(Vars));
  variables->vars = (Var **) calloc(1, sizeof(Var));
  variables->vars[0] = (Var *) malloc(sizeof(Var));
  variables->amount = 0;

  char *a, *b, *c, *d, *e, *f, *g;
  a = malloc(1024 * sizeof(char));
  b = malloc(1024 * sizeof(char));
  c = malloc(1024 * sizeof(char));
  d = malloc(1024 * sizeof(char));
  e = malloc(1024 * sizeof(char));
  f = malloc(1024 * sizeof(char));
  g = malloc(1024 * sizeof(char));

  printf("AAAAA\n");
  sprintf(a,"PATH=%s", getenv("PATH"));
  sprintf(b,"USER=%s", getenv("USERNAME"));
  sprintf(c,"HOME=%s", getenv("HOME"));
  sprintf(d,"PROMPT=%s", "eggshell-1.0 > ");
  sprintf(e,"CWD=%s", cwd);
  sprintf(f,"TERMINAL=%s", ttyname(STDIN_FILENO));
  sprintf(g,"EXEC=%s", exec);
  printf("BBBBB\n");

  createVar(a);
  createVar(b);
  createVar(c);
  createVar(d);
  createVar(e);
  createVar(f);
  createVar(g);
}

// TODO : Need to check
int getExitcode(pid_t p){
  int status;

  if ( waitpid(p, &status, 0) == -1 ) {
    perror("Failure in function - Waitpid");
    return -1;
  }

  if ( WIFEXITED(status) ) {
    int es = WEXITSTATUS(status);
    return es;
  }
}
  char delimiter[2] = " ";

/* Returns a 'Parse Code' that deciphers what the line is trying to achieve.
   1    = 'Assignment'
   2    = 'Print'
   -1   = 'Error in parsing'
   100  = 'External command' */
int parseLine(char* line){
  if(strcmp(line, "exit") == 0) exit(0);

  // Parse check for assignment
  for(int i = 1; i < strlen(line); i++){
    if(line[i] == '=' && line[i-1] != ' ' && line[i+1] != ' '){
      for(int j = i-1; j >= 0; j--){
        // Returns error code if variable name is invalid
        if(line[j] < 65 || line[j] > 90){
          fprintf(stderr, "Invalid assignment - Variable names should be capitalised and alphanumeric!\nEx. 'HOME=3'\n");
          return -1;
        }
      }

      return 1;
    }

    // Returns error code if spaces surround =
    else if(line[i] == '=' && (line[i-1] == ' ' || line[i+1] == ' ')){
      fprintf(stderr, "Invalid assignment - No spaces surround '='\nEx. 'HOME=3'\n");
      return -1;
    }

  }

  char *command = strtok(line, delimiter);

  if(strcmp(command, "print") == 0) return 2;

  return 100;
}

void createVar(char* line){
  char* name = malloc(50 * sizeof(char));
  char* str = malloc(200 * sizeof(char));

  int i = 0;
  int before;

  for(i = 0; i < strlen(line); i++){
    if(line[i] == '='){break;}
  }

  for(before = 0; before < i; before++){
    name[before] = line[before];
  }

  name[before+1] = '\0';


  int j = 0;
  for(int x = i+1; x < strlen(line); x++){
    str[j] = line[x];
    j++;
  }
  str[j] = '\0';

  Var *variable = retrieveVar(name);

  if(variable != 0){
    if(str[0] != '$'){
      variable->varname = name;
      variable->value = str;
    }
    else{
      memmove(str, str+1, strlen(str));
      Var *var = retrieveVar(str);
      if(var == 0){fprintf(stderr, "Variable does not exist!\n"); return;}
      variable->varname = name;
      strcpy(variable->value, var->value);
    }
    return;
  }

  if(str[0] == '$'){
    memmove(str, str+1, strlen(str));
    Var *var = retrieveVar(str);
    if(var == 0){fprintf(stderr, "Variable does not exist!\n"); return;}
    strcpy(str, var->value);
  }

  int a = variables->amount;

  variables->vars[a] = (Var*)malloc(sizeof(Var));
  variables->vars[a]->value = str;


  variables->vars[a]->varname = name;

  variables->amount++;
  a = variables->amount;

  Var** vars = (Var**) realloc(variables->vars, a*sizeof(Var));
  variables->vars = vars;
  variables->vars[a] = (Var *) malloc(sizeof(Var));
}

Var* retrieveVar(char* varname){
  for(int i = 0; i < variables->amount; i++){
    if(strcmp(variables->vars[i]->varname, varname) == 0){
      return variables->vars[i];
    }
  }
  return 0;
}

void displayUserVars(){
  printf("--- VARIABLES ---\n");
  for(int x = 7; x < variables->amount; x++){
    if(variables->vars[x] == 0){ printf("ERR\n"); return;}
    printf("%s=%s\n", variables->vars[x]->varname, variables->vars[x]->value);
  }
}

char* variableValue(char* varname){
  Var *variable = retrieveVar(varname);
  return variable->value;
}
