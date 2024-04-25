#include <stdio.h>
#include <stdlib.h>
#include "headers.h"
#include "MinHeap.h"
#include"Queue.h"
void clearResources(int);

int* readalgo();
int create_schedular(int *algo);
void create_clock();
void readfromfiles(struct Queue*);
int sendlastmessage(int id);
int sendfinishclock(int id);

struct msgBuf 
{
    long mType;
    struct  processdata  p;
};

int main(int argc, char * argv[])
{
    signal(SIGINT, clearResources);
    // TODO Initialization
    struct Queue *AllProcesses = createQueue();

    // 1. Read the input files.
    readfromfiles(AllProcesses);
    // 2. Ask the user for the chosen scheduling algorithm and its parameters, if there are any.
    int *Scheduling_algo = readalgo();
    // 3. Initiate and create the scheduler and clock processes.
    int scheduler_pid=create_schedular(Scheduling_algo);
    create_clock();
    // 4. Use this function after creating the clock process to initialize clock
    initClk();
    // To get time use this
    //int x = getClk();
    //printf("current time is %d\n", x);
     
     //! Sending the process to the scheduler section
    int msgq_id = msgget(Qkey,0666|IPC_CREAT);
    if(msgq_id==-1)
    {
        perror("ERROR IN MESSAGE QUEUE CREATION\n");
        exit(-1);
    }
    struct msgBuf message;
    //! End Of Sending the process to the scheduler
    /////////////////////////////////////////////////////////
    // TODO Generation Main Loop
    // 5. Create a data structure for processes and provide it with its parameters.
    // 6. Send the information to the scheduler at the appropriate time.
    // 7. Clear clock resources
   ////////////////////////////////////////////////////////
   struct processdata *Process;
   int send_val; 
   int arrivalTime;
    while(!isEmpty(AllProcesses))
    {
        if(AllProcesses->front->process->arrival >getClk())
        {
            continue;
        }
        else
        {
            arrivalTime=AllProcesses->front->process->arrival;

        }
        while(!isEmpty(AllProcesses)&&AllProcesses->front->process->arrival==arrivalTime)
        {
            Process=dequeue(AllProcesses);
            message.p.id=Process->id;
            message.p.arrival=Process->arrival;
            message.p.priority=Process->priority;
            message.p.runtime=Process->runtime;
            message.mType=7;
            send_val=msgsnd(msgq_id,&message,sizeof(message.p),!IPC_NOWAIT);  ///////////////////////////    WARNING:can make errors (no wait)  /////////////
            free(Process);
        }
        if(!isEmpty(AllProcesses))
            sendfinishclock(msgq_id);
    }
    sendlastmessage(msgq_id);
   // kill(scheduler_pid,SIGUSR1);  //inform schedular that gen finished
    raise(SIGINT);
}

int* readalgo()
{
    printf("Select the required algo:\n");
    printf("RR:1\tSRTN:2\tHPF:3\n");
    
    int* input=malloc(sizeof(int)*2);
    scanf("%d", &input[0]);
    while (input[0] != 1 && input[0] != 2 && input[0] != 3)
    {
        printf("invalid input\n");
        scanf("%d", &input[0]);
    }
    if (input[0] == 1)
    {
        printf("please enter the quantum:\n");
        scanf("%d", &input[1]);
    }
    return input;
}

int create_schedular(int *input) {
    int pid = fork();
    if (pid == 0) {
        //printf("before sending %d %d\n", input[0], input[1]);
        char arg1[2], arg2[3]; // Temporary strings to hold the converted integers
        snprintf(arg1, sizeof(arg1), "%d", input[0]); // Convert input[0] to string
        snprintf(arg2, sizeof(arg2), "%d", input[1]); // Convert input[1] to string
        char *args[] = {"./sched", arg1, arg2, NULL}; // Pass the strings as arguments to sched program
        execv(args[0], args);
        perror("execv failed");
        exit(EXIT_FAILURE);
    } else if (pid == -1) {
        perror("error occurred while forking\n");
        free(input); // Free the dynamically allocated input memory in case of failure
        return -1; // Return an error code since forking failed
    }
    free(input); // Free the dynamically allocated input memory in the parent process
    return pid;
}
void create_clock()
{
    int pid = fork();
    if (pid == 0)
    {
        char* const args[] = {"./clock", NULL};
        execv("./clock", args);
        perror("execv failed");
        exit(EXIT_FAILURE);
    }
    else if (pid == -1)
    {
        perror("error occurred while forking\n");
    }
}

void readfromfiles(struct Queue * AllProcesses)
{
    FILE *file = fopen("processes.txt", "r"); // Replace with your file path

    if (file == NULL)
    {
        printf("Error opening the file.\n");
        return;
    }

    // Read and ignore the first line to ignore it
    char buffer[30];
    fgets(buffer, sizeof(buffer), file);
    struct processdata ptr;
    while (fscanf(file, "%d\t%d\t%d\t%d", &ptr.id, &ptr.arrival, &ptr.runtime, &ptr.priority) == 4) {
        // Dynamically allocate memory for each process
        struct processdata *newProcess = (struct processdata *)malloc(sizeof(struct processdata));
        // Copy data to the dynamically allocated process object
        newProcess->id = ptr.id;
        newProcess->arrival = ptr.arrival;
        newProcess->runtime = ptr.runtime;
        newProcess->priority = ptr.priority;

        // Enqueue the dynamically allocated process object into the queue
        enqueue(AllProcesses, newProcess);

        printf("ID: %d, Arrival: %d, Runtime: %d, Priority: %d\n", ptr.id, ptr.arrival, ptr.runtime, ptr.priority);
    }

    fclose(file); // Close the file when done
}


void clearResources(int signum)
{
    //TODO Clears all resources in case of interruption
    wait(NULL);
    wait(NULL);
    int msgq_id = msgget(Qkey,0666|IPC_CREAT);
    msgctl( msgq_id, IPC_RMID, (struct msqid_ds *)0);
    exit(0);
}

int sendlastmessage(int id)
{
    struct msgBuf message;
    message.p.id=-1;
    message.mType=7;
    msgsnd(id,&message,sizeof(message.p),!IPC_NOWAIT);
}

int sendfinishclock(int id)
{
    struct msgBuf message;
    message.p.id=-2;
    message.mType=7;
    msgsnd(id,&message,sizeof(message.p),!IPC_NOWAIT);
}