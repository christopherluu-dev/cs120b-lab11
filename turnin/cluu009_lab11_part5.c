/*	Author: christopherluu
 *  Partner(s) Name: 
 *	Lab Section: 22
 *	Assignment: Lab #11  Exercise #5
 *	Exercise Description: [optional - include for your own benefit]
 *
 *	I acknowledge all content contained herein, excluding template or example
 *	code, is my own original work.
 *
 *	Demo Video: https://youtu.be/054Bu5JSVuQ 
 */
#include <avr/io.h>
#include "io.h"
#include "timer.h"
#include "keypad.h"
#include "scheduler.h"

#ifdef _SIMULATE_
#include "simAVRHeader.h"
#endif

unsigned int playerPos = 2;
unsigned char press = 0x00;
unsigned char play_pause = 1;
enum BUTTON_STATES{START, INIT, UP, DOWN, RESET_PAUSE, RELEASE} button_state;
int buttonTick(int state){
    press = (~PINB & 0x07);
    switch (state){
        case START:
            state = INIT;
            break;
        case INIT:
            if(press == 4){
                state = UP;
            }
            else if(press == 2){
                state = DOWN;
            }
            else if(press == 1){
                state = RESET_PAUSE;
            }
            else{
                state = INIT;
            }
            break;
        case UP:
            state = RELEASE;
            break;
        case DOWN:
            state = RELEASE;
            break;
        case RESET_PAUSE:
            state = RELEASE;
            break;
        case RELEASE:
            if(press == 0){
                state = INIT;
            }
            else{
                state = RELEASE;
            }
            break;
        default:
            state = START;
            break;
    }
    
    switch(state){
        case START:
            break;
        case INIT:
            break;
        case UP:
            playerPos = (playerPos == 18) ? 2 : 18;
            break;
        case DOWN:
            playerPos = (playerPos == 2) ? 18 : 2;
            break;
        case RESET_PAUSE:
            play_pause = (play_pause == 1) ? 0 : 1;
            break;
        case RELEASE:
            break;
        default:
            break;
    }
    return state;
}

unsigned char obstacles[] = "           #       #             ";
unsigned char lose_message[] = "Game Over";
unsigned int text_tracker = 0;
enum LCD_STATES{LCD_START, LCD_INIT, LCD_CHANGE, LCD_PAUSE, LCD_LOSE} lcd_state;
int LCDTick(int state){
    switch(state){
        case LCD_START:
            state = LCD_INIT;
            break;
        case LCD_INIT:
            if(press == 1){
                state = LCD_PAUSE;
            }
            else if(obstacles[playerPos + text_tracker] == '#'){
                state = LCD_LOSE;
            }
            else{
                state = LCD_INIT;
            }
            break;
        case LCD_PAUSE:
            if(play_pause == 0){
                state = LCD_PAUSE;
            }
            else{
                play_pause = 1;
                state = LCD_INIT;
            }
            break;
        case LCD_LOSE:
            if(press == 1){
                state = LCD_INIT;
                play_pause = 1;
                text_tracker = 0;
                playerPos = 2;
            }
            else {
                state = LCD_LOSE;    
            }
            break;
        default:
            state = LCD_START;
            break;
    }

    switch(state){
        case LCD_START:
            break;
        case LCD_INIT:
            for(unsigned int i = 1; i < 32; i++){
                LCD_Cursor(i);
                LCD_WriteData(obstacles[(i + text_tracker) % 32]);
            }
            LCD_Cursor(playerPos);
            LCD_WriteData('*');
            text_tracker = (text_tracker >= 32) ? 0 : ++text_tracker;
            break;
        case LCD_PAUSE:
            break;
        case LCD_LOSE:
            for(unsigned int i = 1; i < sizeof(lose_message); i++){
                LCD_Cursor(i);
                LCD_WriteData(lose_message[i - 1]);
            }
            playerPos = 2;
            break;
        default:
            break;
    }
    return state;
}

int main(void) {
    /* Insert DDR and PORT initializations */
    DDRA = 0xFF; PORTA = 0x00;
    DDRB = 0x00; PORTB = 0xFF;
    DDRC = 0xFF; PORTC = 0x00;
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
    task1.TickFct = &buttonTick;

    task2.state = start;
    task2.period = 100;
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
