/*	Author: christopherluu
 *  Partner(s) Name: 
 *	Lab Section: 22
 *	Assignment: Lab #11  Exercise #2
 *	Exercise Description: [optional - include for your own benefit]
 *
 *	I acknowledge all content contained herein, excluding template or example
 *	code, is my own original work.
 *
 *	Demo Video: https://youtu.be/IN0cZB0fPHo
 */
#include <avr/io.h>
#include "io.h"
#include "timer.h"
#include "keypad.h"
#include "scheduler.h"

#ifdef _SIMULATE_
#include "simAVRHeader.h"
#endif

unsigned char message[53] = "                CS120B is Legend... wait for it DARY!";
unsigned int idx = 0;
unsigned char display[16];
enum LCD_STATES{LCD_START, LCD_INIT} lcd_state;
int LCDTick(int state){
    switch(state){
        case LCD_START:
            state = LCD_INIT;
            break;
        case LCD_INIT:
            state = LCD_INIT;
            break;
        default:
            state = LCD_START;
            break;
    }

    switch(state){
        case LCD_START:
            break;
        case LCD_INIT:
            for(int i = 0; i < 16; i++){
                display[i] = message[(i + idx) % 53];
            }
            idx++;
            idx = (idx == 54) ? 0 : idx;
            LCD_DisplayString(1, display);
            break;
        default:
            break;
    }
    return state;
}

int main(void) {
    /* Insert DDR and PORT initializations */
    unsigned char x;
    DDRA = 0xFF; PORTA = 0x00;
    DDRB = 0xFF; PORTB = 0x00;
    DDRC = 0xF0; PORTC = 0x0F;
    DDRD = 0xFF; PORTD = 0x00;
    /* Insert your solution below */
    LCD_init();

    static task task1;
    task *tasks[] = {&task1};
    const unsigned short numTasks = sizeof(tasks)/sizeof(task*);
    const char start = 0;
    
    task1.state = start;
    task1.period = 30;
    task1.elapsedTime = task1.period;
    task1.TickFct = &LCDTick;

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
