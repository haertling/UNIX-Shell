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
    
    piece = strtok(line, " \n\t");//tokenize the user input
    while(piece != NULL){
        inputs[i] = piece;// assigning tokens to array positions
        //printf(piece,"\n");// checking that array is being filled correctly
        i++;//increment array on loop
        if(i >=buffersize){
            buffersize += buffersize;
            inputs = realloc(inputs, buffersize *sizeof(char*));// for big user inputs
        }
        piece = strtok(NULL, " \n\t");//stops looping when everything has been tokenized
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
    if(strncmp("\n", cmd[0], 2) ==0){
    return 1;
    }
    return runCopy(cmd);// run fork and exec process
}

int runCopy(char **cmd){    
    int buffersize = 64;
    int i = 0;
    char **temp = malloc(sizeof(char*) * buffersize);
    char *piece;
    
    piece = strtok(myPath, ":");//tokenize the user input
    while(piece != NULL){
        temp[i] = piece;// assigning tokens to array positions
        //printf(piece,"\n");// checking that array is being filled correctly
        i++;//increment array on loop
        if(i >=buffersize){
            buffersize += buffersize;
            temp = realloc(temp, buffersize *sizeof(char*));// for big user inputs
        }
        piece = strtok(NULL, ":");//stops looping when everything has been tokenized
    }
    temp[i] = NULL;// places an eof on array
       
   
    i = 0;
    while(temp[i] != NULL){// errors in here
        char* temp2 = NULL;
        int tempLen = strlen(temp[i]);
        int cmdLen = strlen(cmd[0]);
        temp2 = malloc(tempLen+1+cmdLen);
        strcpy(temp2, temp[i]);
        strcat(temp2, "/");
        strcat(temp2, cmd[0]);
        printf(temp2,"\n");
        //temp2[strlen(temp2)-1]=0;
        if(access(temp2, X_OK) == 0){//what does this function return
            int a = fork();
                if(a < 0){ //failed fork
            printf("failed fork\n");
                    write(STDERR_FILENO, myErrorMessage, strlen(myErrorMessage));//error
                    return 1;//break out of this function and continue looping
                }
                if(a == 0){ //child process
                    if(execv(temp2,cmd) == -1){
                printf("failed child\n");
                        write(STDERR_FILENO, myErrorMessage, strlen(myErrorMessage));//error
                        return 0;//break out of this function and continue looping
                    }
                }
                else{ //parent process waiting for child
                    int a_wait = wait(NULL);
                    return 1;//break out of this function and continue looping
                }
        }
    i++;    
    }
    printf("failed runCopy \n");
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
    char* c;
    strcpy(c, cmd[1]);
    //c[strlen(c)-1]=0;
        if(chdir(c)!= -1){ // sys call to change diectory for our shell
            //printf("success\n");
            return 1;
        }
        
    }
    //printf("failed chdir\n");
    write(STDERR_FILENO, myErrorMessage, strlen(myErrorMessage));//error
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
    
    return 1;
   
} // end of path function

char** redirect(char **cmd){
    char** tempLeft;
    char* tempRight; // left side of string before ">" and right side of string after ">"
    char* found = ">";
    int length = 0;
    int i;
    while(cmd[length] != NULL){
        length++;
    }
    
    for(i = 0; i < length + 1; i++){
        if(strncmp(cmd[i], found, 1) == 0){
            printf("Redirect symbol at location %d/n", i);
            break; // break out of for loop with i set to location of ">" in cmd[]
        }
        else{
            return cmd; //no redirect
        }
    }
    
    if((cmd[i+1] == NULL) || (cmd[i+2] != NULL)){
        // if (location of ">") + 1 is NULL or (location of ">") + 2 is NOT NULL, throw error
        write(STDERR_FILENO, myErrorMessage, strlen(myErrorMessage));//error
        exit(0);//break out of this function and continue looping
    }
    
    int j = 0; // j used to iterate from 0 to (location of ">") - 1
    while(j < i){
        strcat(tempLeft[0], cmd[j]); // concatenate tempLeft with left side inputs and a space
        strcat(tempLeft[0], " ");
        j++;
    }
    
    tempRight = cmd[i+1]; // the right side of ">" should be one command indicating desired file location
    
    
    // Now we have separated the left and right sides of input
    // left is the desired command
    // right is the file location we wish to add it to
    
    // Check first if file location already exists in directory
    if(access(tempRight, F_OK) != -1){
        // File exists already, truncate then overwrite chosen file
        FILE *oldFile = fopen(tempRight, "w");
        if(oldFile == NULL){ // If file failed to open, exit gracefully
            puts("Failed to open file");
            return cmd;
        }
        else{
            fputs(tempLeft[0], oldFile); // put contents on templeft in file
            fclose(oldFile); // close file
    }
    }
    else{
        // File does not exist already, make new file with name tempRight
        FILE *newFile = fopen(tempRight, "w");
        if(newFile == NULL){ // If file failed to open, exit gracefully
            puts("Failed to open file");
            return tempLeft;
        }
        else{
            fputs(tempLeft[0], newFile); // put contents on templeft in file
            fclose(newFile); // close file
        }
    }
    
}
