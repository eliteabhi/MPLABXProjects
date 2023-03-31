/*
 * File:   Sleep.c
 * Author: Paul Morton
 *
 * Created on March 9, 2020, 7:06 PM
 * 
 * This program is to develop a sleep lab.  Will explore:
 * 1.  Going to Sleep
 * 2.  Wake up with watch dog timer after 8 sec or with the PB
 * 3.  Special BG LED if wake with PB BR if wake with WDT
 * 4.  Go to sleep 2nd time with WDT turned off and only wake up on the PB
 * without GIE on using the push button as an event without interupt
 * 5.  Go to sleep 3rd time and again wake up with PB but this time have GIE=1
 * so will wake up with a full interrupt using the PB either edge 
 */

#include <xc.h>
#include <stdio.h>
#include <stdlib.h>

#pragma config FOSC = INTOSC    // Oscillator Selection (INTOSC oscillator: I/O function on CLKIN pin)
#pragma config WDTE = SWDTEN       
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
#pragma config LVP = ON         // Low-Voltage Programming Enable (Low-voltage programming enabled)

#define _XTAL_FREQ 4000000.0    /*for 4mhz*/
/*Serial Configuration*/
#define BAUD 9600   //Bits per second transfer rate
#define FOSC 4000000L   //Frequency Oscillator
#define DIVIDER ((int)(FOSC/(16UL * BAUD) -1))  //Should be 25 for 9600/4MhZ
#define NINE_BITS 0
#define SPEED 0x4       //T Speed
#define RX_PIN TRISC5   //Recieve Pin
#define TX_PIN TRISC4   //Transmit Pin

//Function Prototypes
void setup_comms(void);
void putch(unsigned char);
unsigned char getch(void);
unsigned char getche(void);
void timer_config(void);
void usartConfig(void);
void Long_White(unsigned char, unsigned char, unsigned char);

void interrupt UART_Rx (){      //This is the ****ISR*****
    //Clear the IOC RB7 flag
    IOCBF &= ~(1<<7);  //Clear the RB7 IOC flag
    LATA2 = 1;  //Turn off the blue  why is this needed?
    //after the sleep command gets executed on wake up then jump to ISR
    //Blink the Red LED 4 times fast
    LATC6 = 0;  //Turn on the Red LED    
    __delay_ms(500);
    LATC6 = 1;  //Turn off the Red LED    
    __delay_ms(500);
    LATC6 = 0;  //Turn on the Red LED    
    __delay_ms(500);
    LATC6 = 1;  //Turn off the Red LED    
    __delay_ms(500);
    LATC6 = 0;  //Turn on the Red LED    
    __delay_ms(500);
    LATC6 = 1;  //Turn off the Red LED    
    __delay_ms(500);
    LATC6 = 0;  //Turn on the Red LED    
    __delay_ms(750);
    LATC6 = 1;  //Turn off the Red LED    
    __delay_ms(750);
    printf ("In the ISR\n\r");
    return;
}



