program:
	gcc -std=gnu99 -g -O0 -o eggshell main.c eggshell.c eggshell.h \
	linenoise.c linenoise.h variables.c variables.h printer.c printer.h \
	externalcmd.c externalcmd.h sig_handler.c sig_handler.h