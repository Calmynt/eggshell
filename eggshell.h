char *line;
char PATH[1024];
char PROMPT[128];
char CWD[1024];
char USER[128];
char HOME[128];
char TTY[128];
char EXEC[1024];

void getExecPath(char*);
void checkVars();
void initVars();
int getExitcode();
