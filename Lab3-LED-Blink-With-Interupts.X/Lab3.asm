;NAME:
;LAB 3 BLINK WITH INTERRUPTS

; PIC16F1829 Configuration Bit Settings

; Assembly source line config statements

#include "p16f1829.inc"
;!!DEFINE MASK HERE!!
 
; CONFIG1
; __config 0xC9E4
 __CONFIG _CONFIG1, _FOSC_INTOSC & _WDTE_OFF & _PWRTE_OFF & _MCLRE_ON & _CP_OFF & _CPD_OFF & _BOREN_OFF & _CLKOUTEN_OFF & _IESO_OFF & _FCMEN_OFF
; CONFIG2
; __config 0xDEFF
 __CONFIG _CONFIG2, _WRT_OFF & _PLLEN_OFF & _STVREN_ON & _BORV_LO & _LVP_ON


 
    list p=16F1829, R=DEC

    CBLOCK 0x30 ; Define GPR variable register locations
VarA
VarB
    ENDC
  
    ORG 0
    goto Start   

    #define mask (1 << 2) ; Doesn't have to be here but it MUST be defined before 'movlw mask' ~ Abhi
    
    ORG 4
	BANKSEL INTCON
	btfss INTCON, 2
	RESET
	bcf INTCON, 2
	BANKSEL LATA
	movlw mask
	
	; My Code
	xorwf LATA, mask ; XORS with mask ~ Abhi
	
    RETFIE
    
    
Start
;Clock Setup
    BANKSEL	OSCCON 	
    movlw   	0x5A ; Made it go slower ~ Abhi   	 	
    movwf  	OSCCON
;Input Output Setup
    ;Configure all of TRISA to be output
    BANKSEL 	TRISA  	 	
    clrf    	TRISA
    ;Set all LATCH A outputs to 0
    BANKSEL	LATA
    clrf	LATA
    ;Configure Push Button as Input
    BANKSEL	TRISB
    bsf		TRISB, 7
    ;Configure Push Input as Digital
    BANKSEL	ANSELB
    bcf		ANSELB, 7
    ;Configure Option Registor for TMR0
    BANKSEL	OPTION_REG
    movlw	0x87
    movwf	OPTION_REG
    
    ;Interrupt Control Register Setup
    BANKSEL INTCON
    bcf INTCON, 2 ; Clears TMR0 flag
    bcf INTCON, 7 ; Set GIE bit on INTCON
    bcf INTCON, 5 ; set TMR0IE bit on INTCON
    
    ; Turn off green LED ~ Abhi
    BANKSEL LATA
    bsf LATA, 5
   
    ;Execution Loop
Loop
    BANKSEL PORTB
    btfss   PORTB, 7
    bra     Loop
    BANKSEL LATA		
    bsf	    LATA, 2 ; Changed to RA2 ~ Abhi
    call Delay
    BANKSEL LATA
    bcf	    LATA, 2 ; Changed to RA2 ~ Abhi
    call Delay
    goto Loop
    
    ;16 bit variable based delay
Delay
    BANKSEL VarA
    movlw   0xFF
    movwf   VarA
Outside
    decfsz  VarA
    goto    Goon
    return      
Goon
    movlw   0xFF
    movwf   VarB
Inside
    decfsz  VarB
    goto    Inside
    goto    Outside
    
    end