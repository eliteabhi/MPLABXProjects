/*

 * File:   lab 8 main.c

 * Author: Abhi Rangarajan

 *

 *

 */

//I2C LCD

//DVD

#include <xc.h>

#include <math.h>

#include <stdio.h>

#include <stdlib.h>

#include  "i2c.h"

#include  "i2c_LCD.h"

// PIC16F1829 Configuration Bit Settings

// 'C' source line config statements

// #pragma config statements should precede project file includes.

// Use project enums instead of #define for ON and OFF.

// CONFIG1

#pragma config FOSC = INTOSC    // Oscillator Selection (INTOSC oscillator: I/O function on CLKIN pin)

#pragma config WDTE = OFF       // Watchdog Timer Enable (WDT disabled)

#pragma config PWRTE = OFF      // Power-up Timer Enable (PWRT disabled)

#pragma config MCLRE = ON       // MCLR Pin Function Select (MCLR/VPP pin function is MCLR)

#pragma config CP = OFF         // Flash Program Memory Code Protection (Program memory code protection is disabled)

#pragma config CPD = OFF        // Data Memory Code Protection (Data memory code protection is disabled)

#pragma config BOREN = ON       // Brown-out Reset Enable (Brown-out Reset enabled)

#pragma config CLKOUTEN = OFF   // Clock Out Enable (CLKOUT function is disabled. I/O or oscillator function on the CLKOUT pin)

#pragma config IESO = OFF       // Internal/External Switchover (Internal/External Switchover mode is disabled)

#pragma config FCMEN = OFF      // Fail-Safe Clock Monitor Enable (Fail-Safe Clock Monitor is disabled)


// CONFIG2

#pragma config WRT = OFF        // Flash Memory Self-Write Protection (Write protection off)

#pragma config PLLEN = OFF      // PLL Enable (4x PLL disabled)

#pragma config STVREN = ON      // Stack Overflow/Underflow Reset Enable (Stack Overflow or Underflow will cause a Reset)

#pragma config BORV = LO        // Brown-out Reset Voltage Selection (Brown-out Reset Voltage (Vbor), low trip point selected.)

#pragma config LVP = ON        // Low-Voltage Programming Enable (High-voltage on MCLR/VPP must be used for programming)

#define I2C_SLAVE 0x27	/* was 1E Channel of i2c slave depends on soldering on back of board*/

#define _XTAL_FREQ 4000000.0    /*for 4mhz*/

//Prototypes

void I2C_LCD_Command(unsigned char, unsigned char);

void I2C_LCD_SWrite(unsigned char, unsigned char *, char);

void I2C_LCD_Init(unsigned char);

void I2C_LCD_Pos(unsigned char, unsigned char);

unsigned char I2C_LCD_Busy(unsigned char);

void main(void) {

    //Variables

    unsigned char Sout[16];

    unsigned char * Sptr;

    int z;

    Sptr = Sout;

    //Clock and Pin Configs-------------------------------------------------------------------------


    OSCCON = 0x68; /* b6..4 = 1101 = 4MHz */

    /*  ********************************I2C**************************** */

    /*  Note the I2C write to LCD uses the 8 bits of the PCF8574 chip to control the LCD  */

    /*  Connected High 4 bits are High 4 data for LCD (use in 4 bit mode)  the other 4 are     */

    /*  bit3=turn on /off bk light  bit 2= E line writes on Hi 2 Lo transition, reads Lo to Hi */

    /*  bit 2=Read write set  to 0 for write   bit 0=RS  command=0 Data=1      */

    /*  ********************************I2C**************************** */
    
    //--------------------------------------------------------------------------

    i2c_Init(); // Start I2C as Master 100KH

    I2C_LCD_Init(I2C_SLAVE); //pass I2C_SLAVE to the init function to create an instance

    //--------------------------------------------------------------------------
    
    //First Words: Let's say Hello

    I2C_LCD_Pos(I2C_SLAVE, 0x40); //Set Position to start bottom line

    sprintf(Sout, "Hello World");

    I2C_LCD_SWrite(I2C_SLAVE, Sout, 11);

    __delay_ms(1000.0);

    //--------------------------------------------------------------------------
    
    //Let's count our ABC's and ASCII's

    I2C_LCD_Command(I2C_SLAVE, 0x01); //Clear display

    __delay_ms(1000.0);

    Sout[0] = 'A'; //Set start point to A

    Sout[1] = 0x00; //Set to null

    for ( z = 0; z < 100; z++ ) { //run for the next 100 ascii values

        I2C_LCD_SWrite(I2C_SLAVE, Sout, 1); //write out the ascii value to lcd

        __delay_ms(200.0);

        Sout[0] += 1; // JUMP TO NEXT ASCII VALUE

    }

    //--------------------------------------------------------------------------
    
    //Challenge part 1

    I2C_LCD_Command(I2C_SLAVE, 0x01);

    sprintf(Sout, "Hello Abhi Rangarajan");

    I2C_LCD_SWrite(I2C_SLAVE, Sout, 17);

    __delay_ms(1000.0);

    //--------------------------------------------------------------------------
    
    //Challenge part 2

    int a = 100;

    int b = 50;

    int sum = a + b;

    printf("%d + %d = %d", a, b, sum);

    //--------------------------------------------------------------------------
    
    //Challenge part 3

    I2C_LCD_Command(I2C_SLAVE, 0x01); //Clear display

    I2C_LCD_Pos(I2C_SLAVE, 0x00);

    sprintf(Sout, "Hi Brandon");

    I2C_LCD_SWrite(I2C_SLAVE, Sout, 10);

    I2C_LCD_Pos(I2C_SLAVE, 0x40);

    sprintf(Sout, "Hi DV");

    I2C_LCD_SWrite(I2C_SLAVE, Sout, 5);

    while (1) {

        for ( int i = 0; i < 15; i++ ) {

            I2C_LCD_Command(I2C_SLAVE, 0x1C);

            __delay_ms(200);

        }

        for ( int i = 0; i < 15; i++ ) {

            I2C_LCD_Command(I2C_SLAVE, 0x18);

            __delay_ms(200);

        }

    }

}
