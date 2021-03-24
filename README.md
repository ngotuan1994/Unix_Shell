# Unix_Shell

/* ******************************************
* Author: Tuan Ngo
* CWID: 887416766
* Instructor: William McCarthy
* Project: Unix_SHELL
* Date: 03/25/2021
* a C program to serve as a shell interface that accepts user commands and then executes.
*
* Some requirements : 
*	
* [X]	(1) Complete a Unix shell (osh) project that runs on any Linux, Unix or macOs system. <br>
* [X]	(2) Program is written in the C language, using fork() and exec() and wait() 
* [X]	(3) Shell program uses dup2() and pipe() 
* [X]    (4) Program creates a Unix shell with the parent process forking a child process
* [X]    (5) Shell can run concurrently, e.g., osh> cat program.c &
* [X]    (6) Shell can run in the background, e.g., osh> ls
* [X]    (7) Shell terminates when exit() is typed, e.g., osh> exit()
* [X]    (8) Shell allows multiple arguments to be passed through the command line
* [X]    (9)Has a history feature that remembers the shell commands typed, e.g.,    
    osh>ls -l    osh> !!will execute ls -l again
* [X]    (10)Allows redirection of input using the dup2() function, and the < and > symbolsosh> < input_file, or output,
    osh> > output_file, or both    osh> < input_file > output_file 
* [X]   (11)Allows the parent and child process to communicate with a pipe (sends the output of ls tothe less program)
    osh>ls -l | less 
*******************************************/
