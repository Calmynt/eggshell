program:
	clang -std=gnu99 -g -O0 -o eggshell main.c eggshell.c \
	linenoise.c variables.c printer.c \
	proc_manager.c sig_handler.c
