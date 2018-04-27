char *line;
char PATH[1024];
char PROMPT[128];
char CWD[1024];
char USER[128];
char HOME[128];
char TTY[128];
char EXEC[1024];

typedef struct variable Var;
typedef struct variables Vars;

void getExecPath(char*);
void checkVars();
void initEggshell();
void createVar();
void displayUserVars();
void showShellVars();
char* variableValue(char*);
Var* retrieveVar(char*);
int getExitcode();
int parseLine();
