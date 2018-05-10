#include "printer.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

void printLine(char* line){
  char *toPrint = malloc(8192); 

  char wordDelimiter[2] = " ";
  char segmDelimiter[2] = "\"";

  char *word = malloc(100);
  char *segment;

  int escaped = 0;

  while(line != 0){
    // Splits line into words
    word = strsep(&line, wordDelimiter);
    
    // Splits words into segments [split by ""]
    segment = strsep(&word, segmDelimiter);

    // Prints first segment
    printSeg(segment, escaped);

    // Prints remaining segments
    while(word != 0){
      segment = strsep(&word, segmDelimiter);
      escaped = (escaped-1) * -1;
      printSeg(segment, escaped);
    }

    // Prints a space to seperate words
    printf(" ");
  }

  // Prints a newline so that the PROMPT appears in a new line
  printf("\n");

  setExitcode(0);
}

void printSeg(char* segment, int escaped){
  if(escaped == 0){
    if(segment[0] == '$'){
      char *varname = malloc(200);

      int i = 0; // To iterate over a loop
      int s = 0; // Flag for possible string seperate from varname

      char *tempSeg = malloc(200);

      strcpy(tempSeg, segment);

      for(i = 0; 1 < strlen(tempSeg); i++){
        tempSeg++;

        // This condition checks whether the segment contains a string after the varname.
        // For example, "$HOMEstring"
        // In this case, the condition will break out of the loop once 's' appears.
        // The varname would be 'HOME', and the tempSeg would contain 'string'.
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
        return;
      }
    }
  }
  printf("%s", segment);
}
