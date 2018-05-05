typedef struct variable Var;
typedef struct variables Vars;

#define VARSIZE 1024

void initShellVars();
void showShellVars();
void createVar(char* line);
void getExecPath(char* PATH);
void updateCWD();
Var* retrieveVar(char* varname);
char* value(char* varname);
void displayUserVars();
Var* retrieveVar(char*);
char* setExitcode(int);
char** environ();
int varExists(char* varname);
