
#pragma once

typedef struct variable{
  char* varname;
  char* value;
}Var;

typedef struct variables{
  Var** vars;
  int amount;
}Vars;

#define VARSIZE 1024
#define PATHMAX 1024

// Initialises the shell variables to be used.
void initShellVars();

// Shows all shell variables present.
void showShellVars();

// Creates a variable using an assignment line.
// Line should be checked for validity before passing it through.
void createVar(char* line);

// Gets the execution path for use with the $SHELL variable.
void getExecPath(char* PATH);

// Updates the current working directory.
// Used whenever the directory is changed.
void updateCWD();

void updatePrompt();

// Retrieves a variable struct with the same name as the parameter.
Var* retrieveVar(char* varname);

// Retrieves the value a variable represents.
// This is to avoid multiple VAR->varname with the retrieveVar function.
char* value(char* varname);

// Displays all user variables assigned within this session.
void displayUserVars();

// Sets the $EXITCODE variable to the parameter given.
void setExitcode(int ec);

// Retrieves all environment variables of the eggshell session.
// Used by retrieving all env variables of the system, and appending eggshell specific vars.
char** environEGG();

// Returns the index of the variable.
// If it returns -1, the var does not exist.
int varExists(char* varname);

// Checks whether the user is trying to assign a variable.
// If an assignment is detected, it is then attempted.
// The only return codes this function returns are 0 and 1.
// 0 if an assignment was detected, 1 if not.
// The assignment error codes are set in the Exitcode variable
int parse_var(char *line);
