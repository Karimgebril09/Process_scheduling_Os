#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "headers.h"
#include "MinHeap.h"
#include "RR_Queue.h"

struct msgBuf {
    long mType;
    struct processdata p;
};
//////////////////VARIABLES//////////////
int generatorFinsished = 0;
int finished = 0;
int algo;
int shmid;
int *remtimeadd;
int quantum;
FILE *Processfile;
FILE *f;
struct cpu Cpu_data;
int msgqid;
void *RR_Q;
int quatumTime;
heap *RQ=NULL;
 int prevtime = 0;
struct msgBuf message;
struct Process *RuningProcess = NULL;
////////////////PROTOTYPES/////////////
int initializeScheduler();
int getProcessFromGEN_ifexisted(int msgqid, int type, int q);
int RRimplementation();
void HPFimplementation();
void SRTNimplementation();
struct Process *createProcess(int pid);
int generateprocess(int runtime);
void initCpu();
void ScheduleLog(struct Process *process, enum states state, FILE *Pfile, int time);
void Scheduleperf();
void cleanResources(int signum);
void ProcessKillHandler(int signum);
void RR_cycle(struct RR_Queue *RR_queue);
void updateInfo();
//////////////////////////////////////////////////
//////////////////////////////////////////////////
void cleanResources(int signum) {
    destroyClk(true);
    exit(0);
    if (shmctl(shmid, IPC_RMID, NULL) == -1) {
        perror("Error deleting shared memory");
        exit(-1);
    }
    if (msgctl(msgqid, IPC_RMID, NULL) == -1) {
        perror("Error deleting message queue");
        exit(-1);
    }
}

int main(int argc, char *argv[]) {
    /// this is for remianing time shared memory////
    shmid = shmget(REMTIMEKEY, 4, IPC_CREAT | 0644);
    if ((long) shmid == -1) {
        perror("Error in creating shm!");
        exit(-1);
    }
    Processfile = fopen("ScheduleLog.txt", "w");
    f=fopen("wta.txt","w");
    quantum = atoi(argv[2]);
    algo = atoi(argv[1]);
    printf("%d", algo);
    if (algo == 1) {
        RR_Q = RR_create_Queue(quantum); //should be changed to void *
    } else if (algo == 2) {
        RQ = createHeap(100, 0);
    } else if (algo == 3) {
        RQ = createHeap(100, 1);
    }
    printf("hello fromm sched my quantum %d\n", quantum);
    msgqid = initializeScheduler();
    quatumTime=-1;

    remtimeadd = (int *) shmat(shmid, (void *) 0, 0);
    if ((long) remtimeadd == -1) {
        perror("Error in attaching the shm in Remaning time!");
        exit(-1);
    }

    initCpu();
    initClk();
    while (!finished) {
        if (prevtime == getClk())
            continue;
        printf("%d\n", getClk());
        updateInfo();
        while (getProcessFromGEN_ifexisted(msgqid, algo, quantum)) {}
        if (algo == 1 &&!finished) // RR
        {
            RRimplementation();
        } else if (algo == 2 &&!finished) // SRTN
        {
            SRTNimplementation();
        } else if(!finished) // HPF
        {
            HPFimplementation();
        }
        prevtime = getClk();
    }
    fclose(f);

    Scheduleperf(Cpu_data);
    fclose(Processfile);
    raise(SIGINT);
}

int initializeScheduler() {
    signal(SIGINT, cleanResources);
    signal(SIGUSR1, ProcessKillHandler);

    int msgq_id = msgget(Qkey, 0666 | IPC_CREAT);
    if (msgq_id == -1) {
        perror("ERROR IN MESSAGE QUEUE CREATION\n");
        exit(-1);
    }
    return msgq_id;
}

int getProcessFromGEN_ifexisted(int msgq_id, int type, int q) {
    int receive_val = msgrcv(msgq_id, &message, sizeof(message.p), 7, IPC_NOWAIT);
    if (message.p.id == -1) {
        generatorFinsished = 1;
        return 0;
    } else if (message.p.id == 0 || message.p.id == -2) {
        return 0;
    }
    int pid = generateprocess(message.p.runtime);
    struct Process *createdprocess = createProcess(pid);
    if (type == 1) {
        RR_enqueue(RR_Q, createdprocess);
    } else {
        insert(RQ, createdprocess);
    }
    return 1;
}

