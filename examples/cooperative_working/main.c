/*
 * main.cpp
 *
 *  Created on: Apr 6, 2014
 *      Author: victor
 */
#include <avr/io.h>
#include <stdlib.h>
#include <stdio.h>
#include "ulthread.h"
#include "util/delay.h"
#include "uart.h"
#include "avr/interrupt.h"
#include "LS_defines.h"
/* define CPU frequency in Mhz here if not defined in Makefile */
#ifndef F_CPU
#define F_CPU 16000000UL
#endif

int teste = 15;

/* 9600 baud */
#define UART_BAUD_RATE 9600
#define UNUSED(x) (void)(x)

#ifdef debug
    static FILE mystdout = FDEV_SETUP_STREAM(usart_putchar_printf, NULL, _FDEV_SETUP_WRITE);

/*************************************************************************
Function: uartInit
Purpose: initialize uart in 9600 8-N-1 standard and sets serial to stdout
 **************************************************************************/
void uartInit(void) { /* testado: OK */
    /*
     * Initialize UART library, pass baudrate and AVR cpu clock
     * with the macro
     * UART_BAUD_SELECT() (normal speed mode )
     * or
     * UART_BAUD_SELECT_DOUBLE_SPEED() ( double speed mode)
     */
    uart_init( UART_BAUD_SELECT(UART_BAUD_RATE,F_CPU) );

    // setup our stdio stream
    stdout = &mystdout;
}

#endif

void thr_run(void * args) {
    int * arg = (int*)args;
    //se quiser usar o valor de a tem que deferenciar ex. *a
    while (1) {
#ifdef debug
        printf("Thread %d: running...\n", getid());
        printf("Thread arg= %d\n", *arg);
        printf("Iteration number= %d\n", teste);
#endif
        PORTB = 127 & *arg;
        _delay_ms(500);
        cpl_bit(PORTB, PB4);
        _delay_ms(500);
        *arg -= 1;
        if (*arg == 0) break;
        /*free cpu to run another thread with yield method()*/
        yield();
    }
    finish(*arg);
}

int main(void) {
#ifdef debug
    uartInit();
    sei();
    printf("AVR Scheduler\n");
    printf("PortB test.\n");
    DDRB = 0b11111111;
    PORTB = 5;
    _delay_ms(500);
#endif 
    PORTB = 0;
#ifdef debug    
    printf("Initializing Scheduler...\n");
#endif
    init_scheduler();
#ifdef debug
    printf("Done\n");
#endif    
    int res_a, res_b;
//    int res_c;
    int arg_a = 7;
    int arg_b = 7;
//    int arg_c = 7;
    int tid_a = create(&thr_run, THREAD_PRIORITY_NORMAL, MINIMAL_STACK_SIZE, &arg_a);
#ifdef debug
    printf("Thread A created\n");
#endif
    int tid_b = create(&thr_run, THREAD_PRIORITY_NORMAL, MINIMAL_STACK_SIZE, &arg_b);
#ifdef debug    
    printf("Thread B created\n");
#endif
//    int tid_c = create(&thr_run, THREAD_PRIORITY_ABOVE_NORMAL, MINIMAL_STACK_SIZE, &arg_c);
#ifdef debug
    printf("Thread C created\n");
#endif
    //UNUSED(tid_a); //macro utilizada para remover warning de variavel nao utilizada
    //UNUSED(tid_b);
    //UNUSED(tid_c);
    join(tid_a, &res_a);
    join(tid_b, &res_b);
//    join(tid_c, &res_c);

    while(1) {
#ifdef debug
        printf("While(1):Main\n");
        _delay_ms(250);
#endif
        PORTB = 17;
    }
}



