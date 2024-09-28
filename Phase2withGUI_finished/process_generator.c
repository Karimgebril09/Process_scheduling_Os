#include <stdio.h>
#include <stdlib.h>
#include "headers.h"
#include "MinHeap.h"
#include"Queue.h"
#include <gtk/gtk.h>

void clearResources(int);
void on_button1_clicked(GtkButton *button, gpointer user_data);
void on_button2_clicked(GtkButton *button, gpointer user_data);
void on_button3_clicked(GtkButton *button, gpointer user_data);
void on_close_button_clicked(GtkButton *button, gpointer data);
int* readalgo(int state);
int create_schedular();
void create_clock();
void readfromfiles(struct Queue*);
int sendlastmessage(int id);
int sendfinishclock(int id);

struct msgBuf 
{
    long mType;
    struct  processdata  p;
};
struct Callback_data{
    GtkWidget *window;
    GtkWidget *entry;
    int state;
}; 
int algo=-1 , quantum=-1;
int Scheduling_algo[2];
char file_name[256];
int main(int argc, char * argv[])
{
    signal(SIGINT, clearResources);
    // TODO Initialization
    struct Queue *AllProcesses = createQueue();
    // 1. Ask the user for the chosen scheduling algorithm and its parameters, if there are any.
    while (algo <=0 || (algo==1 && quantum<=0))
    {
        printf("please choose an algoritm \n");
        readalgo(0);
    }
    // 2. Read the input files.
    readfromfiles(AllProcesses);
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
            message.p.memorySize=Process->memorySize;
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

int* readalgo(int state)
{
    char **argv=NULL;
    int argc=0;
    gtk_init(&argc, &argv);
    GtkWidget *window;
    GtkWidget *button1, *button2, *button3;
    GtkWidget *entry;
    GtkWidget *entry_file;
    GtkWidget *close_button;
    GtkWidget *box;

    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);

    box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    gtk_container_add(GTK_CONTAINER(window), box);

   

    entry_file = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(entry_file), "Enter the file path");
    gtk_box_pack_end(GTK_BOX(box), entry_file, FALSE, FALSE, 0);


    if(state==0)
    {
    entry = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(entry), "Enter the quantum for Round Robin");
    gtk_box_pack_end(GTK_BOX(box), entry, FALSE, FALSE, 0);
    

    button1 = gtk_button_new_with_label("Round Robin");
    gtk_box_pack_start(GTK_BOX(box), button1, FALSE, FALSE, 0);
    g_signal_connect(button1, "clicked", G_CALLBACK(on_button1_clicked), entry);

    button2 = gtk_button_new_with_label("Highest Priority First");
    gtk_box_pack_start(GTK_BOX(box), button2, FALSE, FALSE, 0);
    g_signal_connect(button2, "clicked", G_CALLBACK(on_button2_clicked), NULL);

    button3 = gtk_button_new_with_label("Shortest Remaining Time Next");
    gtk_box_pack_start(GTK_BOX(box), button3, FALSE, FALSE, 0);
    g_signal_connect(button3, "clicked", G_CALLBACK(on_button3_clicked), NULL);
    }

    
    struct Callback_data *data = malloc(sizeof(struct Callback_data));
    data->window = window;
    data->entry = entry_file;
    data->state=state;
    close_button = gtk_button_new_with_label("Submit");
    gtk_box_pack_end(GTK_BOX(box), close_button, FALSE, FALSE, 0);
    g_signal_connect(close_button, "clicked", G_CALLBACK(on_close_button_clicked), data);
    gtk_widget_show_all(window);

    gtk_main();
    
}

int create_schedular() {
    int pid = fork();
    if (pid == 0) {
        //printf("before sending %d %d\n", Scheduling_algo[0], Scheduling_algo[1]);
        char arg1[2], arg2[3]; // Temporary strings to hold the converted integers
        snprintf(arg1, sizeof(arg1), "%d", Scheduling_algo[0]); // Convert Scheduling_algo[0] to string
        snprintf(arg2, sizeof(arg2), "%d", Scheduling_algo[1]); // Convert Scheduling_algo[1] to string
        char *args[] = {"./sched", arg1, arg2, NULL}; // Pass the strings as arguments to sched program
        execv(args[0], args);
        perror("execv failed");
        exit(EXIT_FAILURE);
    } else if (pid == -1) {
        perror("error occurred while forking\n");
        return -1; // Return an error code since forking failed
    }
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
    FILE *file = fopen(file_name, "r"); 

    while (file == NULL)
    {
        printf("Error opening the file.\n");
        readalgo(1);
        file = fopen(file_name, "r"); 
    }

    // Read and ignore the first line to ignore it
    char buffer[100];
    fgets(buffer, sizeof(buffer), file);
    struct processdata ptr;
    while (fscanf(file, "%d\t%d\t%d\t%d\t%d", &ptr.id, &ptr.arrival, &ptr.runtime, &ptr.priority,&ptr.memorySize) == 5) {
        // Dynamically allocate memory for each process
        struct processdata *newProcess = (struct processdata *)malloc(sizeof(struct processdata));
        // Copy data to the dynamically allocated process object
        newProcess->id = ptr.id;
        newProcess->arrival = ptr.arrival;
        newProcess->runtime = ptr.runtime;
        newProcess->priority = ptr.priority;
        newProcess->memorySize=ptr.memorySize;

        // Enqueue the dynamically allocated process object into the queue
        enqueue(AllProcesses, newProcess);

        printf("ID: %d, Arrival: %d, Runtime: %d, Priority: %d MemorySize: %d \n", ptr.id, ptr.arrival, ptr.runtime, ptr.priority,ptr.memorySize);
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

void on_button1_clicked(GtkButton *button, gpointer user_data) {
    GtkWidget *entry = GTK_WIDGET(user_data);
    const gchar *text = gtk_entry_get_text(GTK_ENTRY(entry));
    int new_quantum = atoi(text);
    if (new_quantum > 0) {
        algo = 1; // Set algorithm to Round Robin
        quantum = new_quantum; // Update quantum
        g_print("Algorithm set to Round Robin with quantum %d\n", quantum);
    } else {
        g_print("Invalid quantum value\n");
    }
}

// Callback function for Button 2 (Highest Priority First)
void on_button2_clicked(GtkButton *button, gpointer user_data) {
    algo = 3; // Set algorithm to Highest Priority First
    g_print("Algorithm set to Highest Priority First\n");
}

// Callback function for Button 3 (Shortest Remaining Time Next)
void on_button3_clicked(GtkButton *button, gpointer user_data) {
    algo = 2; // Set algorithm to Shortest Remaining Time Next
    g_print("Algorithm set to Shortest Remaining Time Next\n");
}

// Callback function for Close button
void on_close_button_clicked(GtkButton *button, gpointer user_data) {
    // Cast the user_data pointer to struct Callback_data pointer
    struct Callback_data *data = (struct Callback_data *)user_data;
    // Get the window and entry widget from the struct
    GtkWidget *window = data->window;
    GtkWidget *entry = data->entry;
    // Get the text from the entry widget
    const gchar *text = gtk_entry_get_text(GTK_ENTRY(entry));
    if(text==NULL)
    {
        return;
    }
    // Copy the text to a global variable or perform any desired action
    strcpy(file_name, text);
    // Allocate memory for Scheduling_algo and set values
    if(data->state==0)
    {
    Scheduling_algo[0] = algo;
    Scheduling_algo[1] = quantum;
    }
    
    // Destroy the window
    gtk_widget_destroy(window);
    free(data);
    // Free the memory allocated for Scheduling_algo
}