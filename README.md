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
     *Example :* `print nice >> hello.txt` will append *nice* to `hello.txt`.
                 `print nice > hello.txt` will overwrite `hello.txt` with *nice*.
   
 - `external commands` - Every command your shell offers can also be used!
   - For example, if `ls` is run, `eggshell` will realise that it's not an internal command, and will redirect it to be executed by whichever shell is being used by the system its being run on.

## Notes

  - Doing `echo $[variable name]` will not work, as `echo` is an external command, and with all external commands, `$` is processed by a shell. However, the eggshell environment variables *ARE* still present, as can be proven by running the internal command `all`, and the external command `env`.

  - Coverage is low and builds are failing mostly because of all the code that handles the signals. They cannot be tested via a text file, so all that code is 
  being ignored. How **rude**.
