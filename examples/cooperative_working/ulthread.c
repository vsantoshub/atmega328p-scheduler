/*************************************************************
 * ULThread.c
 *
 * Author: Victor Santos
 * Latest Modified: 05/19/2014i MM/DD/YYYY
 * email: victor.ifsc@gmail.com
 *
 * This file is where is implemented all the function of the thread creation, schedulling and all another functions
 * related to thread and scheduler manipulations.
 *
 ****************************************************************/

#include "ulthread.h"
#include <util/delay.h>
/*********************
 * Globals
 * *****************/

ULScheduler sch;
ULThread * threads[MAX_THREADS];
ULThread * volatile pxCurrentTCB = NULL;

/*******************************
 * Context manipulation functions
 * ***************************/
#define SAVE_CONTEXT()                                  \
    asm volatile (  "push   r0                      \n\t"   \
            "in     r0, __SREG__            \n\t"   \
            "cli                            \n\t"   \
            "push   r0                      \n\t"   \
            "push   r1                      \n\t"   \
            "clr    r1                      \n\t"   \
            "push   r2                      \n\t"   \
            "push   r3                      \n\t"   \
            "push   r4                      \n\t"   \
            "push   r5                      \n\t"   \
            "push   r6                      \n\t"   \
            "push   r7                      \n\t"   \
            "push   r8                      \n\t"   \
            "push   r9                      \n\t"   \
            "push   r10                     \n\t"   \
            "push   r11                     \n\t"   \
            "push   r12                     \n\t"   \
            "push   r13                     \n\t"   \
            "push   r14                     \n\t"   \
            "push   r15                     \n\t"   \
            "push   r16                     \n\t"   \
            "push   r17                     \n\t"   \
            "push   r18                     \n\t"   \
            "push   r19                     \n\t"   \
            "push   r20                     \n\t"   \
            "push   r21                     \n\t"   \
            "push   r22                     \n\t"   \
            "push   r23                     \n\t"   \
            "push   r24                     \n\t"   \
            "push   r25                     \n\t"   \
            "push   r26                     \n\t"   \
            "push   r27                     \n\t"   \
            "push   r28                     \n\t"   \
            "push   r29                     \n\t"   \
            "push   r30                     \n\t"   \
            "push   r31                     \n\t"   \
            "lds    r26, pxCurrentTCB       \n\t"   \
            "lds    r27, pxCurrentTCB + 1   \n\t"   \
            "in     r0, 0x3d                \n\t"   \
            "st     x+, r0                  \n\t"   \
            "in     r0, 0x3e                \n\t"   \
            "st     x+, r0                  \n\t"   \
);

/*
 * Opposite to portSAVE_CONTEXT().  Interrupts will have been disabled during
 * the context save so we can write to the stack pointer.
 */

#define RESTORE_CONTEXT()                               \
    asm volatile (  "lds    r26, pxCurrentTCB       \n\t"   \
            "lds    r27, pxCurrentTCB + 1   \n\t"   \
            "ld     r28, x+                 \n\t"   \
            "out    __SP_L__, r28           \n\t"   \
            "ld     r29, x+                 \n\t"   \
            "out    __SP_H__, r29           \n\t"   \
            "pop    r31                     \n\t"   \
            "pop    r30                     \n\t"   \
            "pop    r29                     \n\t"   \
            "pop    r28                     \n\t"   \
            "pop    r27                     \n\t"   \
            "pop    r26                     \n\t"   \
            "pop    r25                     \n\t"   \
            "pop    r24                     \n\t"   \
            "pop    r23                     \n\t"   \
            "pop    r22                     \n\t"   \
            "pop    r21                     \n\t"   \
            "pop    r20                     \n\t"   \
            "pop    r19                     \n\t"   \
            "pop    r18                     \n\t"   \
            "pop    r17                     \n\t"   \
            "pop    r16                     \n\t"   \
            "pop    r15                     \n\t"   \
            "pop    r14                     \n\t"   \
            "pop    r13                     \n\t"   \
            "pop    r12                     \n\t"   \
            "pop    r11                     \n\t"   \
            "pop    r10                     \n\t"   \
            "pop    r9                      \n\t"   \
            "pop    r8                      \n\t"   \
            "pop    r7                      \n\t"   \
            "pop    r6                      \n\t"   \
            "pop    r5                      \n\t"   \
            "pop    r4                      \n\t"   \
            "pop    r3                      \n\t"   \
            "pop    r2                      \n\t"   \
            "pop    r1                      \n\t"   \
            "pop    r0                      \n\t"   \
            "out    __SREG__, r0            \n\t"   \
            "pop    r0                      \n\t"   \
        );

