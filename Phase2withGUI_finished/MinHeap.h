
#include <stdio.h>
#include <stdlib.h>


// Define the Process struct
struct processdata{
    int priority; 
    int runtime;
    int id;
    int arrival; 
    int memorySize;
};
struct MemNode{
    int size;
    int usedsize;
    int free;
    struct MemNode* Parent;
    struct MemNode* left;
    struct MemNode* right;
    int startidx;
    int endidx;
};
// // Define the Process struct
enum states
{
    START,
    STOP,
    RESUMED,
    FINISH
};

enum Memorystates
{
    ALLOCATED,
    DEALLOCATED
};
struct Process {
    int priority; // Priority of the process
    int remaining_time; // Remaining execution time of the process
    int id;
    int arrival; 
    int runtime;
    enum states state;
    int waittime;
    int finishTime;
    int TA;
    float WTA;
    int ProcessId;
    int memorySize;
    struct MemNode *myadress;
    // Add more attributes as needed
};
struct cpu
{
    int runtime;
    int waitime;
    int numberProcess;
    float totalWTA;
    int totalwaiting;
};

// Declare a heap structure
struct Heap
{
    struct Process **process;
    int size;
    int capacity;
    int heapify_by_priority; // 0 for remaining_time, 1 for priority
};

// Define the struct Heap name
typedef struct Heap heap;

// Forward declarations
heap *createHeap(int capacity, int heapify_by_priority);
void insertHelper(heap *h, int index);
void heapify(heap *h, int index);
struct Process *extractMin(heap *h);
void insert(heap *h, struct Process *data);

// Define a createHeap function
heap *createHeap(int capacity, int heapify_by_priority)
{
    // Allocating memory to heap h
    heap *h = (heap *)malloc(sizeof(heap));

    // Checking if memory is allocated to h or not
    if (h == NULL)
    {
        printf("Memory error");
        return NULL;
    }
    // set the values to size and capacity
    h->size = 0;
    h->capacity = capacity;
    h->heapify_by_priority = heapify_by_priority;

    // Allocating memory to array of pointers to Process
    h->process = (struct Process **)malloc(capacity * sizeof(struct Process *));

    // Checking if memory is allocated to h or not
    if (h->process == NULL)
    {
        printf("Memory error");
        return NULL;
    }

    return h;
}

// Defining insertHelper function
void insertHelper(heap *h, int index)
{
    // Store parent of element at index
    // in parent variable
    int parent = (index - 1) / 2;

    if (h->heapify_by_priority)
    {
        if (h->process[parent]->priority > h->process[index]->priority)
        {
            // Swapping when child is smaller
            // than parent element
            struct Process *temp = h->process[parent];
            h->process[parent] = h->process[index];
            h->process[index] = temp;

            // Recursively calling insertHelper
            insertHelper(h, parent);
        }
    }
    else
    {
        if (h->process[parent]->remaining_time > h->process[index]->remaining_time)
        {
            // Swapping when child is smaller
            // than parent element
            struct Process *temp = h->process[parent];
            h->process[parent] = h->process[index];
            h->process[index] = temp;

            // Recursively calling insertHelper
            insertHelper(h, parent);
        }
    }
}

void heapify(heap *h, int index)
{
    int left = index * 2 + 1;
    int right = index * 2 + 2;
    int min = index;

    // Checking whether our left or child element
    // is at right index or not to avoid index error
    if (left >= h->size || left < 0)
        left = -1;
    if (right >= h->size || right < 0)
        right = -1;

    // store left or right element in min if
    // any of these is smaller that its parent
    if (h->heapify_by_priority)
    {
        if (left != -1 && h->process[left]->priority < h->process[index]->priority)
            min = left;
        if (right != -1 && h->process[right]->priority < h->process[min]->priority)
            min = right;
    }
    else
    {
        if (left != -1 && h->process[left]->remaining_time < h->process[index]->remaining_time)
            min = left;
        if (right != -1 && h->process[right]->remaining_time < h->process[min]->remaining_time)
            min = right;
    }

    // Swapping the nodes
    if (min != index)
    {
        struct Process *temp = h->process[min];
        h->process[min] = h->process[index];
        h->process[index] = temp;

        // recursively calling for their child elements
        // to maintain min heap
        heapify(h, min);
    }
}

struct Process *extractMin(heap *h)
{
    struct Process *deleteItem;

    // Checking if the heap is empty or not
    if (h->size == 0)
    {
        return NULL;
    }

    // Store the node in deleteItem that
    // is to be deleted.
    deleteItem = h->process[0];

    // Replace the deleted node with the last node
    h->process[0] = h->process[h->size - 1];
    // Decrement the size of heap
    h->size--;

    // Call minheapify_top_down for 0th index
    // to maintain the heap property
    heapify(h, 0);
    return deleteItem;
}

// Define a insert function
void insert(heap *h, struct Process *data)
{
    // Checking if heap is full or not
    if (h->size < h->capacity)
    {
        // Inserting data into an array
        h->process[h->size] = data;
        // Calling insertHelper function
        insertHelper(h, h->size);
        // Incrementing size of array
        h->size++;
    }
}
///////Agmai/////
struct Process * getfront(heap *h)
{
    if(h->size==0)
    return NULL;
    return h->process[0];
}
///////Agmai/////