void main(void) {
    unsigned char Letter;
    
    
    OSCCON   = 0x68; /* b6..4 = 1101 = 4MHz */
	APFCON0  = 0x84;  /*makes RC4 & 5 TX & RX for USART (Allows ICSP)*/
    //Modify the Ansel and Tris registers to fit the application 
	ANSELA   = 0x00;
    ANSELB   = 0X00;
    ANSELC   = 0X00;
    TRISA    = 0x00;
    TRISB    = 0xA0;      /* set as input RB5 and RB7 pushbutton */
    TRISC    = 0x00;      /* set as output */
    CLRWDT();
    
    //Turn off all LEDs
    LATA5 = 1;  //Green
    LATA2 = 1;  //Blue
    LATC6 = 1;  //Red
    
        /*USART CONFIG*/
    usartConfig();
    setup_comms();
    
    
    //Set up Pushbutton ON RB7   Tris and Ansel set already above
    //Turn on Interrupt on change but don't set GIE bit
    IOCBF &= ~(1<<7);  //Clear the RB7 IOC flag
    IOCBN = 0X80;    //ENABLE interrupt on change on push button
    INTCON = 0x08;   // enable IOC interrupts but leave GIE =0       

    while(1) {
    //Set up watch dog timer control reg to give 8 second time out and turn on
    WDTCON = 0x1B;        
    LATA5 = 0;  // Only Green on 2 seconds for GO
    LATA2 = 1;  
    LATC6 = 1;
    __delay_ms(2000); 
    LATA5 = 1;    //Green off        
    printf("\n\r\n\r\n\rTop of while loop -- Part 1\n\r");     
    IOCBN = 0X80;    //ENABLE interrupt on change on push button neg edge
    IOCBF = 0x00;  //Clear all port B IOC flags
    printf("First Sleep -- Wake with PB or WD time out\n\r") ;
//Signal going to sleep 1    
    LATA2 = 0;  //Turn on the Blue LED = Sleep
    LATA5 = 1;
    LATC6 = 1;
    //********************************************************************
    SLEEP();    //Wake up with the push button or WD timer
    LATA2 = 1;  //Turn off the Blue LED 
//Indicate if RB7 or Watch Dog caused the Wake up    
    if(IOCBF & (1<<7)){ 
        printf("Wake up by Push Button\n\r");
//blink Blue-Green = PB;
        LATA2 = 0;
        LATA5 = 0;
        LATC6 = 1;
        __delay_ms(500);
        LATA2 = 1;
        LATA5 = 1;
        LATC6 = 1;
        __delay_ms(1000); 
    }
    else {
//Put blink Blue-Red = PB;        
        printf("Wake up by Watch Dog time out\n\r");
        LATA2 = 0;
        LATA5 = 1;
        LATC6 = 0;
        __delay_ms(1000);
        LATA2 = 1;
        LATA5 = 1;
        LATC6 = 1;
        __delay_ms(500); 
    } 
     Long_White(0 ,0 ,0);   //White blinks once
    IOCBF = 0x00;  //Clear all port B IOC flags
    
    printf("Now -- Ready for Part 2\n\r") ;  
    WDTCON &= ~(1<<0);  //Turn off the watch dog timer, won't work in sleep

    //Signal going to Sleep #2
    printf("2nd Sleep (only PB wakes)\n\r") ;
    LATA2 = 0;  //Turn on the Blue LED
    LATA5 = 1;
    LATC6 = 1;
    //********************************************************************    
    SLEEP();    //Wake up with the push button only
    LATA2 = 1;  //Turn off the Blue LED 
    printf("Second wake up\n\r") ;
    //Signal wake up #2    
    Long_White(0 ,0 ,0);
    Long_White(0 ,0 ,0);

    IOCBF = 0x00;    //Clear all Port IOC flags
    IOCBN = 0X80;    //ENABLE interrupt on change on push button Neg edge
    IOCBP = 0X80;    //ENABLE POS EDGE ALSO on button release   
    GIE=1;
    printf("3rd Sleep(PB again to ISR after 1 instruction\n\r") ;
    LATA2 = 0;  //Turn on the Blue LED
    LATA5 = 1;
    LATC6 = 1;
    //********************************************************************    
    SLEEP();    //Wake up with the push button and execute the interrupt
    LATA2 = 1;  //Turn off the Blue LED Should work but does not
    printf("Third wake up-- Interrupt routine finished\n\r") ;    
    Long_White(0 ,0 ,0);
    Long_White(0 ,0 ,0);
    Long_White(0 ,0 ,0); 
    //Now would like to use the UART to wake up but only can in sync mode
    //So, will take the receive pin of uart to another input and do IOC wake
    //will use RB5 driven by RC5  connect with a wire
    //Turn off the RB7 IOC interrupt enable    
    GIE = 0;
    IOCBF = 0;  //Clear all port B IOC flags
    IOCBP = 0X20;    //ENABLE interrupt on change on RB5 Pos edge
    IOCBN = 0X20;    //ENABLE interrupt on change on RB5 Neg edge
    INTCON = 0x08;   // enable IOC interrupts but leave GIE =0 
    do{
    printf("4th Sleep Type Char only\n\r (put cursor in putty window)\n\r") ;
    LATA2 = 0;  //Turn on the Blue LED
    LATA5 = 1;
    LATC6 = 1;
    Letter = RCREG;  //Clear the UART receive buffer first
    SLEEP();    //Wake up with the UART receive a single character
    IOCBF = 0;  //Clear all port B IOC flags    
    LATA2 = 1;  //Turn off the Blue LED 
    Letter = RCREG;
    printf("Fourth wake up\n\r") ; 
    printf ("Char sent was  %c  \n\r\n\r\n\r",Letter);  //print the char
    }while(Letter != 'P'); //Challenge code here put the test in here
    
    Long_White(0 ,0 ,0);
    Long_White(0 ,0 ,0);
    Long_White(0 ,0 ,0);
    Long_White(0 ,0 ,0); 
    
    } 
        //Now turn the watch dog back on
    WDTCON |= (1<<0);
    __delay_ms(3000);   
    return;
}

void Long_White(unsigned char G, unsigned char B, unsigned char R){

    LATA5 = G;
    LATA2 = B;
    LATC6 = R;
    __delay_ms(1000);
    LATA5 = 1;
    LATA2 = 1;
    LATC6 = 1;
    __delay_ms(1000);    
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