int generateprocess(int runtime) {
    int pid = fork();
    if (pid == 0) {
        char arg1[2];                                // Temporary strings to hold the converted integers
        snprintf(arg1, sizeof(arg1), "%d", runtime); // Convert input[0] to string
        char *args[] = {"./process", arg1, NULL};    // Pass the strings as arguments to sched program
        execv(args[0], args);
        perror("execv failed");
        exit(EXIT_FAILURE);
    } else if (pid == -1) {
        perror("error occurred while forking\n");
        return -1;
    }
    kill(pid, SIGSTOP);
    return pid;
}

struct Process *createProcess(int pid) {
    struct Process *p = malloc(sizeof(struct Process));
    p->arrival = message.p.arrival;
    p->runtime = message.p.runtime;
    p->priority = message.p.priority;
    p->id = message.p.id;
    p->remaining_time = message.p.runtime;
    p->waittime = 0;
    p->finishTime = -1;
    p->TA = -1;
    p->WTA = -1;
    p->state = START;
    p->ProcessId = pid;
    return p;
}

/////////////////////////////////////// KARIM MAHMOUD  ////////////////////////////
void ScheduleLog(struct Process *process, enum states state, FILE *Pfile, int time) {
    char *ProcessState;
    process->waittime = time - process->arrival - (process->runtime - process->remaining_time);
    switch (state) {
        case START:
            ProcessState = "started";
            break;
        case FINISH:
            ProcessState = "finished";
            break;
        case STOP:
            ProcessState = "stopped";
            break;
        case RESUMED:
            ProcessState = "resumed";
            break;
    }
    fprintf(Pfile, "At time %d process %d %s arr %d total %d remain %d wait %d", time, process->id, ProcessState,
            process->arrival, process->runtime, process->remaining_time,process->waittime);
    if (state == FINISH) {
        ////////////////// check if ++ working
        wait(NULL);
        Cpu_data.numberProcess++;
        int TA = time - process->arrival;
        float WTA = (float) TA / process->runtime;
        fprintf(Pfile, " TA %d WTA %.02f", TA, WTA);
        Cpu_data.totalWTA += WTA;
        Cpu_data.totalwaiting += process->waittime;
        fprintf(f,"%f\n",WTA);
    }
    fprintf(Pfile, "\n");
}

/////////          completed without std deviation       /////////////////////////
void Scheduleperf() {
    FILE *CPUfile = fopen("Scheduleperf.txt", "w");
    f = fopen("wta.txt", "r");
    int totaltime = Cpu_data.runtime + Cpu_data.waitime;
    float cpuUtilization = (float) Cpu_data.runtime / totaltime;
    float AvgWTA = (float) Cpu_data.totalWTA / Cpu_data.numberProcess;
    float AvgWaiting = (float) Cpu_data.totalwaiting / Cpu_data.numberProcess;
    // float StdDeviation=
    printf("runtime =%d and wait time =%d\n", Cpu_data.runtime, Cpu_data.waitime);
    // std dev //
    int count=0;
    float wta;
    float totalwta=0;
    while (fscanf(f, "%f", &wta) == 1) {
        count++;
        totalwta+=wta;
    }
    float mean=totalwta/count;
     printf("totalwta/count%f\n",mean);
    printf("%f",totalwta);
    totalwta=0;
    fseek(f, 0, SEEK_SET);
    while (fscanf(f, "%f", &wta) == 1) {
        totalwta+=pow((wta-mean),2);
    }
    printf("total wta %f",totalwta);
    fprintf(CPUfile, "CPUutilization= %.02f \n Avg WTA = %.02f \n Avg Waiting= %0.2f \nstd deviation=%.2f", cpuUtilization, AvgWTA, AvgWaiting,sqrt(totalwta));
    fclose(f);
    fclose(CPUfile);
}

