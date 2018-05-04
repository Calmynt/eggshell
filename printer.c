#include "printer.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

void printLine(char* line){
  char wordDelimiter[2] = " ";
  char segmDelimiter[2] = "\"";

  char *wordSAVE;
  char *segmSAVE;

  char *word = malloc(100);
  char *segment;

  int escaped = 0;

  while(line != 0){
    word = strsep(&line, wordDelimiter);
    segment = strsep(&word, segmDelimiter);

    printSeg(segment, escaped);

    while(word != 0){
      segment = strsep(&word, segmDelimiter);
      escaped = (escaped-1) * -1;
      printSeg(segment, escaped);
    }

    printf(" ");
  }

  printf("\n");

  setExitcode(0);
}

void printSeg(char* segment, int escaped){
  if(escaped == 0){
    if(segment[0] == '$'){
      char *varname = malloc(200);

      int i = 0;
      int s = 0;

      char *tempSeg = malloc(200);

      strcpy(tempSeg, segment);

      for(i = 0; 1 < strlen(tempSeg); i++){
        memmove(tempSeg, tempSeg+1, sizeof(tempSeg));
        if(tempSeg[0] < 65 || tempSeg[0] > 90){
          strncpy(varname, segment+1, i);
          s = 1;
          break;
        }
      }

      if(s == 0){
        strcpy(varname, segment+1);
      }

      char *varvalue = value(varname);

      if(varvalue != 0){
        if(s != 0){
          printf("%s", varvalue);
          printf("%s", tempSeg);
        }
        else{
          printf("%s", varvalue);
        }
      }
      else{
        printf("%s", segment);
      }
    }
    else{
      printf("%s", segment);
    }
  }

  else{
    printf("%s", segment);
  }
}
