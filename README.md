# Starve-Free Reader Writers Problem

CSN-232 OS Reading-Assignment
Submitted By- Naman Jain
Enrollment Number- 19114056

## Table of Contents

1. [Introduction](#one)
2. [Constraints](#two)
3. [First Reader-Writers Problem](#four)
4. [Second Reader-Writers Problem](#five)
5. [Starve-Free Reader-Writers Problem](#six)

## <a name="one"></a>Introduction

Many Processes in a Multi-Processor environment may need to access the same memory resource at a time which may lead to data inconsistency. Any process may have 2 kinds of behavior based on its requirements:

- **Readers** - Only For Reading the database never modify it.
- **Writers** - Modifies the database content and may read also.

## <a name="two"></a>Constraints

1. Any number of Readers can access database when there are no active writers.
2. Writers can access database when there are no readers and other writers (i.e, only one at a time).
3. At any time, Only a single thread can manipulate state variables.

We use Semaphores and Mutex locks to solve this problem.

## <a name="four"></a>First Reader-Writers Problem

The First Reader-Writers Problem allows multiple readers to read the Resource while maintaining mutual exclusion among writers. Because of this it may lead to **starvation of writers** and also known as **_readers-preference_** problem.

## <a name="five"></a>Second Readers-Writers Problem

In Contrary, Second Readers-Writers solution helps **_prevent starvation of writers_** and is called the **writer-preference** problem.

## <a name="six"></a> Starve-Free Reader-Writers Problem

As both of the above solutions allow either readers or writers to starve, we need a solution that prevents starvation altogether. This Problem is also Known as Third Reader-Writers Problem.

### FIFO Queue

Here, I have implemented Queue Struct for Semaphore FIFO Queue. We push a new process at end of Queue which gets blocked till it turn and gets woken up and executed during pop operation.

```C++ style Queue Struct
// A queue node.
struct Node
{
    // Pointer to next node and process_id for current process
    Node* next;
    pid_t value;
};


class Queue
{
    // Front pointer for pop operation
    // Rear pointer for adding a new node
    Node* Front, *Rear;

    public:
    // Method to push a node at end of queue
    void push(pid_t val)
    {
        // Create a node and initialize pid value
        Node* new_process = new Node();
        new_process->value = val;

        // Update front if queue is empty
        if(Rear != NULL)
            Rear->next = new_process;
        else
            Front = new_process;

        // Update Rear to point to added node
        Rear = new_process;
    }

    // Method to pop a Node from front of queue
    pid_t pop()
    {
        // check if queue if empty
        if(Front == NULL)
        {
            // Queue is Empty : Underflow Error
            return -1;
        }
        else
        {
            // Get Front node
            pid_t process_id = Front->value;

            // Update Front node
            Front = Front->next;

            // Update Rear if queue is empty now
            if(Front == NULL)
                Rear = NULL;

            // Return the value of procss id
            return process_id;
        }
    }
};
```

### The Semaphore

Implementation for Semaphore struct is done using previously implemented FIFO Queue. It has two methods, wait() adds a process to Queue, decrements the value and blocks it till it gets the turn. Signal is used to get the next process from Queue and wakeup for its operation and turn.

```cpp
// C++-Style Struct Implementation of Semaphore
class Semaphore
{

    int value = 1;
    // Queue to Store Processes waiting to be executed
    Queue processes = Queue();

    public:
    // wait method to block other process while
    // a code executes inside critical section
    void wait(pid_t process_id)
    {
        value--;
        if(value < 0)
        {
            processes.push(process_id);

            // Block other processes using non-busy waiting
            // until signal() is called after execution of
            // Critical Section by executing process.
            wait(process_id);
        }
    }

    void signal()
    {
        value++;
        if(value <= 0)
        {
            pid_t pid = processes.pop();
            // Wakeup the next process for execution from queue
            wakeup(pid);
        }
    }
};
```

### Global variables

Global Semaphore variables are declared for reader/writer Queue (rw_queue), resource, and reader count (r_mutex) changes. Initially reader_count variable is initialized to 0 and data is initialized to some value.

```cpp
// cpp style Global Variable declaration and initialization
//Declare Semaphores
struct Semaphore rw_queue;
struct Semaphore resource;
struct Semaphore r_mutex;

// Initialize readers count
int reader_count = 0;

// Initial critical data section to be shared
int data = 1;
```

### Implementation for writers

First writer waits in rw_queue semaphore till other other active requests are serviced. Now it waits its turn to modify data variable by getting resource Semaphore. Finally, in Entry Section rw_queue Semaphore is resource is released for other processes and Writer enters Critical Section.

Inside Critical Section, Writer performs any kind of modification to data variable in memory. In the End, Writer releases resource Semaphore lock for other Reader/Writers.

```cpp
void *writer(pid_t process_id){

    /*-------------- ENTRY section ------------------*/

    // Wait in queue till other requests are serviced
    rw_queue.wait(process_id);
    // Wait its turn to modify resource semaphore
    resource.wait(process_id);
    // Release the rw_queue semaphore lock for other process
    rw_queue.signal();



    /*------------- CRITICAL section ----------------*/

    // Perform any modification of shared data variable here
    data = data*2;


    /*-------------- EXIT section -------------------*/

    // Release the resource semaphore lock for next process
    resource.signal();
}
```

### Implementation for readers

Reader method allows multiple readers to read at the same time. Similary, Reader waits to acquire the rw_queue Semaphore. Now, to update read_count variable it waits for other readers to update read_count and acquire r_mutex Semaphore. If current reader is the first reader under execution, reader waits for other writers to release resource semaphore and acquires it. At the end of Entry Section, rw_queue and r_mutex Semaphores are released for other processes.

In Critical Section, Reader performs read operation on data from memory. Now in Exit Section, Reader again waits till other readers have updated the read_count variable and then updates it. If no other active readers are remaining then resource Semaphore is released. Finally, r_mutex Semaphore is also released for other processes.

```cpp
void *reader(pid_t process_id){

    /*-------------- ENTRY section ------------------*/

    // Wait in queue till other requests are serviced
    rw_queue.wait(process_id);
    // Only one process should update readers count at a time
    r_mutex.wait(process_id);
    // Incrememt readers count
    reader_count++;

    // If current reader is the first reader wait till
    // other writers release the resource semaphore
    if(reader_count == 1)
        resource.wait(process_id);

    // Allow other process in queue to be serviced
    rw_queue.signal();
    // Readers count is updated
    r_mutex.signal();



    /*------------- CRITICAL section ----------------*/

    // Perform any read operation of data variable from database
    printf("%d",data);



    /*-------------- EXIT section -------------------*/

    // Only one process should update readers count at a time
    r_mutex.wait(process_id);
    // Decrement read count semaphore after reading is done
    reader_count--;

    // Release resource semaphore for writer if
    // no other reader is remaining or left.
    if(reader_count == 0)
        resource.signal();

    // Allow other readers to modify readcount semaphore
    r_mutex.signal();

}
```

Starvation of Reader and Writers is prevented in this method and following technique is used:

- FIFO Semaphore Queue is used.
- When a reader is reading the resource
  - All other readers are allowed to read as well
  - writer is pushed to queue and writes after all readers have completed their operation.
- When a Writer is writing/modifying the resource
  - Any subsequent reader/writers are pushed to the waiting queue.
