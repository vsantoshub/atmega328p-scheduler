
#ifndef __ULTHREAD_H_
#define __ULTHREAD_H_

#define MAX_THREADS 6
#define MINIMAL_STACK_SIZE 128

#include <stdio.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include "stdlib.h"


/* task state definition */
typedef enum ULState {
    RUNNING,
    READY,
    WAITING,
    FINISHING
} ULState;

typedef enum ULPriority {
    THREAD_PRIORITY_TIME_CRITICAL,
    THREAD_PRIORITY_HIGHEST,
    THREAD_PRIORITY_ABOVE_NORMAL,
    THREAD_PRIORITY_NORMAL,
    THREAD_PRIORITY_BELOW_NORMAL,
    THREAD_PRIORITY_LOWEST
} ULPriority;

//resolve o problema de dependencia circular
//forward declaration
struct ULThread;

/* linked list structure */
typedef struct ULList {
    struct ULThread * head;
    struct ULThread * tail;
    uint8_t size;
} ULList;

/* thread structure */
typedef struct ULThread {
    volatile uint16_t *pTopOfStack;
    uint8_t tid;
    uint8_t state;
    uint8_t ret_val;
    uint8_t priority;
    ULList waiting;
    struct ULThread  * next;
    struct ULThread  * prev;
} ULThread;

/* scheduler structure */
typedef struct ULScheduler {
    ULThread * running;
    uint8_t *memptr;    // pointer to free memory
    ULList ready;
    uint8_t last_tid;
} ULScheduler;


/*list manipulators*/
ULThread * list_get(ULList * l, int tid);
ULThread * list_removeTail(ULList * l);
ULThread * list_removeHead(ULList * l);
ULThread * list_remove_tid(ULList * l, int tid);
void list_insert(ULList * l, ULThread *t);
void list_append(ULList * l, ULThread * t);
void plist_insert(ULList * l, ULThread * t);
const char * get_threadState(enum ULState state);
const char * get_threadPriority(enum ULPriority priority);
void waitfor(int nMiliseconds);
void reschedule(void);
void pauseScheduler(void);
void resumeScheduler(void);
void swapcontext(ULThread * old_thread, ULThread * new_thread) __attribute__((naked));;
void debug();
void initTimer();
void idle_task();
int create_idle(void(*entry)(void*), int priority, int stacksize, void * args);
/* public methods to manipulate the threads
 * these methods are described in link below:
 *
 * http://www.lisha.ufsc.br/teaching/os/exercise/ulthreads.html
 *
 */

/* Function descriptions
 *
 *   int init(void)
 *   Initializes the user-level thread manager, registering the calling process as the first thread. This function must be     called before any other in the package and returns "0" in case of success and "-1" in case of failure.
 *
 *   int create(void (* entry)(int), int arg)
 *   Creates a thread with entry point set to entry, which is a pointer to a plain function taking an integer as argument. The function returns an identifier for the created thread in case of success and "-1" in case of failure.
 *
 *   int join(int tid, int * status)
 *   Waits for the thread identified by tid to exit, returning it's return value in status. In case of success, this function  returns "0", otherwise, "-1".
 *
 *   void exit(int status)
 *   Immediately terminates the execution of the calling thread, allowing for a return value to be eventually handed over to   join.
 *
 *   void yield(void)
 *   Causes the calling thread to release the CPU, thus activating the scheduler.
 *
 *   int getid(void)
 *   Returns the identifier of the calling thread.
 *
 */

int init_scheduler(void);
int create(void(*entry)(void*), int priority, int stacksize, void *args);
int join(int tid, int * ret_val);
void finish(int ret_val);
void yield(void);
int getid(void);




#endif /* __ULTHREAD_H_ */
