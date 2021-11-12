/*	Author: christopherluu
 *  Partner(s) Name: 
 *	Lab Section: 22
 *	Assignment: Lab #11  Exercise #1
 *	Exercise Description: [optional - include for your own benefit]
 *
 *	I acknowledge all content contained herein, excluding template or example
 *	code, is my own original work.
 *
 *	Demo Video:
 */
#include <avr/io.h>
#include "io.h"
#include "timer.h"
#include "keypad.h"
#include "scheduler.h"

#ifdef _SIMULATE_
#include "simAVRHeader.h"
#endif

unsigned char x = 0x00;
unsigned char out = 0x00;

enum KEY_STATES{START, INIT} key_state;
int keypadTick(int state){
    x = GetKeypadKey();
    switch(state){
        case START:
            state = INIT;
            break;
        case INIT:
            state = INIT;
            break;
        default:
            state = START;
            break;
    }

    switch(state){
        case START:
            break;
        case INIT:
            switch(x){
                case '0':
                    out = 0x00;
                    break;
                case '1':
                    out = 0x01;
                    break;
                case '2':
                    out = 0x02;
                    break;
                case '3':
                    out = 0x03;
                    break;
                case '4':
                    out = 0x04;
                    break;
                case '5':
                    out = 0x05;
                    break;
                case '6':
                    out = 0x06;
                    break;
                case '7':
                    out = 0x07;
                    break;
                case '8':
                    out = 0x08;
                    break;
                case '9':
                    out = 0x09;
                    break;
                case 'A':
                    out = 0x0A;
                    break;
                case 'B':
                    out = 0x0B;
                    break;
                case 'C':
                    out = 0x0C;
                    break;
                case 'D':
                    out = 0x0D;
                    break;
                case '*':
                    out = 0x0E;
                    break;
                case '#':
                    out = 0x0F;
                    break;
                case '\0':
                    out = 0x1F;
                    break;
                default:
                    out = 0x1B;
                    break;
            }
            PORTB = out;
            break;

        default:
            break;
    }
    return state;
}

int main(void) {
    /* Insert DDR and PORT initializations */
    unsigned char x;
    DDRB = 0xFF; PORTB = 0x00;
    DDRC = 0xF0; PORTC = 0x0F;
    /* Insert your solution below */
    
    static task task1;
    task *tasks[] = {&task1};
    const unsigned short numTasks = sizeof(tasks)/sizeof(task*);
    const char start = 0;
    
    task1.state = start;
    task1.period = 50;
    task1.elapsedTime = task1.period;
    task1.TickFct = &keypadTick;

    unsigned long GCD = tasks[0]->period;
    for(int i = 0; i < numTasks; i++){
        GCD = findGCD(GCD, tasks[i]->period);
    }

    TimerSet(GCD);
    TimerOn();

    while (1) {
        for(unsigned short i = 0; i < numTasks; i++){
            if(tasks[i]->elapsedTime == tasks[i]->period){
                tasks[i]->state = tasks[i]->TickFct(tasks[i]->state);
                tasks[i]->elapsedTime = 0;
            }
            tasks[i]->elapsedTime += tasks[i]->period;
        }
        while(!TimerFlag){};
        TimerFlag = 0;
    }
    return 1;
}
