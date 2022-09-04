/*****************************************
 * Justin Chen
 * main program file
 * executes the main program, a smallsh
*****************************************/
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <fcntl.h>
#include <signal.h>
#include <unistd.h>
#include <errno.h>
#include <limits.h>
#include <sys/wait.h>
#include "smallsh.h"

#define inputSize 2049

int foreground = 1;

int main(){
    // Variable declaration
    int exitStatus = 0;
    int childExitStatus = -5;
    char* input;
    pid_t child;
    size_t inputLength;

    inputLength = inputSize;

    input = (char *) malloc(inputSize * sizeof(char));

    signal(SIGINT, signalControlC); // Handles the signal ctrl-c
    signal(SIGTSTP, signalControlZ); // Handles the signal ctrl-z
    
    while(1){
        int pID;

        // User input
        printf(": ");
        fflush(stdout);
        fgets(input, inputSize, stdin);

        pID = pid(input);

        if(pID != -1){ //if $$ is read convert it to the process id.
            char amp[pID];

            amp[pID] = '\0';

            memcpy(&amp, input, pID);

            sprintf(input, "%s%d\n", &amp, (int) getpid());
        }

        if(input[0] == '#' || strlen(input) == 1) // Checks if '#' is typed in
            exitStatus = 0;

        // Checks for built in commands
        if(strncmp(input, "exit", 4) == 0)
            return 0;
        
        else if(strncmp(input, "cd", 2) == 0){
            char token = '\n';

            strtok(&input[3], &token);

            if(strlen(&input[3]) == 0) 
                chdir(getenv("HOME"));
        
            else{
                if(chdir(&input[3]) == -1){
                    printf("Error! %s\n", strerror(errno));
                    exitStatus = -1;
                }
            }
        }

        else{
            int background = 0;

            if(input[strlen(input) - 2] == '&'){
                if(!foreground)
                    background = 1;

                input[strlen(input) - 2] = '\0';
            }

            child = fork();

            if(child == -1){
                perror("Hull Breach");
                exitStatus = -1;
            }
        
            else if(child == 0){
                int in;
                int out;

                if(background){
                    signal(SIGINT, SIG_IGN);
                    signal(SIGTSTP, SIG_IGN);
                }

                in = inputOutput(1, input);
                out = inputOutput(0, input);
                
                files(input, in, out);
            }

            else{
                if(background){
                    printf("background pid is %i\n", (int) child);
                    waitpid(child, &childExitStatus, WNOHANG);
                }

                else
                    waitpid(child, &exitStatus, 0);
            }
        }

        // Calls the exit status
        waitpid(-1, &childExitStatus, WNOHANG);
        if(childExitStatus != -5 && WIFEXITED(childExitStatus)){
            printf("exit value %d\n", WEXITSTATUS(childExitStatus));
            childExitStatus = -5;
        }

        if(childExitStatus != -5 && WIFSIGNALED(childExitStatus)){
            printf("terminated by signal %d\n", WTERMSIG(childExitStatus));
            childExitStatus = -5;
        }
    }

    free(input);
    
    return 0;
}
