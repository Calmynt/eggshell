#include "eggshell.h"
#include "linenoise/linenoise.h"
#include <stdio.h>

int main(int argc, char *argv[]){
  initEggshell();
  char *prompt = value("PROMPT");
  while((line = linenoise(prompt)) != NULL) {
    int parsed = parseLine(line);
    if(parsed == 1){
      createVar(line);
    }
    showShellVars();
    linenoiseFree(line); /* Or just free(line) if you use libc malloc. */
  }
}
