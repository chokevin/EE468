/*
 *  This is a simple shell program from
 *  rik0.altervista.org/snippetss/csimpleshell.html
 *  It's been modified a bit and comments were added.
 *
 *  It doesn't allow misdirection, e.g., <, >, >>, or |
 */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#define BUFFER_SIZE 80
#define ARR_SIZE 80

// #define DEBUG 1  /* In case you want debug messages */

void parse_args(char *buffer, char** args,
                size_t args_size, size_t *nargs)
{

    char *buf_args[args_size]; /* You need C99.  Note that args_size
                                  is normally a constant. */
    char **cp;  /* This is used as a pointer into the string array */
    char *wbuf;  /* String variable that has the command line */
    size_t i, j;

    wbuf=buffer;
    buf_args[0]=buffer;
    args[0] =buffer;

    for(cp=buf_args; (*cp=strsep(&wbuf, " \n\t")) != NULL ;){
        if ((*cp != '\0') && (++cp >= &buf_args[args_size]))
            break;
    }

/*
 * Copy 'buf_args' into 'args'
 */
    for (j=i=0; buf_args[i]!=NULL; i++){
        if(strlen(buf_args[i])>0)  /* Store only non-empty tokens */
            args[j++]=buf_args[i];
    }

    *nargs=j;
    args[j]=NULL;
}

void getCurrentCommand(char* currCommand[], char* args[], int i) {
    int count = 0;
    int currCommandArgs = 0;
    while( i >= 0 ) {
        if (i == 0) {
            while (args[count] != NULL && strcmp(args[count], "|") != 0) {
                currCommand[currCommandArgs++] = args[count];
                count++;
            }
            i--;
        } else {
            while (args[count] != NULL && strcmp(args[count], "|") != 0) {
                count++;
            }
            if (strcmp(args[count], "|") == 0) {
                count++;
            }
            i--;
        }
    }
}

int main(int argc, char *argv[], char *envp[]){
    char buffer[BUFFER_SIZE];
    char *args[ARR_SIZE];
    char *currCommand[ARR_SIZE];
    int status;
    int count;

    size_t nargs;
    pid_t pid;

    while(1) {
        count = 0;
        printf("ee468>> "); /* Prompt */
        fgets(buffer, BUFFER_SIZE, stdin); /* Read in command line */
        /* Parse the command line into args */
        parse_args(buffer, args, ARR_SIZE, &nargs);

        if (nargs == 0) {
            continue; /* Nothing entered so prompt again */
        }
        if (!strcmp(args[0], "exit")) {
            exit(0);
        }
        int currCmdCount = 0;
        while (args[count] != NULL) {
            while (args[count] != NULL && strcmp(args[count], "|") != 0) {
                count++;
            }
            count++;
            currCmdCount++;
        }

        int numOfPipes = currCmdCount - 1;
        count = 0;

        /* Initializing the pipes based on how many pipes are in the command */
        int pipes[2*numOfPipes];

        while( count < 2*numOfPipes ) {
            pipe(pipes + count);
            count += 2;
        }

        int i = 0;
        int j = 0;
        for (i = 0; i < currCmdCount; i++) {
            getCurrentCommand(currCommand, args, i);
            pid = fork();

            if (pid == 0) {

                /* If not last command */
                if (i < currCmdCount - 1) {
                    if (dup2(pipes[j + 1], 1) < 0) {

                    }
                }

                if (j != 0) {
                    if (dup2(pipes[j - 2], 0) < 0) {

                    }
                }

                int val;
                for (val = 0; val < 2 * numOfPipes; val++) {
                    close(pipes[val]);
                }


                if (execvp(currCommand[0], currCommand) < 0) {

                }
            }
            j += 2;
            memset(&currCommand[0], 0, sizeof(currCommand));
        }

        for(i = 0 ; i < 2 * numOfPipes; i++) {
            close(pipes[i]);
        }
        for (i = 0; i < 3; i++)
            wait(&status);
        memset(&args[0], 0, sizeof(args));
    }
        return 0;
}
