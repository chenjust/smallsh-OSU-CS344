/********************************
 *Justin Chen
 *compiler program for smallsh.c
 *contains the function definitions
********************************/
#include "smallsh.h"

int foregroundMode = 0;

/*****************************************
 * pid
 * Handles the special character '$'
*****************************************/
int pid(char* input){
    int i = 0;
    
    while(i < strlen(input)){
        if(input[i] == '$' && input[i + 1] == '$')
            return i;
    
        i++;
    }

    return -1;
}

/********************************
 *space
 *Looks for the space character
********************************/
int space(char* string, int num){
    int i;
    
    for(i = num; i < strlen(string) - 1; i++){
        if(string[i] == ' ')
            return i;
    }

    return -1;
}

/*********************************************
 * inputOutput
 * Handles the special characters '>' and '<'
 * '>' for output file
 * '<' for input file
*********************************************/
int inputOutput(int num, char* input){
    int i = 0;
    char bracket;

    if(num == 0)
        bracket = '>';

    else
        bracket = '<';
    
    while(i < strlen(input) - 1){
        if (input[i] == bracket) 
            return i;
        
        i++;
    }
  
    return -1;
}

/**********************************
 * inputLength
 * gets the length of the user input
 * in the command line
**********************************/
int inputLength(char* input){
    int num;
    int i;
    
    for(i = 0; i < strlen(input); i++){
        if((int)input[i] == 64)
            num++;
    }
    
    num += 2;

    return num;
}

/******************************
 *signalControlC
 *Handles the situation when
 *ctrl-c is entered in the shell.
******************************/
void signalControlC(int sig){
    printf("terminated by signal %d\n", WTERMSIG(sig));
}

/********************************
 *signalControlZ
 *Handles the situation when
 *ctrl-z is entered in the shell
********************************/
void signalControlZ(int sig){
    if(foregroundMode == 0){
        printf("Entering foreground-only mode (& is now ignored)\n");
        foregroundMode = 1; // Enables foreground mode.
    }

    else{
        printf("Exiting foreground-only mode\n");
        foregroundMode = 0; //Disables foreground mode.
    }

    fflush(stdout);
}

/************************************************
 * argument
 * creates an array from the user input
 * to be use as an arument on the command line
************************************************/
void argument(char** array, char* string){
    int num;
    int current;
    int previous;
    int max;

    current = space(string, 0);
    max = strlen(string);
    
    while(previous != -1) {
        int i;
        int result;
        char* str;

        if(current != -1)
            result = current;
        
        else
            result = max;
        
        if(previous != 0) 
            previous++;
        
        str = (char *) malloc((result - previous) * sizeof(char));

        for(i = 0; i < result - previous; i++){
            str[i] = string[previous + i];
        }
    
        array[num] = str;

        previous = current;

        current = space(string, current + 1);

        num++;
    }
}

/*******************************************
 * files
 * Handles the input and output of files
*******************************************/
void files(char* input, int in, int out){
    int inputEnd = INT_MAX;

    if(in != -1){
        int result;
        int fileDirectory;
        char* fileName;

        inputEnd = in;
        in += 2;

        if(space(input, in) != -1)
            result = space(input, in);

        else
            result = strlen(input) - 1;

        fileName = (char*) malloc((result - in) * sizeof(char));
        memcpy(fileName, &input[in], (result - in));

        fileDirectory = open(fileName, O_RDONLY);
        if (fileDirectory < 0) {
            fprintf(stderr, "cannot open %s for input\n", fileName);
            exit(1);
        }

        if(dup2(fileDirectory, STDIN_FILENO) == -1) {
        printf("Error");
        }

        free(fileName);
    }

    if(out != -1){
        int result;
        int fileDirectory;
        char* fileName;

        if(out < inputEnd)
            inputEnd = out;

        else
            inputEnd = inputEnd;

        out += 2;

        if(space(input, out) != -1)
            result = space(input, out);

        else
            result = strlen(input) - 1;

        fileName = (char*) malloc((result - out) * sizeof(char));
        memcpy(fileName, &input[out], (result - out));

        fileDirectory = open(fileName, O_WRONLY | O_CREAT | O_TRUNC, 0644);
        if(fileDirectory < 0){
            fprintf(stderr, "cannot open %s for output\n", fileName);
            exit(1);
        }
        
        if(dup2(fileDirectory, STDOUT_FILENO) == -1) {
            printf("Error");
        }

        free(fileName);
    }

    int num;
    char* command;

    if(inputEnd < INT_MAX)
        inputEnd = inputEnd - 1;
    
    else
        inputEnd = strlen(input) - 1;

    command = (char*) malloc(inputEnd * sizeof(char));
    memcpy(command, input, inputEnd);

    num = inputLength(command);
    
    char* user[num];

    argument(user, command);
    user[num-1] = NULL;

    if(execvp(user[0], user) == -1){
        int result;
        char* fileName;

        if(space(input, out) != -1)
            result = space(input, out);

        else
            result = strlen(input) - 1;

        fileName = (char*) malloc((result - out) * sizeof(char));
        memcpy(fileName, &input[out], (result - out));

        printf("%s %s\n", fileName, strerror(errno));
        
        free(fileName);

        exit(1);
    }
  
    exit(1);
}
