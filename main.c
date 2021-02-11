/***************
 * CLASS : EGR 436 EMBEDDED SYSTEMS INTERFACE
 * PROFESSORS : JIAO AND BRAKORA
 * DATE : 2/8/21
 * LAB GROUP MEMBERS: DAKOTA CULBERTSON  ANDREW KORNAKI  CONNOR WEBSTER
 * TITLE: LAB 1 UART BLINKY BOI
 * DESCRIPTION: THIS CODE RECIEVES A CHARACTER FROM A PC USING UART COMMUNICATION AND
 *              CHANGES THE BLINK RATE OF AN ONBOARD LED. THE NEW BLINK RATE THAT IS SET
 *              IS SENT BACK TO THE PC.
 */


#include "msp.h"
#include <stdio.h>
#include "Serial.h"
#include "hardware.h"

void initialize_SysTick();


int UART_read;      //used to check if there has been a UART communication
float bpm = 60.0;   //blink rate of the LED
float divide = 0;   //variable that is used to create the LED blink period
float bps;          //holds the conversion to beats per second
float spb = 1;      //holds the conversion to seconds per beat



int main(){
    //setup green on board LED
    P2->SEL0 &= ~BIT1;
    P2->SEL1 &= ~BIT1;
    P2->DIR  |=  BIT1;
    P2->OUT  &= ~BIT1;

    __enable_interrupt();
    __enable_irq();
    setupSerial();
    initialize_SysTick();
    while(1){
        UART_read = check_read();
        if(UART_read){
            bpm = readBuffer(); //read in the value in the buffer to change the blink rate
            writeOutput(bpm);   //send back the value that was read in the buffer
            bps = bpm/60.0;     //convert the value to beats per second
            spb = 1/bps;        //convert the value to seconds per beat
            //printf("\n new tempo is %f \n",bpm);  //uncomment this line if the debugger is connected and you want to see the value
        }
    }
}

void initialize_SysTick(){
    SysTick->CTRL = 0;              //disable SysTick during configuration
    SysTick->LOAD = 30000;          //load in a value that will cause an interrupt every 1/100 second
    SysTick->VAL = 0;               //any value written to VAL resets the counter
    SysTick->CTRL = 7;              //enable SysTick with interrupt
    NVIC_EnableIRQ(SysTick_IRQn);   //enable the interrupt
}

void SysTick_Handler(){
        if((divide * .01) >= spb){  //the divide value is compared to the the flash period. divide is multiplied by .01 to convert to a seconds value
            P2->OUT |= BIT1;        //the green led is turned on when the divided value is equal to the period
            divide = 0;             //divide is set to zero to restart the time division process
        }
        if(divide == 2){            //an arbitrarily small value is set to turn off the LED creating a short blink.
            P2->OUT &=~ BIT1;
        }
        divide = divide + 1;        //increment the divide value on every interrupt

}

