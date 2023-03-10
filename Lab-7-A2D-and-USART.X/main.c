// 'C' source line config statements  
// #pragma config statements should precede project file includes.  
// Use project enums instead of #define for ON and OFF.  
// CONFIG1  
#pragma config FOSC = INTOSC // Oscillator Selection (INTOSCoscillator: I/O function on CLKIN pin)  
#pragma config WDTE = OFF // Watchdog Timer Enable (WDT disabled)  
#pragma config PWRTE = OFF // Power-up Timer Enable (PWRT disabled)  
#pragma config MCLRE = ON // MCLR Pin Function Select (MCLR/VPP pinfunction is MCLR)  
#pragma config CP = OFF // Flash Program Memory Code Protection(Program memory code protection is disabled)  
#pragma config CPD = OFF // Data Memory Code Protection (Datamemory code protection is disabled)  
#pragma config BOREN = OFF // Brown-out Reset Enable (Brown-out Resetdisabled)  
#pragma config CLKOUTEN = OFF // Clock Out Enable (CLKOUT function isdisabled. I/O or oscillator function on the CLKOUT pin)  
#pragma config IESO = OFF // Internal/External Switchover(Internal/External Switchover mode is disabled)  
#pragma config FCMEN = OFF // Fail-Safe Clock Monitor Enable (FailSafeClock Monitor is disabled)  
// CONFIG2  
#pragma config WRT = OFF // Flash Memory Self-Write Protection(Write protection off)  
#pragma config PLLEN = OFF // PLL Enable (4x PLL disabled)  
#pragma config STVREN = ON // Stack Overflow/Underflow Reset Enable(Stack Overflow or Underflow will cause a Reset)  
#pragma config BORV = LO // Brown-out Reset Voltage Selection(Brown-out Reset Voltage (Vbor), low trip point selected.)  
#pragma config LVP = ON // Low-Voltage Programming Enable (Highvoltageon MCLR/VPP must be used for programming)  
  
//These are the values to set baud rate for UART and configure some functions
#define BAUD 9600  
#define FOSC 4000000L  
#define DIVIDER ((int)(FOSC/(16UL * BAUD) -1)) // Should be 25 for9600/4MhZ  
#define NINE_BITS 0  
#define SPEED 0x4  
#define RX_PIN TRISC5  
#define TX_PIN TRISC4  
#define _XTAL_FREQ 4000000.0 /*for 4mhz*/  

#include <xc.h>  
#include <stdio.h>  
#include <stdlib.h> 
  
//*Function Prototypes*//  
void RA5Blink(void);  
void pinConfig(void);  
void setup_comms(void);  
void putch(unsigned char);  
unsigned char getch(void);  
unsigned char getche(void);  
  
//*Global Vars*//  
unsigned int Pval, Temp, FVval;  
double Ctemp;  
  
void main(void) {  
  
 pinConfig();  
  
setup_comms(); // set up the USART - settings defined in usart.h  
// Set up for the Temp Sensor  and FVR
FVRCON = 0xA2;
 
 
// Get set up for A2D  
 ADCON1 = 0xC0; //Right justify and Fosc/4 and Vss and Vdd references  
  
 while (1)  {
    
    /*Pot Read  it is on Chanel AN11  Thus the value in the ADCON0 reg = 0x2D */   
       ADCON0 = 0x2D; // set up for the pot input channel AN11
       __delay_ms(10); //Allow cap to recharge  
       ADGO = 1; // initiate conversion on the selected channel  
       while(ADGO)continue;  
       Pval = (unsigned)((ADRESH<<8)+(ADRESL)); //Store 10 bits into Pval, 8 + 2

    /*Fixed Voltage Reference Read*/     
       ADCON0 = 0x7D; // set up for the FVR input channel = 11111  
       __delay_ms(10); //Allow cap to recharge  
       ADGO = 1; // initiate conversion on the selected channel  
       while(ADGO)continue;  
       FVval = ((ADRESH<<8)+(ADRESL)); //Store 10 bits into Pval, 8 + 2
    
    /*Internal Temperature Read*/
       ADCON0 = 0x75; // set up for the temp input channel = 11101  
       __delay_ms(10); //Allow cap to recharge  
       ADGO = 1; // initiate conversion on the selected channel  
       while(ADGO)continue;  
       Temp = ((ADRESH<<8)+(ADRESL)); //Store 10 bits into Pval, 8 + 2

         
    /*Write to Terminal + Temperature Math*/   
       Ctemp = ((0.659-((1.48)*(1-((float)Temp/1023))))/0.00132)-40;  
       printf("Raw Temp value is %d RAW | Pot value is %d | Fixed Voltage is %d \n\r", Temp, Pval, FVval);  
       __delay_ms(1000);  
         
    /*Blink RA5 once every transmission packet*/     
       RA5Blink();  
   }  
 return;  
  }  

void setup_comms(void)  {  
RX_PIN = 1;  
TX_PIN = 1;  
SPBRG = DIVIDER;  
RCSTA = (NINE_BITS | 0x90);  
TXSTA = (SPEED | NINE_BITS | 0x20);  
TXEN = 1;  
SYNC = 0;  
SPEN = 1;  
BRGH = 1;  
  }  

void putch(unsigned char byte)  
  {  
/* output one byte */  
while(!TXIF) /* set when register is empty */  
continue;  
TXREG = byte;  
  }  

unsigned char getch()  
  {  
/* retrieve one byte */  
while(!RCIF) /* set when register is not empty */  
continue;  
return RCREG;  
  }  

unsigned char getche(void)  
  {  
unsigned char c;  
putch(c = getch());  
return c;  
  }  
  
void pinConfig(void)  {  
 OSCCON = 0x6A; /* b6..4 = 1101 = 4MHz */  
 TXCKSEL = 1; // both bits in APFCON0 MUST BE 1 for 1829 0 for 1825  
 RXDTSEL = 1; /* makes RC4 & 5 TX & RX for USART (Allows ICSP)*/  
  
 TRISA = 0x10;  
 ANSELA =0x10;  
 TRISC = 0x0C; /* set as output */  
 ANSELC =0x0C; /* all ADC pins to digital I/O */  
 TRISB = 0xF0;
 ANSELB =0xF0;
 INTCON = 0; // purpose of disabling the interrupts.  
 TSEN = 1;
 TSRNG = 0;
  }  
  
void RA5Blink(void)  {  
RA5 ^= 0x01; // Toggles the LED to help with debugging  
__delay_ms(200);   
  }  
 