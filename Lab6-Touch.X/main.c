/*
 * File:   main.c
 * Author: Abhi Rangarajan
 *
 * Created on March 2, 2023, 3:34 PM
 */

/*
 Lab 6 Starter Code
 
 */

// PIC16F1829 Configuration Bit Settings
// #pragma config statements should precede project file includes.
// Use project enums instead of #define for ON and OFF.

#include <stdio.h>
#include <stdlib.h>
#include <xc.h>

// CONFIG1
#pragma config FOSC = INTOSC    // Oscillator Selection (INTOSC oscillator: I/O function on CLKIN pin)
#pragma config WDTE = OFF       // Watchdog Timer Enable (WDT disabled)
#pragma config PWRTE = OFF      // Power-up Timer Enable (PWRT disabled)
#pragma config MCLRE = ON       // MCLR Pin Function Select (MCLR/VPP pin function is MCLR)
#pragma config CP = OFF         // Flash Program Memory Code Protection (Program memory code protection is disabled)
#pragma config CPD = OFF        // Data Memory Code Protection (Data memory code protection is disabled)
#pragma config BOREN = OFF      // Brown-out Reset Enable (Brown-out Reset disabled)
#pragma config CLKOUTEN = OFF   // Clock Out Enable (CLKOUT function is disabled. I/O or oscillator function on the CLKOUT pin)
#pragma config IESO = OFF       // Internal/External Switchover (Internal/External Switchover mode is disabled)
#pragma config FCMEN = OFF      // Fail-Safe Clock Monitor Enable (Fail-Safe Clock Monitor is disabled)

// CONFIG2
#pragma config WRT = OFF        // Flash Memory Self-Write Protection (Write protection off)
#pragma config PLLEN = OFF      // PLL Enable (4x PLL disabled)
#pragma config STVREN = ON      // Stack Overflow/Underflow Reset Enable (Stack Overflow or Underflow will cause a Reset)
#pragma config BORV = LO        // Brown-out Reset Voltage Selection (Brown-out Reset Voltage (Vbor), low trip point selected.)
#pragma config LVP = ON        // Low-Voltage Programming Enable (High-voltage on MCLR/VPP must be used for programming)




/*Serial Configuration*/
#define BAUD 9600   //Bits per second transfer rate
#define FOSC 4000000L   //Frequency Oscillator
#define DIVIDER ((int)(FOSC/(16UL * BAUD) -1))  //Should be 25 for 9600/4MhZ
#define NINE_BITS 0
#define SPEED 0x4       //T Speed
#define RX_PIN TRISC5   //Recieve Pin
#define TX_PIN TRISC4   //Transmit Pin

/*Xtal Macro*/
#define _XTAL_FREQ 4000000.0    /*for 4mhz*/

//Function Prototypes
void setup_comms(void);
void putch(unsigned char);
unsigned char getch(void);
unsigned char getche(void);
void timer_config(void);
void clockAndpin_config(void);
void usartConfig(void);
void Display (int);

int main(int argc, char** argv) {
/*Variable Declaration*/  
    unsigned int Tcount, Threshold;
/*TouchPad Setup*/  
    CPSCON0 	=   0x8C;   //Set up Touch sensing module control reg 0
    CPSCON1     =   0x03;   //Channel Select

/*Clock and Pin Configuration*/
    clockAndpin_config();      //Configures clock and pins, enables timers
    
/*USART CONFIG*/
    usartConfig();
    setup_comms();
    
/*Threshold Value*/
    Threshold 	=   0x0000; //using the threshold for all 3 channels
    
/*Start Replace*/  
/*Infinite While*/
    while (1){
        timer_config(); //Configures Timer 0 and 1
        while (!TMR0IF) continue;             //wait here till Timer 0 overflows
            Tcount = (TMR1H<<8)+TMR1L;        //Save 16 bit count value in timer      
            printf("Tcount = %x, Threshold= %x \r\n", Tcount, Threshold); //Write to UART TERMINAL
            __delay_ms(200); //Delay Scroll on USART
            TMR0IF = 0; //Clear the interrupt flag for Timer 1
           if (Tcount < Threshold) //Touch Detection
               RA5 = 1; //Hello World On
           else
               RA5 = 0; //Hello World Off

    }
/*End Replace*/
 return (EXIT_SUCCESS);
}

void Display (int delay){  
    switch(delay){

//  -********put the display function here!*******************
    }

}


void clockAndpin_config(){
    OSCCON      =	0X6A;  	//set up 4MHz for Fosc
    INTCON      =   0;      // purpose of disabling the interrupts.
    OPTION_REG  =   0XC5;  	// set up timer 0, and then timer 1
    T1CON    	=	0XC1;   //TMR 1 Enable
    T1GCON   	=  	0X81;   //81 is without toggle mode A1 is toggle mode
    TRISA   	= 	0X10;
    TRISC   	=	0XFF;
    PORTA   	= 	0;
    ANSELA  	=   0X10;
}

void timer_config(){
    TMR1ON  	=   0;
    TMR0    	=   0;
    TMR1H       =   0;
    TMR1L   	=   0;
    TMR1ON      =   1;
    TMR0IF  	=   0;      	//Clear the interrupt flag for Timer 1
    TMR0    	=   0;  
}

void usartConfig(){
    APFCON0 = 0x84; //This sets pins RC5 and RC4 as RX & TX on pic16f1829
    TXCKSEL = 1;   
    RXDTSEL = 1;  //makes RC4 & 5 TX & RX for USART (Allows ICSP)
}

void setup_comms(void){

	RX_PIN = 1;
	TX_PIN = 1;
	SPBRG = DIVIDER;
	RCSTA = (NINE_BITS|0x90);
	TXSTA = (SPEED|NINE_BITS|0x20);
    	TXEN=1;
    	SYNC=0;
    	SPEN=1;
    	BRGH=1;
}

void putch(unsigned char byte)
{
	/* output one byte */
	while(!TXIF)	/* set when register is empty */
		continue;
	TXREG = byte;
}

unsigned char getch() {
	/* retrieve one byte */
	while(!RCIF)	/* set when register is not empty */
        continue;
	return RCREG;
}

unsigned char getche(void)
{
	unsigned char c;
	putch(c = getch());
	return c;
}