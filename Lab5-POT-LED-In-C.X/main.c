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

#include <stdio.h>
#include <stdlib.h>
#include <xc.h>
#define _XTAL_FREQ 4000000.0    /*for 4mhz*/



//Global Variables 
long PWM_freq = 1000;
int TMR2PRESCALE = 4;

void PWM_Initialize() {  
    APFCON1 = 0x01;  //Set PWM to alternative PIN RA5 (HELLO WORLD LED)
    PR2 = 0xFF;  //PR2 value needed for 1000Hz frequency
    CCP2CON = 0x0C; //Enable pwm mode pg.238
    CCPR2L = 0x00;  //Set initial dutycycle to 0
    CCPTMRS = 0x00; //Set TMR2 to work with PWM
    PIR1 &= 0xFF;  //Clear TMR 2 int flag
    T2CON = 0b00000101; // 1:1 postscaler 4:1 prescaler on timer 2 pg.203
}


void PWM_Duty(unsigned int duty)
{
      if(duty<1024)
  {
        // The scaling calculation is not needed because ADC is exactly 10 bits like duty cycle
        //duty = ((float)duty/1023)*(_XTAL_FREQ/(PWM_freq*TMR2PRESCALE)); 
    DC2B0 = duty & 1; //Store the 0th bit
    DC2B1 = (duty & 2)>>1; //Store the 1st bit
    CCPR2L = duty>>2;// Store the remaining 8 bit //shifted right to delete lower 2 bits
  }
}

void ADC_Initialize()
{
  ADCON0 = 0b00101101; //ADC ON and RB5/AN11
  ADCON1 = 0b11000000; // Internal reference voltage is selected,  Fosc/16 is selected 
}
unsigned int ADC_Read()
{
  ADCON0 = 0b00101101;//Read on Channel 11 RB5/AN11 for pot on analog board

  __delay_ms(2); //Acquisition time delay
  GO_nDONE = 1; //Initializes A/D Conversion
  while(GO_nDONE); //Wait for A/D Conversion to complete
  return ((ADRESH << 8u) + ADRESL); //Returns 10 Bit Result
}

void main()
{
  //Variable Declarations

    unsigned int adc_value = 0;
    
  // Tristate and ANSELx Configuration  Remember ANSELB 5 must remain in analog mode =1
  // All PWM channels should be set to output mode in the TRIS registers RA5, RA2, RC6
  // Push button must be set for input TRISB7 =1  and TRISB5=1  must be set as an input also for the 
  // Pot input on pin RB5 as an analog input AN11
    
    // PWM Channels set as Output
    TRISA = 0x00;
    TRISC = 0x00;
    
    TRISC2 = 1;
    ANSELC = 0x00;
    
    TRISC3 = 1;
    ANSELC = ((1u << 3u) | ANSELC);
    
    ADC_Initialize();
    PWM_Initialize();

  //Initialization Function Calls

  do {
      
    if (!RB7) TRISA5 = ~TRISA5;
   
    //Remaining Function Calls
    adc_value = ADC_Read();
    PWM_Duty(adc_value);
    
    __delay_ms(50);

  } while(1); //Infinite Loop
  
}