void swapcontext(ULThread * old_thread, ULThread * new_thread) {

    /* Make the global variable context point on the current stack pointer
       to use it the assembly SAVE_CONTEXT macro */
    pxCurrentTCB = old_thread;
    SAVE_CONTEXT();
    // restore the new context only if it exists
    if (new_thread != 0) {
        /* Make the global variable context point on the new stack pointer
           to use it the assembly RESTORE_CONTEXT macro.*/
        pxCurrentTCB = new_thread;
        RESTORE_CONTEXT();
    } else {
        printf("Trying to load a non existing context ? \n");
    }
    // naked function. need manual ret
    __asm__ __volatile__ ( "ret" );
}


/*******************************
 * List manipulation functions
 * ***************************/

/*******************************************
 * ULThread * list_get(ULList * l, int tid)
 *
 * Get the thread object specified by its tid number
 * args: ULList * list, int task_id
 * return: ULThread of desired tid
 *
 ******************************************/
ULThread * list_get(ULList * l, int tid) {
    ULThread * ret = 0;
    if (l->size > 0) {
        ret = l->head;
        while (ret != 0) {
            if (ret->tid == tid) {
                break;
            } else {
                ret = ret->next;
            }
        }
    }
    return ret;
}

/**********************************************
 * ULThread * list_removeTail(ULList * l)
 *
 * Remove the thread in the list tail and returns the removed thread object
 * args: ULList * list
 * return: ULThread removed
 *
 * ******************************************/
ULThread * list_removeTail(ULList * l) {
    ULThread * ret = 0;
    if (l->size > 0) {
        if (l->size == 1) {
            ret = l->tail;
            ret->prev = 0;
            ret->next = 0;
            l->head = 0;
            l->tail = 0;
            l->size = 0;

        } else {
            ret = l->tail;
            l->tail = l->tail->prev;
            l->tail->next = 0;
            ret->prev = 0;
            ret->next = 0;
            l->size--;
        }
    }
    return ret;
}

/***********************************************
 * ULThread * list_removeHead(ULList * l)
 *
 * Remove the thread in the list head and return the removed thread object
 * args: ULList * list
 * return: ULThread removed
 *
 * *********************************************/
ULThread * list_removeHead(ULList * l) {

    ULThread * ret = 0;
    if (l->size > 0) {
        if (l->size == 1) {
            ret = l->head;
            ret->prev = 0;
            ret->next = 0;
            l->head = 0;
            l->tail = 0;
            l->size = 0;

        } else {
            l->head = l->head->next;
            ret = l->head->prev;
            l->head->prev = 0;
            ret->next = 0;
            l->size--;
        }
    }
    return ret;
}

/***********************************************
 * ULThread * list_remove_tid(ULList * l, int tid)
 *
 * Remove the thread with specified tid in ULList * list passed and return the removed thread object
 * args: ULList * list, int task_tid
 * return: ULThread removed
 *
 * **********************************************/
ULThread * list_remove_tid(ULList * l, int tid) {
    ULThread * ret = 0;
    if (l->size > 0) {
        ret = l->head;
        while (ret != 0) {
            if (ret->tid == tid) {
                break;
            } else {
                ret = ret->next;
            }
        }
        if (ret != 0) {
            /*Tests if it is HEAD*/
            if (ret->prev == 0)
                ret = list_removeHead(l);
            /*Tests if it is TAIL*/
            else if (ret->next == 0)
                ret = list_removeTail(l);
            /*No HEAD nor TAIL*/
            else {
                ret->next->prev = ret->prev;
                ret->prev->next = ret->next;
                ret->prev = 0;
                ret->next = 0;
                l->size--;
            }
        }
    }
    return ret;
}

/*************************************************
 * void plist_insert(ULList * l, ULThread *t)
 *
 * Insert thread by priority
 * args: ULList * list, ULThread * thread
 *
 * *********************************************/
