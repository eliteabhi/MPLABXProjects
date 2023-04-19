/* 
 * File:   Try Project.c
 * Author: lexan
 *
 * Created on April 18, 2023, 5:31 PM
 */

#include <stdio.h>
#include <stdlib.h>
#define _XTAL_FREQ 8000000

#define RS RD2
#define EN RD3
#define D4 RD4
#define D5 RD5
#define D6 RD6
#define D7 RD7
#define trigger RA1
#define echo RA2

#include <xc.h>
#include "lcd.h";
#include <pic16f1829.h>

// BEGIN CONFIG
#pragma config FOSC = HS // Oscillator Selection bits (HS oscillator)
#pragma config WDTE = OFF // Watchdog Timer Enable bit (WDT enabled)
#pragma config PWRTE = OFF // Power-up Timer Enable bit (PWRT disabled)
#pragma config BOREN = ON // Brown-out Reset Enable bit (BOR enabled)
#pragma config LVP = OFF // Low-Voltage (Single-Supply) In-Circuit Serial Programming Enable bit (RB3 is digital I/O, HV on MCLR must be used for programming)
#pragma config CPD = OFF // Data EEPROM Memory Code Protection bit (Data EEPROM code protection off)
#pragma config WRT = OFF // Flash Program Memory Write Enable bits (Write protection off; all program memory may be written to by EECON control)
#pragma config CP = OFF // Flash Program Memory Code Protection bit (Code protection off)
//END CONFIG
void main()
{
  int a;

  TRISA = ((1u << 2u));        //RA2 as Input PIN (ECHO)
  TRISC = 0x00; // LCD Pins as Output

  Lcd_Init();

  Lcd_Set_Cursor(1,1);
  Lcd_Write_String("Developed By");
  Lcd_Set_Cursor(2,1);
  Lcd_Write_String("electroSome");

  __delay_ms(3000);
  Lcd_Clear();

  T1CON = 0x10;                 //Initialize Timer Module

  while(1)
  {
    TMR1H = 0;                  //Sets the Initial Value of Timer
    TMR1L = 0;                  //Sets the Initial Value of Timer

    RA1 = 1;               //TRIGGER HIGH
    __delay_us(10);               //10uS Delay
    RA1 = 0;               //TRIGGER LOW

    while(!RA2);           //Waiting for Echo
    TMR1ON = 1;               //Timer Starts
    while(RA2;            //Waiting for Echo goes LOW
    TMR1ON = 0;               //Timer Stops

    a = (TMR1L | (TMR1H<<8));   //Reads Timer Value
    a = a/58.82;                //Converts Time to Distance
    a = a + 1;
    //Distance Calibration
    if(a>=2 && a<=400)          //Check whether the result is valid or not
    {
      Lcd_Clear();
      Lcd_Set_Cursor(1,1);
      Lcd_Write_String("Distance = ");
      
      Lcd_Set_Cursor(1,14);
      Lcd_Write_Char(a%10 + 48);

      a = a/10;
      Lcd_Set_Cursor(1,13);
      Lcd_Write_Char(a%10 + 48);

      a = a/10;
      Lcd_Set_Cursor(1,12);
      Lcd_Write_Char(a%10 + 48);

      Lcd_Set_Cursor(1,15);
      Lcd_Write_String("cm");
    }
    else
    {
      Lcd_Clear();
      Lcd_Set_Cursor(1,1);
      Lcd_Write_String("Out of Range");
    }
    __delay_ms(400);
  }
}

