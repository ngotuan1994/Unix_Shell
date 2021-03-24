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
*[X]	(1) Complete a Unix shell (osh) project that runs on any Linux, Unix or macOs system. 
*[X]	(2) Program is written in the C language, using fork() and exec() and wait() 
*[X]	(3) Shell program uses dup2() and pipe() 
*[X]    (4) Program creates a Unix shell with the parent process forking a child process
*[X]    (5) Shell can run concurrently, e.g., osh> cat program.c &
*[X]    (6) Shell can run in the background, e.g., osh> ls
*[X]    (7) Shell terminates when exit() is typed, e.g., osh> exit()
*[X]    (8) Shell allows multiple arguments to be passed through the command line
*[X]    (9)Has a history feature that remembers the shell commands typed, e.g.,    
    osh>ls -l    osh> !!will execute ls -l again
*[X]    (10)Allows redirection of input using the dup2() function, and the < and > symbolsosh> < input_file, or output,
    osh> > output_file, or both    osh> < input_file > output_file 
*[X]   (11)Allows the parent and child process to communicate with a pipe (sends the output of ls tothe less program)
    osh>ls -l | less 
*********************************************/

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <stdbool.h>
// The maximum length command 
#define MAXLINE 80


void freeinput(char *input[],int c)
{
    int i = 0;
    while(input[i] != NULL && (i < c))
    {
        free(input[i]);
        ++i;
        if (i == 80)
        {
            break;
        } 
    }
}
void exec_pipe(char* input[], int *hasAmp, int *c)
{
    int pid = fork();
    if (pid == 0) 
    {          
        //child
        for (int i = 1 ; i< *c-1 ; i++)
        {
            if (strcmp(input[i], "|") == 0) 
            {
                // case "|" . Needs use pipe
                int fd[2];
                if (pipe(fd) == -1) 
                {
                    fprintf(stderr, "Pipe Failed\n");
                    return ;
                }
                // split 2 commands, put a head of pipe in stdout and exec first command to get the first result
                // put a tail of pipe in stdin, exec second command
                char *cm1[i + 1];
                char *cm2[*c - i - 1 + 1];
                for (int j = 0; j < i; ++j) 
                {
                    cm1[j] = input[j];
                }
                cm1[i] = NULL;
                for (int j = 0; j < *c - i - 1; ++j) 
                {
                    cm2[j] = input[j + i + 1];
                }
                cm2[*c - i - 1] = NULL;

                int pid_pipe = fork();
                if (pid_pipe > 0) 
                {
                    wait(NULL);
                    close(fd[1]);
                    dup2(fd[0], STDIN_FILENO);
                    close(fd[0]);
                    if (execvp(cm2[0], cm2) == -1) 
                    {
                        printf("Invalid Command!\n");
                        return;
                    }

                } 
                else if (pid_pipe == 0) 
                {

                    close(fd[0]);
                    dup2(fd[1], STDOUT_FILENO);
                    close(fd[1]);
                    if (execvp(cm1[0], cm1) == -1) 
                    {
                        printf("Invalid Command!\n");
                        return ;
                    }
                    exit(1);
                }
                close(fd[0]);
                close(fd[1]);
                break;
            }
        }
    }
    else if ( pid > 0)
    {
        //parent
        if( *hasAmp ==0)
        {
             wait(NULL);
        }
    }
    else
    {
        perror("Fork Failed");
    }
}
void analysisCM(char *input[], int *hasAmp, int *c) {
 
    // declare variables 
    char commands[MAXLINE];
    int count = 0;
    const char DELIMETER[]= " ";

    // reads from stdin
    count = read(STDIN_FILENO, commands, 80);


    // delete \n (endline) from user input
    if (commands[count - 1] == '\n')
    {
        commands[count - 1] = '\0';
    }

    // Checks prompt from user command. If it is "!!", print out no command in history and return
    // old commands still save in input
    if (strcmp(commands, "!!") == 0) {
        if (*c == 0)
        {
            printf("No commands in history.\n");
        }
        return;
    }
    
    //Release old command
    freeinput(input, *c);
    *c = 0;
    // has ampersand
    *hasAmp = 0;

    // split all commands from prompt and put it in the array input
    
    char *p = strtok(commands, DELIMETER);
    while (p != NULL) {
        if (p[0] == '&') {
            *hasAmp = 1;
            p = strtok(NULL, DELIMETER);
            continue;
        }
        *c += 1;
        input[*c - 1] = strdup(p);
        p = strtok(NULL, DELIMETER);
    }

    // change the last element = NULL to execvp know
    input[*c] = NULL;
    
}
void exec(char *input[], int *hasAmp, int *c)
{
   
    int fd[2]={-1,-1};
    while(*c >= 3)
    {
		if(strcmp(input[*c-2], ">") == 0)
        {	
            //case ">" output from file
			fd[1] = open(input[*c-1], O_CREAT|O_WRONLY|O_TRUNC, S_IRUSR | S_IWUSR | S_IRGRP|S_IWGRP);
			if(fd[1] == -1){
				printf("open");
				return;
			}
			input[*c-2] = NULL;
			*c -= 2;
		}
        else if(strcmp(input[*c-2], "<") == 0)
        { 
            // case "<" input from file  Open for reading only
			fd[0] = open(input[*c-1], O_RDONLY);
			if(fd[0] == -1){
				printf("open");
				return;
			}
			input[*c-2] = NULL;
			*c -= 2;
		}

    }
	int status;
    
    //fork()
	pid_t pid = fork();	
    
    // <=-1 fork failed
	if(pid<=-1)	
    {
		perror("Fork failed");
    }
    //child	
    else if(pid ==0)
    {	
            fflush(stdout);
			if(fd[0] != -1)
            {	
				if(dup2(fd[0], STDIN_FILENO) != STDIN_FILENO)
                {
					perror("dup2 fd[0] error");
					exit(1);
				}
			}
			if(fd[1] != -1)
            {	
				if(dup2(fd[1], STDOUT_FILENO) != STDOUT_FILENO)
                {
					perror("dup2 fd[1] error");
					exit(1);
				}
			}
			execvp(input[0], input);
			perror("execvp error");
			exit(0);
    }
    // parent
	else
    { 
		close(fd[0]);close(fd[1]);
        // if has & , don't have to wait child
		if(!*hasAmp)
        {
            waitpid(pid, &status, 0);
        }
	}
}
int main()
{
    // command line arguments
    char* input[MAXLINE/2+1];
  
 
    // flag to determine when to exit program
    bool finished = false;
    pid_t pid;
    int hasAmp = 0;
    int c = 0;
    label:
    while (!finished) 
    {
        printf("osh> ");        
        fflush(stdout);
        
        analysisCM(input, &hasAmp, &c);
        //exit program when user enter "exit()"
        for( int i = 0 ; i < c - 1; ++i)
            if(strcmp(input[i],"|")==0){
                exec_pipe(input,&hasAmp,&c);
                goto label;
                
            }
        if(strcmp(input[0],"exit()") == 0)
        {
            printf("osh exited\n");
            printf("program finished\n");

            return 0;
        }
        else{
            exec(input,&hasAmp,&c);
            
        }
    }
    return 0;
}
/*
* How to run shell.c
* command: make
* It will be automatically compile shell.c and run the shell program. 
* Now you can use osh shell by our shell program
* to delete the shell program
* command: make clean
*/
//========================================================================
// OUTPUT (your output will vary)
//========================================================================
// eric@lenovo:~/Downloads/testing/testing01$ make
// gcc shell.c -o shell 
// ./shell
// osh> exit()
// osh exited
// program finished
// eric@lenovo:~/Downloads/testing/testing01$ make
// gcc shell.c -o shell 
// ./shell
// osh> cat Makefile
// CC=gcc
// TARGET=shell
// all:
//         $(CC) $(TARGET).c -o $(TARGET) 
//         ./$(TARGET)
// clean:
//         rm $(TARGET)
// osh> ls
// int.txt  Makefile  online_academy  shell  shell.c  simple_shell.c  test.c
// osh> exit() 
// osh exited
// program finished
// eric@lenovo:~/Downloads/testing/testing01$ make
// gcc shell.c -o shell 
// ./shell
// osh> ls -l
// total 64
// -rw-rw-r-- 1 eric eric    16 Mar 20 14:39 int.txt
// -rw-rw-r-- 1 eric eric    75 Mar 20 17:03 ls.txt
// -rw-rw-r-- 1 eric eric    92 Mar 20 17:01 Makefile
// drwxrwxr-x 5 eric eric  4096 Mar 11 16:21 online_academy
// -rwxrwxr-x 1 eric eric 17704 Mar 20 17:06 shell
// -rw-rw-r-- 1 eric eric  8360 Mar 20 17:00 shell.c
// -rw-rw-r-- 1 eric eric  6413 Mar 20 16:57 simple_shell.c
// -rw-rw-r-- 1 eric eric  6922 Mar 20 16:35 test.c
// osh> !!
// total 64
// -rw-rw-r-- 1 eric eric    16 Mar 20 14:39 int.txt
// -rw-rw-r-- 1 eric eric    75 Mar 20 17:03 ls.txt
// -rw-rw-r-- 1 eric eric    92 Mar 20 17:01 Makefile
// drwxrwxr-x 5 eric eric  4096 Mar 11 16:21 online_academy
// -rwxrwxr-x 1 eric eric 17704 Mar 20 17:06 shell
// -rw-rw-r-- 1 eric eric  8360 Mar 20 17:00 shell.c
// -rw-rw-r-- 1 eric eric  6413 Mar 20 16:57 simple_shell.c
// -rw-rw-r-- 1 eric eric  6922 Mar 20 16:35 test.c
// osh> cat int.txt
// 1
// 5
// 1
// 7
// 9
// 3
// 2
// 5
// osh> sort < int.txt
// 1
// 1
// 2
// 3
// 5
// 5
// 7
// 9
// osh> sort < int.txt > sort.txt
// osh> cat sort.txt
// 1
// 1
// 2
// 3
// 5
// 5
// 7
// 9
// osh> pwd
// /home/eric/Downloads/testing/testing01
// osh> cat sort.txt &
// osh> 1
// 1
// 2
// 3
// 5
// 5
// 7
// 9
// ls
// int.txt  k.txt  list.txt  ls.txt  main.c  Makefile  shell  shell.c  simple  simple_shell.c  sort.txt  test01  test01.c  test.c
// total 200
// -rw-rw-r-- 1 eric eric   279 Mar 21 10:41 abc.c
// -rwxrwxr-x 1 eric eric 17856 Mar 21 19:05 a.out
// -rw-rw-r-- 1 eric eric    16 Mar 20 14:39 int.txt
// -rw-rw-r-- 1 eric eric   110 Mar 20 18:57 khong.c
// -rw-rw-r-- 1 eric eric   961 Mar 21 13:04 k.txt
// -rw-r--r-- 1 eric eric   913 Mar 21 01:03 list.txt
// -rw-rw-r-- 1 eric eric    75 Mar 20 17:03 ls.txt
// -rw-rw-r-- 1 eric eric 12780 Mar 21 18:29 main.c
// -rw-rw-r-- 1 eric eric    92 Mar 20 17:01 Makefile
// -rwxrwxr-x 1 eric eric 16896 Mar 20 22:03 mm
// drwxrwxr-x 5 eric eric  4096 Mar 11 16:21 online_academy
// -rw-r--r-- 1 eric eric    50 Mar 21 13:12 output.txt
// -rwxrwxr-x 1 eric eric 17856 Mar 21 19:06 shell
// -rw-rw-r-- 1 eric eric  9820 Mar 21 19:02 shell.c
// -rwxrwxr-x 1 eric eric 18200 Mar 20 17:13 simple
// -rw-rw-r-- 1 eric eric  8057 Mar 20 17:13 simple_shell.c
// -rw-rw---- 1 eric eric    16 Mar 21 18:43 sort.txt
// -rwxrwxr-x 1 eric eric 22024 Mar 21 17:18 test01
// -rw-rw-r-- 1 eric eric  9820 Mar 21 19:02 test01.c
// -rw-rw-r-- 1 eric eric  6997 Mar 20 21:04 test.c
// (END)
//* PRESS "Q" to back our shell
