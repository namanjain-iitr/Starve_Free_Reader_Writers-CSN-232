#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>

// Initialize readers count
int r_count = 0;

// Initial critical data section to be shared
int data = 1;

// Declare semaphores
sem_t rw_queue,resource,r_mutex;


// Function to implement read functionality of shared data
void *reader(void *rid)
{   
    /*-------------- ENTRY section ------------------*/

    // Wait in queue till other requests are serviced
    sem_wait(&rw_queue);

    // wait till read count semaphore is available
    sem_wait(&r_mutex);

    // Incrememt read count semaphoere
    r_count++;

    // If current reader is the first reader wait till
    // other writers release the resource semaphore
    if(r_count == 1)
        sem_wait(&resource);

    // Release the rw_queue semaphore for other process
    sem_post(&rw_queue);
    
    // Allow other readers to modify readcount semaphore
    sem_post(&r_mutex);
    
    

    /*------------- CRITICAL section ----------------*/
    printf("Reader %d: read data as %d\n",*((int *)rid),data);
    


    /*-------------- EXIT section -------------------*/

    // Wait for read count semaphore access
    sem_wait(&r_mutex);

    // Decrement read count semaphore after reading is done
    r_count--;

    // Release resource semaphore for writer if
    // no other reader is remaining or left.
    if(r_count == 0) 
        sem_post(&resource);
    
    // Allow other readers to modify readcount semaphore
    sem_post(&r_mutex);
}



// Function to implement write functionality of shared data
void *writer(void *wid)
{   
    /*-------------- ENTRY section ------------------*/

    // Wait in queue till other requests are serviced
    sem_wait(&rw_queue);

    // Wait its turn to modify resource semaphore
    sem_wait(&resource);

    // Release the rw_queue semaphore for other process
    sem_post(&rw_queue);
    


    /*------------- CRITICAL section ----------------*/
    data = data*2;
    printf("Writer %d modified data to %d\n",(*((int *)wid)),data);
    


    /*-------------- EXIT section -------------------*/

    // Release the resource semaphore for next process
    sem_post(&resource);
}




int main()
{   
    // Initialize count of readers, writers and max of both
    int READERS,WRITERS,MAX_ID;

    // Get count of readers from user input 
    printf("Enter Number of Readers: ");
    scanf("%d", &READERS);
    
    // Now get writers count as user input
    printf("Enter Number of Writers: ");
    scanf("%d", &WRITERS);
    
    // Store larger count in MAX_ID variable
    MAX_ID = READERS >= WRITERS ? READERS : WRITERS;
    

    // create an array and store id number in it
    int id_arr[MAX_ID];
    for(int i = 0; i < MAX_ID; i++) 
        id_arr[i] = i+1;
    
    // create array of threads for all readers and writers
    pthread_t read[READERS],write[WRITERS];
    

    // semaphore initialisation
    sem_init(&rw_queue,0,1);
    sem_init(&r_mutex,0,1);
    sem_init(&resource,0,1);


    // Initialize reader thread with reader function
    for(int i = 0; i < READERS; i++)
        pthread_create(&read[i], NULL, (void *)reader, (void *)&id_arr[i]);
    
    // Similarly, Initialize writer thread with writer function
    for(int i = 0; i < WRITERS; i++)
        pthread_create(&write[i], NULL, (void *)writer, (void *)&id_arr[i]);

    // Join all reader threads to parent process
    for(int i = 0; i < READERS; i++)
        pthread_join(read[i], NULL);
    
    // Also, Join all writer threads to parent process
    for(int i = 0; i < WRITERS; i++)
        pthread_join(write[i], NULL);
    

    // Destroy all semphores after execution of code
    sem_destroy(&rw_queue);
    sem_destroy(&r_mutex);
    sem_destroy(&resource);

    return 0;
    
}