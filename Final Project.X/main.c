/* 
 * File:   Final Prject main.c
 * Author: lexan
 *
 * Created on April 18, 2023, 1:52 PM
 */

// PIC16F1829 Configuration Bit Settings

// 'C' source line config statements

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
#pragma config LVP = ON         // Low-Voltage Programming Enable (Low-voltage programming enabled)

// #pragma config statements should precede project file includes.
// Use project enums instead of #define for ON and OFF.

#include <xc.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include  "i2c.h"
#define I2C_SLAVE 0x27	/* was 1E Channel of i2c slave depends on soldering on back of board*/
#define _XTAL_FREQ 8000000   /*for 8mhz*/

//Prototypes
void I2C_LCD_Command(unsigned char,unsigned char);
void I2C_LCD_SWrite(unsigned char,unsigned char *, char);
void I2C_LCD_Init(unsigned char);
void I2C_LCD_Pos(unsigned char,unsigned char);
unsigned char I2C_LCD_Busy(unsigned char);

void ADC_Initialize()
{
    ADCON0 = 0b00101101;
    ADCON1 = 0b11000000;
}

unsigned int ADC_Read()
{
    ADCON0 = 0b00101101;

    __delay_ms(2);
    GO_nDONE = 1;
    while(GO_nDONE);
    return ((ADRESH<<8)+ADRESL);
}

void main(void) {
    
    TMR1H = TMR1L = 0; // Set initial value of timers

    unsigned int adc_value = 0;
    
    TRISA = 0x00; // Clear TRISA
    
    TRISA = ((1u << 1u)); // Sets RA1 as an Input for the ECHO pin on proximity sensor
    
    TRISC = 0x00;
    
    ADC_Initialize();
  
    //Variables
	unsigned char  Sout[20];
	unsigned char * Sptr;
    int z;
	Sptr = Sout;

	i2c_Init();				// Start I2C as Master 100KH
	I2C_LCD_Init(I2C_SLAVE); //pass I2C_SLAVE to the init function to create an instance
    
    I2C_LCD_Command(I2C_SLAVE, 0x01);

    sprintf(Sout, "Hello World");

    I2C_LCD_SWrite(I2C_SLAVE, Sout, 11);
    
    __delay_ms(50);
    
    I2C_LCD_Command(I2C_SLAVE, 0x01);
    sprintf(Sout, "");
    
    RA4 = 0;
    
    while(1) {
        
        adc_value = ADC_Read();
                
        
        if (adc_value < 712) RA4 = 1;
        else RA4 = 0;
        
        RC0 = 1;               //TRIGGER HIGH
        __delay_us(10);               //10uS Delay
        RC0 = 0;               //TRIGGER LOW
        int a = 0;
        while(!RA1);         //Waiting for Echo
        TMR1ON = 1;            //Timer Starts
        while(RA1);            //Waiting for Echo goes LOW
        TMR1ON = 0;            //Timer Stops

        a = (TMR1L | (TMR1H<<8));   //Reads Timer Value
        a = a/58.82;                //Converts Time to Distance
        a = a + 1;
        
        sprintf(Sout, "LightValue = %d", adc_value);
        
        I2C_LCD_SWrite(I2C_SLAVE, Sout, 16);
        
        I2C_LCD_Pos(I2C_SLAVE, 0x40); //Set Position to start bottom line
        
        sprintf(Sout, "DistValue = %d", a);
        
        I2C_LCD_SWrite(I2C_SLAVE, Sout, 16);

                
        __delay_ms(50);
        
        I2C_LCD_Command(I2C_SLAVE, 0x01);
  
    }
 
}



