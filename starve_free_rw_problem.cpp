#include <iostream>
using namespace std;

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

// cpp style Global Variable declaration and initialization
//Declare Semaphores
struct Semaphore rw_queue;
struct Semaphore resource;
struct Semaphore r_mutex;

// Initialize readers count
int reader_count = 0;

// Initial critical data section to be shared
int data = 1;

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