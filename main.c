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
#include "LS_ATmega328.h"
#include "LS_HD44780.h"
 #include "string.h"

/* define CPU frequency in Mhz here if not defined in Makefile */
#ifndef F_CPU
#define F_CPU 16000000UL
#endif

#define MCU_8_BITS

int teste = 15;
volatile int leitura_adc = 0;
char buffer[10];

/* 9600 baud */
#define UART_BAUD_RATE 9600
#define UNUSED(x) (void)(x)

#ifdef debug
    static FILE mystdout = FDEV_SETUP_STREAM(usart_putchar_printf, NULL, _FDEV_SETUP_WRITE);
#endif

void ADC_init(void) {
    DDRC &= ~_BV(PC0);
    //ADC in 10bits : used in IR sensor routine : without shift left or right
    ADC_REFERENCE_AREF();           //ADC reference in 5V
    ADC_CLOCK_PRESCALER_128();
    /* the original ADC frequency of this project was 125KHz (Prescaler = 128), thus, I changed it to sample faster, in 1MHz (Prescaler16)
     * I have some loss in precision, working in 10 bits with a frequency bigger than 200KHz, but in this case this do not matters
     * */
    ADC_ENABLE();
    ADC_SELECT_CHANNEL_0();
    ADC_DIGITAL_INPUT_0_DISABLE();
    ADC_START_CONVERSION();              //I discard the first sample, which takes 25 clock cycles
    ADC_WAIT_CONVERSION_FINISH();
}

void LCD_init(void) {
    lcdInit();
}

/*************************************************************************
Function: uartInit
Purpose: initialize uart in 9600 8-N-1 standard and sets serial to stdout
 **************************************************************************/
#ifdef debug
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

void thr_lcd(void * args) {
    int * arg = (int*)args;
    static int i = 0;
    //se quiser usar o valor de a tem que deferenciar ex. *a
    while (1) {
#ifdef debug
        printf("Thread %d: running...\n", getid());
        printf("Thread arg= %d\n", *arg);
        printf("Iteration number= %d\n", teste);
#endif
        LCD_CLEAR_SCREEN();
        LCD_CURSOR_MOVE_FIRST_LINE();
        for (i=0;i<10;i++) {
            buffer[i]='0';
        }
        snprintf(buffer, 10, "%d", leitura_adc);
        lcd_debug(buffer);  
        _delay_ms(500);
        //*arg -= 1;
        //if (*arg == 0) break;
        /*free cpu to run another thread with yield method()*/
        yield();
    }
    finish(*arg);
}

void thr_adc(void * args) {
    int * arg = (int*)args;
    //se quiser usar o valor de a tem que deferenciar ex. *a
    while (1) {
#ifdef debug
        printf("Thread %d: running...\n", getid());
        printf("Thread arg= %d\n", *arg);
        printf("Iteration number= %d\n", teste);
#endif
        ADC_START_CONVERSION();
        ADC_WAIT_CONVERSION_FINISH();
        leitura_adc = ADC;          //read the entire ADC
        //*arg -= 1;
        //if (*arg == 0) break;
        /*free cpu to run another thread with yield method()*/
        yield();
    }
    finish(*arg);
}


int main(void) {
    LCD_init();
    LCD_STDIO();
    LCD_CLEAR_SCREEN();
    LCD_CURSOR_MOVE_FIRST_LINE();
    _delay_ms(500);
    printf("LCD\n");
    _delay_ms(500);
    ADC_init();

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
    int tid_a = create(&thr_adc, THREAD_PRIORITY_NORMAL, MINIMAL_STACK_SIZE, &arg_a);
#ifdef debug
    printf("Thread A created\n");
#endif
    int tid_b = create(&thr_lcd, THREAD_PRIORITY_NORMAL, MINIMAL_STACK_SIZE, &arg_b);
#ifdef debug    
    printf("Thread B created\n");
#endif
//    int tid_c = create(&thr_run, THREAD_PRIORITY_ABOVE_NORMAL, MINIMAL_STACK_SIZE, &arg_c);
#ifdef debug
    printf("Thread C created\n");
#endif
    LCD_CLEAR_SCREEN();
    LCD_CURSOR_MOVE_FIRST_LINE();
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



