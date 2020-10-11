# Background
This program was written for my OS course. This program is a simple shell, similar to zsh or Bash, but without many of
their features. The purpose of the program is to demonstrate knowledge of system execution, multithreading, and forking.

# Installation
`git clone`  
`make`

# Removal
`make clean`

# Usage
`./smallsh` will start the shell.


The shell supports foreground commands, backgrounds commands (those that end with "&"), and piping. Output from background commands
is automatically sent to /dev/null, and a message will go to the console that the process, identified by PID number, has finished.
Please note that this message will not appear until the user has finished another command entry.


The shell supports SIGINT (CTRL-C) and SIGSTP (CTRL-Z) commands. SIGINT will terminate any foreground processes, and SIGSTP will
toggle the ability to run background processes.


Finally, there are three built-in commands: exit, cd, and status. `exit` will exit the shell, killing an background processes currently running first. A linked list data structure is used to monitor all currently running background processes. `cd` will
change the working direcotry of the shell. `status` will print the exit status of the last foreground process completed.

# Disclaimer
This program has only been testing on LINUX operating systems, and should not be trusted on Mac or Windows machines!
