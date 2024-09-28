#include <stdio.h>
#include <stdlib.h>


// Structure to represent a node in the queue
struct NodeQueue {
    struct processdata* process;
    struct NodeQueue* next;
};

// Structure to represent the queue
struct Queue {
    struct NodeQueue *front, *rear;
};

// Function to create a new node
struct NodeQueue* createNode(struct processdata* process) {
    struct NodeQueue* newNode = (struct NodeQueue*)malloc(sizeof(struct NodeQueue));
    if (!newNode) {
        printf("Memory allocation failed!\n");
        exit(1);
    }
    newNode->process = process;
    newNode->next = NULL;
    return newNode;
}

// Function to create an empty queue
struct Queue* createQueue() {
    struct Queue* queue = (struct Queue*)malloc(sizeof(struct Queue));
    if (!queue) {
        printf("Memory allocation failed!\n");
        exit(1);
    }
    queue->front = queue->rear = NULL;
    return queue;
}

// Function to enqueue an element
void enqueue(struct Queue* queue, struct processdata* process) {
    struct NodeQueue* newNode = createNode(process);
    if (!queue->rear) {
        queue->front = queue->rear = newNode;
        return;
    }
    queue->rear->next = newNode;
    queue->rear = newNode;
}

// Function to dequeue an element
struct processdata* dequeue(struct Queue* queue) {
    if (!queue->front) {
        printf("Queue is empty!\n");
        exit(1);
    }
    struct processdata* process = queue->front->process;
    struct NodeQueue* temp = queue->front;
    queue->front = queue->front->next;
    if (!queue->front) {
        queue->rear = NULL;
    }
    free(temp);
    return process;
}

// Function to check if the queue is empty
int isEmpty(struct Queue* queue) {
    return queue->front == NULL;
}

// Function to print the queue
// void display(struct Queue* queue) {
//     struct NodeQueue* temp = queue->front;
//     while (temp) {
//         printf("%d ", temp->process->remaining_time); // Change this line according to the attribute you want to print
//         temp = temp->next;
//     }
//     printf("\n");
// }