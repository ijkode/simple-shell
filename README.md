Simple Shell
Authored by Liran Libster

==Description==

This program is a simple shell that runs given commands.

Functions:

buildArr - this function receives a string that the user inputs and the number of words in the string, builds a 2D array and return the array. 

countWords - this function counts how many words in the string.

freeArray - this function runs in all cells of the 2D array and free the allocated memory.

printDir - prints the user id and the folder direction.

pipeCount - counts how many pipes entered.

onePipe - function that runs commands with 1 pipe.

twoPipes - function that runs commands with 2 pipes.

sigtstpHandler - handler that catches signal if process has stopped, this handler helps to save the stopped process pid (to use it later with fg) and then send signal to stop the son. the father process ignores the signal.

sigchldHandler - handler that get signal each time when child process stopped or terminated, this handler helps to clear the 'zombie' status processes.

==Program files==

ex4.c

==How to compile==

compile: gcc ex4.c -o ex4
run: ./ex4

==Input==

sentence followed by enter

example: ls -l

example 2: ls -l|wc -l

example 3: ls -l|sort|wc

example 4: wc, next press ctrl+z and then type fg

to exit type: done

==Output==

example 1: will display a list of files or directories with information about each one. 

example 2: prints how many files in the current folder.

example 3: will do ls -l, the output will be sorted and then the wc will count, then print the output.

example 4: the terminal starts wc command, after pressing ctrl+z the wc proccess will stop and the father procces will ignore the signal and continue as usual, then when you will type fg, the wc process will come back to foreground.

when exiting: the program will end by printing number of commands and number of pipes, and then see you next time!

exit example:

"Number of commands: 4

Number of pipes: 2

See you Next time !"