void plist_insert(ULList * l, ULThread *t) {

    ULThread * ret = 0;
    //if empty list, the thread will be simply added
    if (l->size == 0) {
        l->head = t;
        l->tail = t;
        l->size++;
    } else {
        if (t->priority == THREAD_PRIORITY_LOWEST) {
            list_append(l, t);
        } else {
            ret = l->tail;
            while (ret != 0) {  //sweeps tail to head
                if (ret->priority <= t->priority) {
                    break;
                } else {
                    ret = ret->prev;
                }
            }
            if (ret != 0) {
                /*Tests if it is TAIL*/
                if (ret->next == 0)
                    list_append(l, t);
                else {
                    t->next = ret->next;
                    ret->next->prev = t;
                    ret->next = t;
                    t->prev = ret;
                    l->size++;
                }
            } else {
                //sweeped all the list, and all the elements (or the only one) have/has low priority than t
                list_insert(l, t);
            }
        }
    }
}
/*************************************************
 * void list_insert(ULList * l, ULThread *t)
 *
 * Insert thread on list head
 * args: ULList * list, ULThread * thread
 *
 * *********************************************/
void list_insert(ULList * l, ULThread *t) {
    if (l->size == 0) {
        l->head = t;
        l->tail = t;
        l->size++;
    } else {
        t->prev = 0;
        t->next = l->head;
        l->head->prev = t;
        l->head = t;
        l->size++;
    }
}

/**********************************************
 * void list_append(ULList * l, ULThread * t)
 *
 * Insert thread on list tail
 * args: ULList * list, ULThread * thread
 *
 * *******************************************/
void list_append(ULList * l, ULThread * t) {
    if (l->size == 0) {
        l->head = t;
        l->tail = t;
        l->size++;
    } else {
        t->next = 0; //bug fix:14/06
        t->prev = l->tail;
        l->tail->next = t;
        l->tail = t;
        l->size++;
    }
}

/***************************************************
 * Thread manipulation functions
 * ************************************************/

int create(void (*entry)(void*), int priority, int stacksize, void *args) {

    uint8_t *sp=NULL;
    uint8_t i;
    uint16_t parameter_tmp_addr;

    if (sch.last_tid > MAX_THREADS)
        return -1;

    ULThread * thread = (ULThread *) malloc(sizeof(ULThread));
    thread->tid = sch.last_tid++;
    threads[thread->tid] = thread;
    thread->state = READY;
    thread->waiting.head = 0;
    thread->waiting.tail = 0;
    thread->waiting.size = 0;
    thread->prev = 0;
    thread->next = 0;
    thread->priority = priority;

    //initialize stack
    sp = sch.memptr;
    sch.memptr -= stacksize;   //decrease free mem pointer

    parameter_tmp_addr = (uint16_t) entry;

    // initialize stack
    // set up the entry point in stack's bottom
    *sp-- = (uint8_t) (parameter_tmp_addr & (uint16_t) 0x00FF); // store PC(lo)
    parameter_tmp_addr >>= 8;
    *sp-- = (uint8_t) (parameter_tmp_addr & (uint16_t) 0x00FF); // store PC(hi)

    *sp-- = (uint8_t) 0x00; //R0
    *sp-- = (uint8_t) 0x80; //SREG
    *sp-- = (uint8_t) 0x00; //R1

    for (i = 2; i <= 23; i++)
        *sp-- = (uint8_t) 0x00; // r2-r23

    // Save args in r24-25 (input arguments stored in these registers)
    parameter_tmp_addr = (uint16_t) args;
    *sp-- = (uint8_t) (parameter_tmp_addr & (uint16_t) 0x00FF); // store args(lo)
    parameter_tmp_addr >>= 8;
    *sp-- = (uint8_t) (parameter_tmp_addr & (uint16_t) 0x00FF); // store args(hi)

    // store r26-r31
    for (i = 26; i <= 31; i++)
        *sp-- = (uint8_t) 0x00;

    thread->pTopOfStack = (uint16_t *) sp; // store stack pointer
    plist_insert(&sch.ready, thread); //insert with priority support
    return thread->tid;
}

