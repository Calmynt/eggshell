#pragma once

// Parses the line to check for pipe symbols.
// If it finds one, it proceeds to execute the command itself using its executer.
// If not, it returns an exitcode telling the main parses to continue.
int pipe_parser(char *line);

// A special executer that is compatible with piping.
// Used as its additional pipe feature bloats the executer,
//  so it was seperated from the main executer.
int pipe_executer(char **commands);