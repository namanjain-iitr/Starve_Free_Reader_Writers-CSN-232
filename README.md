<h2 align=center> CSN 232 Assignment </h2>

<h3 align=center> Starve-Free Readers Writers Problem in C </h3>

***SEMAPHORES***
- **rw_queue** : Maintains the order of arrival of readers and writers.
- **r_mutex** : Semaphore used to provide mutual exclusion to r_count (or readers count) variable currently executing in critical section.
- **resource** : Semaphore used to prevent readers and writer/s from simultaneously executing Critical Section Code.

***Shared variables***
- r_count : Stores count for number of readers present in critical section at any given time.
- data : Part of program that is shared among various threads.

***Reader function***
- **Entry section** :
  - Reader Enters the request queue and waits till it acquires the "rw_queue" Semaphore.
  - Now reader tries to aquire "r_mutex" SEMAPHORE to modify r_count.
  - Reader waits till it acquires the "resource" SEMAPHORE if it the only reader under execution.
  - "rw_queue" and "r_mutex" Semaphores are released before entering critical section.

- **Critical section** : Reader reads the shared data variable.

- **Exit section** : 
  - Reader waits till it aquires "r_mutex" Semaphore to modify r_count variable.
  - If it is the last reader, it releases "resource" Semaphore.
  - "r_mutex" Semaphore is released after modifying r_count value.

***Writer function***
- **Entry section** :
  - Writer Enters the request queue and waits till it acquires the "rw_queue" Semaphore.
  - Now it waits to aquire "resource" Semaphore and enter critical section.
  - "rw_queue" Semaphore is released before entering critical section.

- **Critical section** : Writer modifies the shared data.

- **Exit section** :
  - Writer releases the resource SEMAPHORE.


***Main function***
- Number of readers and writers are taken as input from the user.
- Threads are created corresponding to number of readers and writers.
- Semaphores are now Initialized.
- Reader and Writer threads are initialized with corresponding functions.
- Now, all threads are joined back to the parent thread.
- Finally all Semaphores are destroyed after finishing execution.
