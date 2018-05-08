program:
	clang -std=gnu99 -g -O0 -o eggshell main.c eggshell.c \
	./add-on/linenoise.c ./src/variables.c ./src/printer.c \
	./src/proc_manager.c ./src/sig_handler.c ./src/redirection.c
