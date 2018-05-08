#pragma once
#define OUT 0
#define IN 1

// These parse the line given to seperate the command from the filename
// The "filename" in the input here string case would be the literal string.
char *out_redirect_parse(char *append, char *rewrite, char *line);
char *in_redirect_parse(char *in_file, char *in_string, char *line);

// Initialises the redirect depending on which flags were activated.
// Returns the file descriptor to be used with the redirections.
int init_redirect(char *filename);

// Proceeds to redirect input and output depending on the file descriptor.
int redirect_in(int filefd);
int redirect_out(int filefd);

// Closes the redirects and resets the flag variables.
void close_redirects(int direction, int filefd);