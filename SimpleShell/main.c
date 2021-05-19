#include <stdio.h>
#include <stdlib.h>
#include<string.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/wait.h>
#include<readline/readline.h>

//for logging purpose
char* cmd=NULL;
//function to log termination into log file when a child signals its termination
void logAction()
{
    //wait(NULL);
    FILE* fptr;
    fptr = fopen("logfile.txt", "a");
    strcat(cmd," child process terminated\n");
    fputs(cmd,fptr);
    fclose(fptr);

}


int main()
{
    char c;
    signal (SIGCHLD, logAction);  //when child terminates a signal is sent and logAction is executed
    while(1)
    {

        int waitFlag=1;            // flag to decide whether to wait for child termination or not
        printf("Enter command ");    //commandLine
        char* line=NULL;
        scanf("%m[^\n]", &line);    //scan the line
        scanf("%c",&c);             //to avoid scannig issues

        const char **args;          //arguments of command are stored here
        int argsSize=0;

        char delim[] = " ";        //for parsing
        char *ptr = strtok(line, delim);    //splitting the input
        cmd=ptr;                            //for logging purpose

        //this loop parses the line to get arguments of command

        while(ptr != NULL)
        {

            int s=strlen(ptr);
            args = realloc(args, s* sizeof (char *));  //allocate space for current string
            args[argsSize]=ptr;
            ptr = strtok(NULL, delim);
            argsSize++;
        }

        //allocate space to a void pointer to null terminate the arguments

        args=realloc(args,4*sizeof(void*));
        args[argsSize]=(void*)NULL;

        //to exit the shell
        if(!(strcmp(args[0],"exit")))
        {
           kill(0, SIGKILL);

        }

        //check if we should wait or not for child process termination
        if (args[argsSize-1]!=NULL)
            waitFlag= strcmp(args[argsSize-1],"&");

        //forking a child process
        pid_t childPid;
        childPid = fork();


        // child process
        if(childPid == 0)
        {
            int val=execvp(args[0],args);   //execute the command
            if(val==-1) {printf("No such command\n");
             }

        }
        //failed to create process

        else if(childPid < 0)
        {
            printf("failed to create child process\n");
        }

        //parent should wait on child process
        else if(waitFlag!=0)
        {

            int status;

            pid_t id= waitpid(childPid, &status,WUNTRACED);

            if (status==0)
            {
                printf("Child process terminated\n");
            }
        }
    }

    return 0;
}
