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
#define _XTAL_FREQ 4000000.0    /*for 4mhz*/
#define LED RA4

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
	
        OSCCON = 0x00;
    unsigned int adc_value = 0;
    
    TRISA = 0x00;
    TRISC = 0x00;
      
    TRISC2 = 1;
    ANSELC = 0x00;
    
    TRISC3 = 1;
    ANSELC = ((1u << 3u) | ANSELC);
    
    ADC_Initialize();
  
  do
  {
      if (!RB7) TRISA5 = ~TRISA5;
      
      adc_value = ADC_Read(0);
      __delay_ms(50);

  }while(1); //Infinite Loop
  
    //Variables
	unsigned char  Sout[16];
	unsigned char * Sptr;
    int z;
	Sptr = Sout;

	i2c_Init();				// Start I2C as Master 100KH
	I2C_LCD_Init(I2C_SLAVE); //pass I2C_SLAVE to the init function to create an instance
	
    //Final Project 
    I2C_LCD_Command(I2C_SLAVE, 0x01);
    while(1) {
        for (int i = 0; i < 1000; i++){
            I2C_LCD_Command(I2C_SLAVE, 0x1C);
        __delay_ms(200);
        sprintf(Sout, "On/High");
        LED = 1;
        }
        for (int i = 0; i < 1000; i++){
            I2C_LCD_Command(I2C_SLAVE, 0x18);// unsure
            __delay_ms(200);
            sprintf(Sout, "Low/Off");
             LED = 0;      
        }
    }
 
}



