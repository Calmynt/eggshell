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

}

void printSeg(char* segment, int escaped){
  if(escaped == 0){
    if(segment[0] == '$'){
      char *varname = segment+1;

      char *varvalue = value(varname);

      if(varvalue != 0){
        printf("%s", varvalue);
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
