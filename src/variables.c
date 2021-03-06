#include "variables.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>

Vars *variables;

int parse_var(char *line){
  int assign_check = 0;

  char *vardelimiter = "=";

  char *vartest = strdup(line);
  void *tofree = vartest;

  char *varname = strsep(&vartest, vardelimiter);

  // Checks for variable assignment
  if(vartest != 0){
    if(vartest[0] != ' ' && strstr(varname, " ") == 0){
      assign_check = 1;
      for(int i = 0; i < strlen(varname); i++){
        if(varname[i] < 65 || varname[i] > 90){
          assign_check = 0;
          break;
        }
      }

      if(assign_check == 1){
        createVar(line);
        setExitcode(0);
        return 0;
      }
    }
    else{
      if(varname[strlen(varname)-1] == ' ' && vartest[0] == ' '){
        setExitcode(-2);
      }
    }
  }
  free(tofree);
  return 1;
}

void updateCWD(){
  Var *cwdVar = retrieveVar("CWD");
  char cwd[VARSIZE];
  getcwd(cwd, sizeof(cwd));
  strcpy(cwdVar->value, cwd);
}

void updatePrompt(){
  Var *prompt = retrieveVar("PROMPT");
  sprintf(prompt->value, "<[%s] - [%s]> $ ", value("CWD"), value("EXITCODE"));
}

void initShellVars(char *ex){
  // Sets execution path and current working directory
  char *exec = getExecPath(ex);
  char cwd[VARSIZE];
  getcwd(cwd, sizeof(cwd));

  // Initialises variables array
  variables = (Vars *) malloc(sizeof(Vars));
  variables->vars = (Var **) calloc(1, sizeof(Var));
  variables->amount = 0;

  // Initialises shell variable-injection strings
  char *a, *b, *c, *d, *e, *f, *g, *h;
  a = malloc(VARSIZE * sizeof(char));
  b = malloc(VARSIZE * sizeof(char));
  c = malloc(VARSIZE * sizeof(char));
  d = malloc(VARSIZE * sizeof(char));
  e = malloc(VARSIZE * sizeof(char));
  f = malloc(VARSIZE * sizeof(char));
  g = malloc(VARSIZE * sizeof(char));
  h = malloc(VARSIZE * sizeof(char));

  // Creates strings to set shell variables
  sprintf(a,"PATH=%s", getenv("PATH"));
  sprintf(b,"USER=%s", getenv("USER"));
  sprintf(c,"HOME=%s", getenv("HOME"));
  sprintf(e,"CWD=%s", cwd);
  sprintf(f,"TERMINAL=%s", ttyname(STDIN_FILENO));
  sprintf(g,"SHELL=%s", exec);
  sprintf(h,"EXITCODE=%s", "(none)");
  sprintf(d,"PROMPT=<Welcome to Eggshell> $ ");

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
  free(exec);
}

char* getExecPath(char *ex){
  char *temp = malloc(VARSIZE);
  char *cwd = getenv("PWD");
  if(ex[0] == '/') sprintf(temp, "%s", ex);
  else sprintf(temp, "%s/%s",cwd,ex);
  char *PATH = realpath(temp, NULL);
  free(temp);
  return PATH;
}

void showShellVars(){
   printf("CWD=%s\n", value("CWD"));
   printf("PATH=%s\n", value("PATH"));
   printf("PROMPT=%s\n", value("PROMPT"));
   printf("USER=%s\n", value("USER"));
   printf("HOME=%s\n", value("HOME"));
   printf("TERMINAL=%s\n", value("TERMINAL"));
   printf("SHELL=%s \n", value("SHELL"));
   printf("EXITCODE=%s \n", value("EXITCODE"));
}

char** environEGG(){
  extern char **environ; // brings all environment variables

  // Loop that places all 8 shell variables into the environment.
  for(int i = 0; i < 8; i++){
    Var *envVar = variables->vars[i];
    setenv(envVar->varname, envVar->value, 1);
  }

  return environ;
}

void createVar(char* line){
  char* name = malloc(50 * sizeof(char));
  char* str = malloc(VARSIZE * sizeof(char));
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
      if(var == 0){fprintf(stderr, "Variable does not exist!\n"); setExitcode(-1); return;}
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
    if(var == 0){fprintf(stderr, "Variable does not exist!\n"); setExitcode(-1); return;}
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
  Var** vars = (Var**) realloc(variables->vars, (a)*sizeof(Var));
  variables->vars = vars;

  if(retrieveVar("EXITCODE") == 0){
    return;
  }
  setExitcode(0);
}

Var* retrieveVar(char* varname){
  int var = varExists(varname);
  if(var != -1){
    return variables->vars[var];
  }
  return 0;
}

char* value(char* varname){
  Var *variable = retrieveVar(varname);

  if(variable != 0){
    return variable->value;
  }
  else{
    return 0;
  }
}

void displayUserVars(){
  printf("--- VARIABLES ---\n");
  for(int x = 8; x < variables->amount; x++){
    if(variables->vars[x] == 0){ printf("ERR\n"); setExitcode(-1); return;}
    printf("%s=%s\n", variables->vars[x]->varname, variables->vars[x]->value);
  }
  setExitcode(0);
}

void setExitcode(int ec){
  int status;
  char *exitcode;

  Var *exit = retrieveVar("EXITCODE");
  // printf("%d\n", ec);
  sprintf(exit->value, "%d", ec);;
}

int varExists(char *varname){
  for(int i = 0; i < variables->amount; i++){
    if(strcmp(variables->vars[i]->varname, varname) == 0){
      return i;
    }
  }

  return -1;
}
