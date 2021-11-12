/*	Author: christopherluu
 *  Partner(s) Name: 
 *	Lab Section: 22
 *	Assignment: Lab #11  Exercise #3
 *	Exercise Description: [optional - include for your own benefit]
 *
 *	I acknowledge all content contained herein, excluding template or example
 *	code, is my own original work.
 *
 *	Demo Video: https://youtu.be/v6DOh8HQBTI
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
            if(out != 0x1F){
                LCD_Cursor(1);
                LCD_WriteData(x);
            }
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

    static task task1, task2;
    task *tasks[] = {&task1, &task2};
    const unsigned short numTasks = sizeof(tasks)/sizeof(task*);
    const char start = 0;
    
    task1.state = start;
    task1.period = 10;
    task1.elapsedTime = task1.period;
    task1.TickFct = &keypadTick;

    task2.state = start;
    task2.period = 30;
    task2.elapsedTime = task2.period;
    task2.TickFct = &LCDTick;

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