void yield(void) {
    ULThread * old_thread;

    if (sch.ready.size > 0) { //uma tarefa running e uma+ ready
        if (sch.running->priority >= sch.ready.head->priority) {
            //pauseScheduler(); //preempcao
            old_thread = sch.running;
            old_thread->state = READY;
            sch.running = list_removeHead(&sch.ready);
            sch.running->state = RUNNING;
            //list_append(&sch.ready, old_thread);
            plist_insert(&sch.ready, old_thread);
#ifdef debug
            printf("yield\n");//
#endif
            //resumeScheduler(); //preempcao
            swapcontext(old_thread, sch.running);
        } 
    }
}

/*returns the tID of the running thread*/
int getid(void) {
    return sch.running->tid;
}

/*espera a thread especificada com tid join acabar para prosseguir seu andamento */
int join(int tid, int * ret_val) {

    ULThread * old_thread;
    if (threads[tid]) {
        if (threads[tid]->state != FINISHING) {
            if (sch.ready.size > 0) { //uma tarefa running e uma+ ready
                //pauseScheduler(); //preempcao
                old_thread = sch.running;
                old_thread->state = WAITING;
                sch.running = list_removeHead(&sch.ready);
                sch.running->state = RUNNING;
                //pega a thread running e insere na lista de waiting da thread A
                list_append(&threads[tid]->waiting, old_thread);
#ifdef debug
                printf("join\n");
#endif
                //resumeScheduler(); //preempcao
                swapcontext(old_thread, sch.running);

            }
        }
        //atualiza o estado da thread para FINISHING
        *ret_val = threads[tid]->ret_val;
        return 0;

    } else {
        return -1;
    }
}

void finish(int ret_val) {
    ULThread * tmp; //variavel de auxilio
    ULThread * thr = sch.running;
    thr->ret_val = ret_val;
    thr->state = FINISHING;
    while (thr->waiting.size) {
        //pauseScheduler(); //preempcao
        tmp = list_removeHead(&thr->waiting);
        tmp->state = READY;
        //acordando as tarefas que estao esperando liberacao
        plist_insert(&sch.ready, tmp);
        //resumeScheduler(); //preempcao
    }
    if (sch.ready.size > 0) { //uma tarefa running e uma+ ready
        //pauseScheduler(); //preempcao
        sch.running = list_removeHead(&sch.ready);
        sch.running->state = RUNNING;
#ifdef debug
        printf("finish ready.size > 0\n");
#endif
        //resumeScheduler(); //preempcao
        swapcontext(thr, sch.running);
    }
}

/*translates int state to a string-readable state*/
const char * get_threadState(enum ULState state) {
    switch (state) {
        case RUNNING:
            return "RUNNING";
        case READY:
            return "READY";
        case WAITING:
            return "WAITING";
        case FINISHING:
            return "FINISHING";
        default:
            return "ERROR";
    }
}

/*translates int priority to a string-readable priority*/
const char * get_threadPriority(enum ULPriority priority) {
    switch (priority) {
        case THREAD_PRIORITY_TIME_CRITICAL:
            return "THREAD_PRIORITY_TIME_CRITICAL";
        case THREAD_PRIORITY_HIGHEST:
            return "THREAD_PRIORITY_HIGHEST";
        case THREAD_PRIORITY_ABOVE_NORMAL:
            return "THREAD_PRIORITY_ABOVE_NORMAL";
        case THREAD_PRIORITY_NORMAL:
            return "THREAD_PRIORITY_NORMAL";
        case THREAD_PRIORITY_BELOW_NORMAL:
            return "THREAD_PRIORITY_BELOW_NORMAL";
        case THREAD_PRIORITY_LOWEST:
            return "THREAD_PRIORITY_LOWEST";
        default:
            return "ERROR";
    }
}

/*debug function to print all elements on a list*/
void print_list(ULList * l) {

    ULThread * handler;
    handler = l->head;
    while (handler != 0) {
        printf("thread ID: %d\tpriority: %s\tstate: %s\n", handler->tid,
                get_threadPriority(handler->priority),
                get_threadState(handler->state));
        handler = handler->next;
    }
}

