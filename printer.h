#include "variables.h"

/* Prints a line, incl. variable support */
void printLine(char* line);

// A subset of the printLine function, prints the segments of the line.
// segment is the actual segment to be considered.
// escaped is whether the segment is escaped within "" or not.   
void printSeg(char* segment, int escaped);
