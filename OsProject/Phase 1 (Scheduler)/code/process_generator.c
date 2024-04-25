#include "headers.h"

void clearResources(int);

int readalgo();
void create_schedular(int algo);
void create_clock();

int main(int argc, char * argv[])
{
    signal(SIGINT, clearResources);
    // TODO Initialization
    // 1. Read the input files.
    
    // 2. Ask the user for the chosen scheduling algorithm and its parameters, if there are any.
    int Scheduling_algo=readalgo();
    // 3. Initiate and create the scheduler and clock processes.
    create_schedular(Scheduling_algo);
    create_clock();
    // 4. Use this function after creating the clock process to initialize clock
    initClk();
    // To get time use this
    int x = getClk();
    printf("current time is %d\n", x);
    // TODO Generation Main Loop
    // 5. Create a data structure for processes and provide it with its parameters.
    // 6. Send the information to the scheduler at the appropriate time.
    // 7. Clear clock resources
    destroyClk(true);
}

int readalgo()
{
    printf("Selecting the required algo:\n");
    printf("RR:1\t SRTN:2\t HPF:3\n");
    int input;
    scanf("%d",input);
    while(input!=1 && input!=2 && input!=3 )
    {
        printf("invlid input\n");
        scanf("%d",input);
    }
    if(input==1)
    {
        key_t key_id3;
        key_id3=ftok("empty.txt",'a');
        shmid = shmget(key_id3,8, IPC_CREAT | 0666);
        if(shmid == -1)
        {
            perror("Error in create");
            exit(-1);
        }
        void *quantum = shmat(shmid, (void *)0, 0);
        printf("please enter the quantum:\n");
        scanf("%d",(int*)quantum);
    }
    return input;
}
create_schedular(int algo)
{
    int pid=fork();
    if(pid==0)
    {
        execv("./schedular",algo,NULL);
    }
    else if(pid==-1)
    {
        perror("error occured while forking\n");
    }
}
create_clock();
{
    int pid=fork();
    if(pid==0)
    {
        execv("./clock",NULL);
    }
    else if(pid==-1)
    {
        perror("error occured while forking\n");
    }
}

void clearResources(int signum)
{
    //TODO Clears all resources in case of interruption
}