void reschedule(void) {
    ULThread * old_thread;
#ifdef debug
    printf("reschedule\n");
    printf("lista de ready atual:\n ");
    print_list(&sch.ready);
#endif
    if (sch.ready.size > 0) { //uma tarefa running e uma+ ready
        if (sch.running->priority >= sch.ready.head->priority) {
#ifdef debug
            printf("prioridade da running maior ou igual do que a head da lista\n");
#endif
            //pauseScheduler(); //futuro trabalho, quando implementar preempcao
            old_thread = sch.running;
            old_thread->state = READY;
            sch.running = list_removeHead(&sch.ready);
            sch.running->state = RUNNING;
            plist_insert(&sch.ready, old_thread);
#ifdef debug
            printf("lista de ready atual:\n ");
            print_list(&sch.ready);
            printf("running ID: %d\n", sch.running->tid);
#endif
            //resumeScheduler(); //futuro trabalho, preempcao
            swapcontext(old_thread, sch.running);
        } 
    }
    //resumeScheduler(); //preempcao

}

void debug() {
    printf("prioridade do head da lista: %d\n", sch.ready.head->priority);
}

void idle_task(void * args) {
    while (1) {
#ifdef debug
        printf("idle_task\n");
#endif
        _delay_ms(250);
        yield();
    }
}

int create_idle(void (*entry)(void*), int priority, int stacksize, void * args) {

    uint8_t *sp=NULL;
    uint8_t i;
    uint16_t parameter_tmp_addr;

    if (sch.last_tid > MAX_THREADS)
        return -1;

    ULThread * thread = (ULThread *) malloc(sizeof(ULThread));
    thread->tid = sch.last_tid++;
    threads[thread->tid] = thread;
    thread->state = READY;
    thread->waiting.head = 0;
    thread->waiting.tail = 0;
    thread->waiting.size = 0;
    thread->prev = 0;
    thread->next = 0;
    thread->priority = priority;

    parameter_tmp_addr = (uint16_t) entry;

    // initialize stack
    // set up the entry point in stack's bottom
    *sp-- = (uint8_t) (parameter_tmp_addr & (uint16_t) 0x00FF); // store PC(lo)
    parameter_tmp_addr >>= 8;
    *sp-- = (uint8_t) (parameter_tmp_addr & (uint16_t) 0x00FF); // store PC(hi)

    *sp-- = (uint8_t) 0x00; //R0
    *sp-- = (uint8_t) 0x80; //SREG
    *sp-- = (uint8_t) 0x00; //R1

    for (i = 2; i <= 23; i++)
        *sp-- = (uint8_t) 0x00; // R2-R23

    // Save args in R24-R25 (input arguments stored in these registers)
    parameter_tmp_addr = (uint16_t) args;
    *sp-- = (uint8_t) (parameter_tmp_addr & (uint16_t) 0x00FF); // store args(lo)
    parameter_tmp_addr >>= 8;
    *sp-- = (uint8_t) (parameter_tmp_addr & (uint16_t) 0x00FF); // store args(hi)

    // store R26-R31
    for (i = 26; i <= 31; i++)
        *sp-- = (uint8_t) 0x00;

    thread->pTopOfStack = (uint16_t *) sp; // store stack pointer
    plist_insert(&sch.ready, thread); //insert with priority support
    return thread->tid;
}

/* init of the main thread */
int init_scheduler(void) {

    if (sch.last_tid > MAX_THREADS)
        return -1;
    //initialize scheduler
    sch.running = 0;
    sch.last_tid = 0;
    sch.ready.head = 0;
    sch.ready.tail = 0;
    sch.ready.size = 0;

    //forjar thread main
    sch.running = (ULThread *) malloc(sizeof(ULThread));
    sch.running->tid = sch.last_tid++;
    threads[sch.running->tid] = sch.running;
    sch.running->waiting.head = 0;
    sch.running->waiting.tail = 0;
    sch.running->waiting.size = 0;
    sch.running->state = RUNNING;
    sch.running->prev = 0;
    sch.running->next = 0;
    sch.memptr = (void*) (RAMEND - MINIMAL_STACK_SIZE);

    int idle_parameter = 0;
    create_idle(idle_task, THREAD_PRIORITY_LOWEST, MINIMAL_STACK_SIZE, &idle_parameter);
    return 0;
}

