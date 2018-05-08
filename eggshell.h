#pragma once

char *line;

#include <stdio.h>

#include "./src/variables.h"
#include "./src/printer.h"
#include "./src/proc_manager.h"
#include "./src/sig_handler.h"

/* Initialises the eggshell */
void initEggshell();

/* Tests the shell using a testfile called "testinput.txt" */
void runScript(char* filename);

// Changes the directory of the eggshell
void changeDirectory(char* directory);

// Executes the command with varargs *line
// As well as execute command, it also initialises the necessary signal handlers.
void runLine(char *command, char *line);

// The main core of the eggshell
// This is what the user will be interacting with most of the time
// It acts as a parser, utilising multiple methods to parse the line itself.
// After this, functions are called to execute the line.
void execute(char* line);