#include <stdio.h>
#include <stdlib.h>


// Structure to represent a node in the queue
struct RR_NodeQueue {
    struct Process *process;
    struct RR_NodeQueue *next;
};

// Structure to represent the rr_queue
struct RR_Queue {
    struct RR_NodeQueue *front, *rear;
    int quantum;
    int rem_quantum;
};

// Function to create a new node
struct RR_NodeQueue *RR_createNode(struct Process *process) {
    struct RR_NodeQueue *newNode = (struct RR_NodeQueue *) malloc(sizeof(struct RR_NodeQueue));
    if (!newNode) {
        printf("Memory allocation failed!\n");
        exit(1);
    }
    newNode->process = (struct Process *) malloc(sizeof(struct Process));
    if (!newNode->process) {
        printf("Memory allocation failed!\n");
        exit(1);
    }
    *newNode->process = *process;
    newNode->next = NULL;
    return newNode;
}


// Function to create an empty queue
struct RR_Queue *RR_create_Queue(int quantum) {
    struct RR_Queue *queue = (struct RR_Queue *) malloc(sizeof(struct RR_Queue));
    if (!queue) {
        printf("Memory allocation failed!\n");
        exit(1);
    }
    queue->quantum = quantum;
    queue->rem_quantum = quantum;
    queue->front = queue->rear = NULL;

    return queue;
}

// Function to enqueue an element
void RR_enqueue(struct RR_Queue *queue, struct Process *process) {
    struct RR_NodeQueue *newNode = RR_createNode(process);
    if (!queue->rear) {
        queue->front = queue->rear = newNode;
        return;
    }
    queue->rear->next = newNode;
    queue->rear = newNode;

}

// Function to dequeue an element
struct Process *RR_dequeue(struct RR_Queue *queue) {

    if (!queue->front) {
        printf("Queue is empty!\n");
        exit(1);
    }
    struct Process *process = queue->front->process;
    struct RR_NodeQueue *temp = queue->front;
    queue->front = queue->front->next;
    if (!queue->front) {
        queue->rear = NULL;
    }
    free(temp);
    return process;
}

// Function to check if the queue is empty
int RR_isEmpty(struct RR_Queue *queue) {
    if(queue==NULL)
    {
        return 1;
    }
    return queue->front == NULL;
}


struct Process *RR_front(struct RR_Queue *queue) {
    if (RR_isEmpty(queue)) {
        // printf("null\n");
        return NULL;
    }
    return queue->front->process;
}

int RR_get_quantum(struct RR_Queue *queue) {
    return queue->quantum;
}

int RR_get_rem_quantum(struct RR_Queue *queue) {
    return queue->rem_quantum;
}

void RR_set_rem_quantum(struct RR_Queue *queue, int rem_quantum) {
    queue->rem_quantum = rem_quantum;
}
// Function to print the queue
// void display(struct Queue* queue) {
//     struct RR_NodeQueue* temp = queue->front;
//     while (temp) {
//         printf("%d ", temp->process->remaining_time); // Change this line according to the attribute you want to print
//         temp = temp->next;
//     }
//     printf("\n");
// }