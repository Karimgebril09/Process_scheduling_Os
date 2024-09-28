#include "headers.h"
/* Modify this file as needed*/
int remainingtime;
int main(int agrc, char * argv[])
{
   // printf("hello from a process with pid %d\n",getpid());
    if(agrc < 2)
    {
        perror("ERROR IN CREATING THE PROCESS \n");
        exit(-1);
    }
   // remainingtime = atoi(argv[1]); //! It will be passed as a paramater while creating the process
    //TODO it needs to get the remaining time from somewhere
    //remainingtime = ??;
    int shmid = shmget(REMTIMEKEY, 4, IPC_CREAT | 0644);
    if ((long)shmid == -1)
    {
        perror("Error in creating shm!");
        exit(-1);
    }
    int * remtimeadd = (int *) shmat(shmid, (void *)0, 0);
    if ((long)remtimeadd == -1)
    {
        perror("Error in attaching the shm in Remaning time!");
        exit(-1);
    }
    remainingtime=*remtimeadd;
    while (remainingtime > 0)
    {
        remainingtime=*remtimeadd;
    }
    kill(getppid(),SIGUSR1);
    return 0;
}
