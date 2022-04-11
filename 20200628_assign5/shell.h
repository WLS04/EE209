// 20200628 Jinwoogyeol
// Assignment 5
// shell.h
#define _GNU_SOURCE
#define _DEFAULT_SOURCE
#include "dynarray.h"
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <unistd.h>
#include <sys/wait.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
/*--------------------------------------------------------------------*/
/* make redirection */
/* use open and close -> create file or read file */
void Redirection(char **argument, char *in, char *out)
{
    assert(argument);
    if(in && out)
    {
        fflush(NULL);
        int pid = fork();
        if (pid == 0)
        {
            int fdin = open(in, O_RDONLY);
            dup2(fdin, 0);
            close(fdin);
            int fdout = open(out, O_CREAT|O_TRUNC|O_WRONLY, 0600);
            dup2(fdout, 1);
            close(fdout);
            fflush(NULL);
            execvp(argument[0], argument);
            fprintf(stderr, "%s: No such file or directory\n", 
                                argument[0]);
                fflush(stdout);
            _exit(EXIT_FAILURE);
        }
        else
        {
            wait(NULL);
        }
    }
    else if(in)
    {
        fflush(NULL);
        int pid = fork();
        if(pid == 0)
        {
            int fdin = open(in, O_RDONLY);
            dup2(fdin, 0);
            close(fdin);
            fflush(NULL);
            execvp(argument[0], argument);
            fprintf(stderr, "%s: No such file or directory\n", 
                                argument[0]);
                fflush(stdout);
                _exit(EXIT_FAILURE);
        }
        else
        {
            wait(NULL);
        }
    }
    else if(out)
    {
        fflush(NULL);
        int pid = fork();
        if(pid == 0)
        {
            int fdout = open(out, O_CREAT|O_TRUNC|O_WRONLY, 0600);
            dup2(fdout, 1);
            close(fdout);
            fflush(NULL);
            execvp(argument[0], argument);
            fprintf(stderr, "%s: No such file or directory\n", 
                                argument[0]);
                fflush(stdout);
                _exit(EXIT_FAILURE);
        }
        else
        {
            wait(NULL);
        }
    }
}

/*--------------------------------------------------------------------*/
/* excute command */
/* if setenv, unsetenv, cd -> use c function */
/* if other command -> use execvp */
int execute(char **argument, char *input, char *output)
{
    if (argument[0])
    {
        if(strcmp(argument[0], "setenv")==0)
        {
            if(argument[1]==NULL)
            {
    fprintf(stderr, "./ish: setenv takes one or two parameters\n"); 
            }
            else if((argument[1]!=NULL)&&(argument[2]==NULL))
            {
                setenv(argument[1], "", 1);
            }
            else if(argument[3])
            {
    fprintf(stderr, "./ish: setenv takes one or two parameters\n");
            }
            else
            {
                setenv(argument[1], argument[2], 1);
            }
        }
        else if(strcmp(argument[0], "unsetenv")==0)
        {
            if(argument[2])
            {
    fprintf(stderr, "./ish: unsetenv takes one parameter\n");
            }
            else if(argument[1]==NULL)
            {
    fprintf(stderr, "./ish: unsetenv takes one parameter\n");
            }
            else
            {
                unsetenv(argument[1]);
            }
        }
        else if (strcmp(argument[0], "cd")==0)
        {
            if(argument[2])
            {
    fprintf(stderr, "./ish: cd takes one parameter\n");
            }
            else if (argument[1]==NULL)
            {
                if(chdir(getenv("HOME"))!=0)
                {
            fprintf(stderr, "./ish: No such file or directory\n");
                }
            }
            else if(chdir(argument[1])!=0)
                fprintf(stderr, "./ish: No such file or directory\n");
        }
        else
        {
            if(input||output)
            {
                Redirection(argument, input, output);
                return 1;
            }
            fflush(NULL);
            int pid = fork();
            if (pid!=0)
            {
                wait(NULL);
            }
            else 
            {
                execvp(argument[0], argument);
                fprintf(stderr, "%s: No such file or directory\n", 
                                argument[0]);
                fflush(stdout);
                _exit(EXIT_FAILURE);
            }
        }
    }
    return 1;
}