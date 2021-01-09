#!/bin/sh
avr-gcc -mmcu=atmega8 -I. -gdwarf-2 -std=c11 -Os -o code.o code.c
avr-objcopy -O ihex code.o code.hex
avrdude -p m8 -c usbasp -P usb -U flash:w:code.hex

#avrdude -c usbasp -p m32 -U lfuse:w:0xc3:m -U hfuse:w:0x99:m
