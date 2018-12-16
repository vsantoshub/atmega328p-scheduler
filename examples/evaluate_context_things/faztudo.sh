#!/bin/bash
avr-gcc -Os -DF_CPU=16000000 -g -Wall -mmcu=atmega328p evaluate_context_things.c ulthread.c uart.c -o atmega_scheduler
avr-objcopy -j .text -j .data -O ihex atmega_scheduler atmega_scheduler.hex
sudo avrdude -P /dev/ttyACM0 -b 115200 -pm328p -c AVRISP -F -e -U flash:w:atmega_scheduler.hex