void initCpu() {
    Cpu_data.numberProcess = 0;
    Cpu_data.runtime = 0;
    Cpu_data.totalwaiting = 0;
    Cpu_data.totalWTA = 0;
    Cpu_data.waitime = 0;
}

///////////////////////////////   SEIF  ///////////////////////////////////
void HPFimplementation() {
    if(RuningProcess==NULL && RQ->size!=0)
    {
        RuningProcess=extractMin(RQ);
        ScheduleLog(RuningProcess, START, Processfile, prevtime+1);
        (*remtimeadd)=RuningProcess->remaining_time;
        kill(RuningProcess->ProcessId,SIGCONT);
    }
}

void ProcessKillHandler(int signum) {
    if (algo == 2) {
        extractMin(RQ);
    }
    quatumTime=0;
    if(RR_Q!=NULL &&(algo==1 && RR_isEmpty(RR_Q) && generatorFinsished) || RQ!=NULL && ((algo==2 || algo==3)&& RQ->size==0 && generatorFinsished))
    {
        finished=1;
    }  
}

///////////////////////// AGAMI//////////////////////
void SRTNimplementation() {

    if (getfront(RQ)!=NULL && getfront(RQ) != RuningProcess) {
        if (RuningProcess != NULL) {
            RuningProcess->remaining_time = *remtimeadd;
            kill(RuningProcess->ProcessId, SIGSTOP);
            ScheduleLog(RuningProcess, STOP, Processfile,prevtime+1);//time shift if sent paramter was getclk should be reserved paramter
        }
        RuningProcess = getfront(RQ);
        *remtimeadd = RuningProcess->remaining_time;
        kill(RuningProcess->ProcessId, SIGCONT);
        if (RuningProcess->remaining_time == RuningProcess->runtime)
            ScheduleLog(RuningProcess, START, Processfile, prevtime+1);
        else {
            ScheduleLog(RuningProcess, RESUMED, Processfile, prevtime+1);
        }
    }
}
int RRimplementation() {
    if(RuningProcess==NULL && !RR_isEmpty(RR_Q))
    {
        quatumTime=quantum;
        RuningProcess = RR_front(RR_Q);
        RR_dequeue(RR_Q);
        if (RuningProcess->remaining_time == RuningProcess->runtime)
        {
            ScheduleLog(RuningProcess, START, Processfile, prevtime+1);// changed every getclk with prevtime+1 for more reliable code
        }
        else {
            ScheduleLog(RuningProcess, RESUMED, Processfile, prevtime+1);
        }
        *remtimeadd = RuningProcess->remaining_time;
        kill(RuningProcess->ProcessId,SIGCONT);
    }
    else if(quatumTime==0)
    {
        RR_enqueue(RR_Q, RuningProcess);
        ScheduleLog(RuningProcess, RuningProcess->state = STOP, Processfile, prevtime+1);
        kill(RuningProcess->ProcessId, SIGSTOP);
        quatumTime=quantum;
        RuningProcess = RR_front(RR_Q);
        RR_dequeue(RR_Q);
        if (RuningProcess->remaining_time == RuningProcess->runtime)
            ScheduleLog(RuningProcess, START, Processfile, prevtime+1);
        else {
            ScheduleLog(RuningProcess, RESUMED, Processfile, prevtime+1);
        }
        *remtimeadd = RuningProcess->remaining_time;
        kill(RuningProcess->ProcessId,SIGCONT);
    }
}
////////////////////////////////////////////////////
void updateInfo()
{
    /////        update cpu   ///////////
    if (RuningProcess != NULL)
    {
        Cpu_data.runtime++;
    }
    else
    {
        Cpu_data.waitime++;
    }
    //////          time updates  ///////////
    if(RuningProcess!=NULL)
    {
        (*remtimeadd)--;
        RuningProcess->remaining_time--;
        if(algo==1)
        {
            quatumTime--;
        }
        if(RuningProcess->remaining_time==0)
        {
            ScheduleLog(RuningProcess, RuningProcess->state = FINISH, Processfile, prevtime+1);
            RuningProcess=NULL;
        } 
    }
}

