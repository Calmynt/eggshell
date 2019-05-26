# Eggshell

A tiny shell made for personal purposes.

[![Build Status](https://travis-ci.org/Calmynt/eggshell.svg?branch=master)](https://travis-ci.org/MasterTextman/eggshell)
[![codecov](https://img.shields.io/codecov/c/github/Calmymt/eggshell.svg)](https://codecov.io/gh/MasterTextman/eggshell)

## Compilation

The `Makefile` presented uses `clang`. If you have `clang` installed, or are ok with installing it, leave the Makefile as it is.
However, if you'd rather use the default `gcc` compiler that's pretty standard, you can run `switch.sh` and the Makefile will
change to a `gcc` equivalent.

If you can't run `./switch.sh`, try doing `chmod 777 ./switch.sh`. It will make the script executable.

The reason I switched to Clang is for the much more helpful warnings and errors that it threw at me.

## Execution

After building the shell with `make` with your Makefile of choise, you can run either one of two commands:

- `./eggshell`
- `./eggshell test`

The first one will run the **eggshell** normally, whereas the second one will
run some tests of the eggshell. The tests are run using the `testinput.txt` file.
Each line represents a seperate command that **eggshell** will try to execute.

The second command is equivalent to running the shell, and entering the command `source testinput.txt` then exiting.

## Different commands

- `source <filename>` - executes commands from a file, effectively using it as a script
  - ***Note :*** The files sourced by eggshell have a commenting system. Lines starting with whitespace of `#` will be ignored.
- `all` - prints out all the global shell variables belonging to eggshell, such as `$HOME` and `$EXITCODE`.
- `vars` - prints out all user-made variables present in eggshell.
- `print <string>` - prints out `<string>`, replacing any variables such as `$HOME` with their actual value, *unless* they are put between `" "`

## Additional features

- `variable assignment` - variables can be created or modified.
  - `VAR=USER` - Creates a variable `VAR`, setting it to `USER`.
  - `VAR=$USER` - Creates a variable `VAR`, setting it to the value `$USER` represents.
  - ***Note :*** *If the name on the lefthand side belongs to an already existing variable, it will be overwritten, rather than creating a new variable with the same name.*

- `process management` - You can manage processes using various commands:
  - `&` - Putting `&` at the end of the string makes the command run in the background.
  - `<CTRL-C>` and `<CTRL-Z>` - Pressing these key combinations will *interrupt* and *suspend* the currently running process respectively.
  - `fg` and `bg` - These will recover the last *suspended* processes in the foreground/background respectively.

- `redirection` - You can redirect output/input as well:
  - `>>` and `>` - The former will append to a file, whereas the latter will overwrite it. Both will create a file if it doesn't exist however.
    - *Example 1:* `print nice >> hello.txt` will append *nice* to `hello.txt`.
    - *Example 2:* `print nice > hello.txt` will overwrite `hello.txt` with *nice*.
  - `<` and `<<<` - The former will retrieve the contents of a file, whereas the latter will use whatever is after it as a string.
    - *Example 1:* `cat < hello.txt` will place the contents of `hello.txt` onto the `stdin` of `cat`.
    - *Example 2:* `cat <<< hello.txt` will place *hello.txt* as a string onto the `stdin` of `cat`.

- `external commands` - Every command your shell offers can also be used!
  - For example, if `ls` is run, `eggshell` will realise that it's not an internal command, and will redirect it to be executed by whichever shell is being used by the system its being run on.

- `piping` - Piping between programs is now also possible!
  - You can pipe between 2 or more programs, for example `print cat | wc` and `echo nice | wc | figlet | wc`

## Notes

- Doing `echo $[variable name]` will not work, as `echo` is an external command, and with all external commands, `$` is processed by a shell. However, the eggshell environment variables *ARE* still present, as can be proven by running the internal command `all`, and the external command `env`.
