typedef struct variable Var;
typedef struct variables Vars;

void initShellVars();
void showShellVars();
void createVar(char* line);
void getExecPath(char* PATH);
Var* retrieveVar(char* varname);
char* value(char* varname);
void displayUserVars();
Var* retrieveVar(char*);
char* setExitcode(int);
int varExists(char* varname);
