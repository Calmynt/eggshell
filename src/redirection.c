#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

#include "redirection.h"
#include "variables.h"

// Flag variables for redirection
// Append - Overwrite - Input File - Input String respectively
int app = 0;
int ovr = 0;
int inf = 0;
int ins = 0;

int save_out = 0;
int save_in = 0;

char *out_redirect_parse(char *append, char *rewrite, char *line){
    // Sets filename to redirect output to
    char *filename;

    if(append != 0){
      filename = append+2;
      app = 1;
    }
    else{
      filename = rewrite+1;
      ovr = 1;
    }
    line = strsep(&line, ">");
    line[strlen(line)-1] = 0;

    while(filename[0] == ' '){
      filename++;
    }

    return filename;
}

char *in_redirect_parse(char *in_file, char *in_string, char *line){
    char *filename;

    if(in_string != 0){
      filename = in_string+3;
      ins = 1;
    }
    else{
      filename = in_file+1;
      inf = 1;
    }

    line = strsep(&line, "<");
    line[strlen(line)-1] = 0;

    while(filename[0] == ' '){
      filename++;
    }

    return filename;
}

int init_redirect(char *filename){
    int filefd = -255;

    if(app == 1){
        filefd = open(filename, O_WRONLY|O_CREAT|O_APPEND, 0666);
    }
    else if(ovr == 1){
        filefd = open(filename, O_WRONLY|O_CREAT|O_TRUNC, 0666);
    }
    else if(inf == 1){
        filefd = open(filename, O_RDONLY);
    }
    else if(ins == 1){
        FILE *f = fopen("stdin.tmp", "w+");
        fprintf(f, "%s", filename);

        filefd = open("stdin.tmp", O_RDONLY);
        fclose(f);
    }

    return filefd;
}

int redirect_out(int filefd){
    save_out = dup(fileno(stdout));

    if(dup2(filefd, fileno(stdout)) == -1){
      perror("Failed in redirecting stdout");
      setExitcode(255);
      return 255;
    }

    return 0;
}

int redirect_in(int filefd){
    save_in = dup(fileno(stdin));

    if(dup2(filefd, fileno(stdin)) == -1){
      perror("Failed in redirecting stdin");
      setExitcode(255);
      return 255;
    }

    return 0;
}

void close_redirects(int direction, int filefd){
    if(direction == OUT){
        fflush(stdout);
        close(filefd);

        dup2(save_out, fileno(stdout));
        close(save_out);
    }
    else if(direction == IN){
        fflush(stdin);
        close(filefd);

        dup2(save_in, fileno(stdin));
        close(save_in);

        if(ins == 1){
            remove("stdin.tmp");
        }
    }

    app = ovr = inf = ins = save_in = save_out = 0;
}