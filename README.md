# Eggshell
A tiny shell made for personal purposes.

[![Build Status](https://travis-ci.org/MasterTextman/eggshell.svg?branch=master)](https://travis-ci.org/MasterTextman/eggshell)
[![codecov](https://img.shields.io/codecov/c/github/MasterTextman/eggshell.svg)](https://codecov.io/gh/MasterTextman/eggshell)

After building the shell with `make`, you can run either one of two commands:
 - `./eggshell`
 - `./eggshell test`

The first one will run the **eggshell** normally, whereas the second one will
run some tests of the eggshell. The tests are run using the `testinput.txt` file.
Each line represents a seperate command that **eggshell** will try to execute.

The second command is equivalent to running the shell, and entering the command `source testinput.txt`.

## Different commands
 - `source <filename>` - executes commands from a file, effectively using it as a script
 - `all` - prints out all the global shell variables belonging to eggshell, such as `$HOME` and `$EXITCODE`.
 - `vars` - prints out all user-made variables present in eggshell.
 - `print <string>` - prints out `<string>`, replacing any variables such as `$HOME` with their actual value, *unless* they are put between `" "`

## Other commands
 - `variable assignment` - variables can be created or modified.
   - `VAR=USER` - Creates a variable `VAR`, setting it to `USER`.
   - `VAR=$USER` - Creates a variable `VAR`, setting it to the value `$USER` represents.
   - ***Note :*** *If the name on the lefthand side belongs to an already existing variable, it will be overwritten, rather than creating a new variable with the same name.*
   
 - The shell also has external command support.
   - For example, if `ls` is run, `eggshell` will realise that it's not an internal command, and will redirect it to be executed by whichever shell is being used by the system its being run on.

## Notes
  - Doing `echo $[variable name]` will not work, as `echo` is an external command, and with all external commands, `$` is processed by a shell. However, the eggshell environment variables *ARE* still present, as can be proven by running the internal command `all`, and the external command `env`.
