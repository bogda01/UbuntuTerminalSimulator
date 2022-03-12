# UbuntuTerminalSimulator
this was an assignment for the operating system course, the program isn't fully done, here are the requirements:

Your project is to implement a shell-alike application, which acts like the Terminal / Konsole application in Ubuntu. Upon startup, it should show a command line asking the user to enter a command, for instance:

>_

[Part 1]
For your first milestone, you need to implement the basic functionality in your shell application. It should only respond to the exit command and it should terminate upon entering that command. Additionally, you need to add history support (using a key of your choice) - e.g. if the user presses the Up or DOwnarrow key, it should show the previous or next command in the terminal just like the real terminal does and auto-complete them in the terminal. Example:

>_ (pressing Up)
>ls_ (pressing Up)
>cat test.txt_ (pressing Down)
>ls_ (pressing Down)
>_

You also need to implement support for executing external commands, including support for piped commands (e.g. $ command1 | command2 | command3). Your application should be able to execute commands just like the real terminal does. Example:

>cat test.txt | grep "contents"
This is the contents of test.txt
>_

OPERATING SYSTEMS - FALL 2021
=============================

= PART 2 ==
===========

Scope: create a simple command line interpreter, able to interpret a set of internally-implemented commands.

Your task for this task assignment is to create a simple command line interpreter. The command line interpreter should offer the following facilities:

    * Accept user commands from command line. The interpreter will expose a specific prompt (e.g. '$>') showing that it is prepared to accept a user command.
    * Execute user commands, from the list of accepted commands. The following error situations MUST be considered:
          o Command execution returned an error. The exact error code must be reported.
          o Issued command is not an accepted command. Error information will contain 'offending' command name.
          o Arguments are in a wrong format: the only accepted format is "-o ARG", where ARG is the optional argument of your option. It is not accepted to use the '/' notation for options or the --option long notation for options. These errors should be identified during preprocessing of command line.

IMPLEMENTATION SUGGESTIONS
--------------------------
    * Maintain and manage some environmental information, including the root directory, current directory, prompt, or environmental variables (some of the implemented commands will address these issues).
    * Offer a command called "help", that will list all available commands with implemented options.
    * Offer a command called "version", that will offer author information, versioning information, and possibly other valuable information.
    * All commands will be implemented as external functions. A library of functions could be developed for this purpose.
          o Each command will accept the main calling convention: int yourCommandFunction (int argc, char *argv[]) ;. Implementation of the additional main parameter (char **envp) is not required.
          o Each command will return success or an error code (see error code manipulation, above).
          o Each command could report additional error information prior to command exit. E.g. a command that opens an existing file will report the fact that the file does not exist or that it has no permission to access the file.

REQUIREMENTS
------------
The following commands must be implemented, along with their parameters (see man help pages for information on each):

1. The "wc" command. Parameters that need to be implemented are: -c, -w, -l, -L

2. The "expand" command. Parameters that need to be implemented are: -t, -i, -a

3. The "env" command. Parameters that need to be implemented are: -u

4. Your program must also support pipes in the commands, e.g. > ls -l | grep -e 'tmp' | wc -l (it must work with any type of command!)

5. Your program must also support redirection in the commands, e.g. > ls -l > out.txt (it must work with any type of command!)

6. Your program must support both pipes and redirection in the same command line, e.g. > ls -l | grep -e 'tmp' > out.txt (it must work with any type of command!)

