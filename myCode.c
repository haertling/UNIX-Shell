//header files
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
//function declaration
char *readLine(void);
char **parseLine(char *line);
int myrun(char **cmd);
int runCopy(char **cmd);
int cd(char **cmd);
int myexit(char **cmd);
int path(char **cmd);
//global variable
char myErrorMessage[33] = "There has been an oopsy whoopsy\n";
char *myPath = "/bin";

int main(int argc, char *argv[]){
    char *line;
    char **cmd;
    int check = 1;
    
    while( check == 1 ){
        printf("sjsush> ");//Our shell :)
        line = readLine();//get input
        cmd = parseLine(line);//parse that input
        check = myrun(cmd);//run that input
    }
    free(myPath);
    exit(0);    
}

char *readLine(void){//example of getline from prof
    char *line = NULL;
    ssize_t linesize = 0;    
    getline(&line, &linesize, stdin);
    return line;
}

char **parseLine(char *line){
    int buffersize = 64;
    int i =0;
    char **inputs = malloc(sizeof(char*) * buffersize);
    char *piece;
    
    piece = strtok(line, " :");//tokenize the user input
    while(piece != NULL){
        inputs[i] = piece;// assigning tokens to array positions
        printf(piece, "/n");// checking that array is being filled correctly
        i++;//increment array on loop
        if(i >=buffersize){
            buffersize += buffersize;
            inputs = realloc(inputs, buffersize *sizeof(char*));// for big user inputs
        }
        piece = strtok(NULL, " ");//stops looping when everything has been tokenized
    }
    inputs[i] = NULL;// places an eof on array
    return inputs;    
}

int myrun(char **cmd){
    if(cmd[0] == NULL){ //no cmd
        return 1;//break out of this function and continue looping
    }
    if(strncmp("exit", cmd[0], 4) == 0){ // search built ins
        return(myexit(cmd));// run built in exit function
    }
    if(strncmp("path", cmd[0], 4) == 0){ // search built ins
        return(path(cmd));// run built in path function
    }
    if(strncmp("cd", cmd[0], 2) == 0){ // search built ins
        return(cd(cmd));// run built in cd function
    }
    return runCopy(cmd);// run fork and exec process
}

int runCopy(char **cmd){
    char **temp = malloc(sizeof(char*)*64);
    int i = 0;
    printf("hello");
    temp = parseLine(myPath);//parse that input
    printf(temp[0],"\n");
    while(temp[i] != NULL){// errors in here
    strcat(temp[i], "/");
    strcat(temp[i], cmd[0]);
    printf(temp[i],"\n");
    if(access(temp[i], X_OK) == 0){//what does tis function return
        int a = fork();
            if(a < 0){ //failed fork
             write(STDERR_FILENO, myErrorMessage, strlen(myErrorMessage));//error
             return 1;//break out of this function and continue looping
            }
            if(a == 0){ //child process
             if(execv(temp[i],cmd) == -1){
                  write(STDERR_FILENO, myErrorMessage, strlen(myErrorMessage));//error
                  return 0;//break out of this function and continue looping
             }
            }
            else{ //parent process waiting for child
             int a_wait = wait(NULL);
             return 1;//break out of this function and continue looping
            }
            
    temp[i] = strtok(NULL, " ");//stops looping when everything has been tokenized
    i++;
    }
    }
    write(STDERR_FILENO, myErrorMessage, strlen(myErrorMessage));//error
    return 1;
}

int cd(char **cmd){
    if(cmd[1] == NULL){ //cd entered on its own is an error for our shell
        write(STDERR_FILENO, myErrorMessage, strlen(myErrorMessage));//error
        return 1;//break out of this function and continue looping
    }
    else{
        if(cmd[2] != NULL){ // more than one command is an error in our shell
            write(STDERR_FILENO, myErrorMessage, strlen(myErrorMessage));//error
            return 1;//break out of this function and continue looping
        }
        chdir(cmd[1]); // sys call to change diectory for our shell
    }
    return 1;//break out of this function and continue looping
}

int myexit(char **cmd){
    if(cmd[1] != NULL){// more commands after exit
        write(STDERR_FILENO, myErrorMessage, strlen(myErrorMessage));//error
        return 1;//break out of this function and continue looping
    }
    return 0;
}

int path(char **cmd){
    int i = 1;
    char* temp = malloc(sizeof(cmd) + 1);
    while(cmd[i] != NULL){
        strcat(temp, cmd[i]); // add current command to temp char array
        if(cmd[i+1] == NULL){
            // next cmd location is EOF, break from while loop
            // this avoids printing unncecesarry colon
            break;
        }
        strcat(temp, ":"); // separate commands by a colon in temp char array        
        i++; // increment i
    } // end of while
    myPath = temp;
    printf(temp, "/n");
    //free(temp); // free allocated memory
    printf(myPath, "/n");
    
    
    return 1;
   
} // end of path function







