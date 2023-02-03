; LAB 1: Led Blink with Delay Loop
; Abhi Rangarajan uxs876
; Dr. Morton
    
; PIC16F1829 Configuration Bit Settings

; Assembly source line config statements

#include "p16f1829.inc"

; CONFIG1
; __config 0x9E4
 __CONFIG _CONFIG1, _FOSC_INTOSC & _WDTE_OFF & _PWRTE_OFF & _MCLRE_ON & _CP_OFF & _CPD_OFF & _BOREN_OFF & _CLKOUTEN_OFF & _IESO_OFF & _FCMEN_OFF
; CONFIG2
; __config 0x1EFF
 __CONFIG _CONFIG2, _WRT_OFF & _PLLEN_OFF & _STVREN_ON & _BORV_LO & _LVP_ON
 
 
; PROCCESSOR AND RADIX
list p = 16F1829,  R = DEC
 
; DECLARE VARIABLES
CBLOCK 0x30
    varA
    varB
ENDC

; START SETUP
ORG 0
goto Start
    
ORG 4
RETFIE

Start
    
    ; CLOCK SETUP
    BANKSEL OSCCON
    movlw 0x5A	    ; I made it go slower so you can see it blink more easily
    movwf OSCCON

    ; PIN CONFIGURATION
    BANKSEL TRISA
    clrf TRISA
    BANKSEL LATA
    clrf LATA
    
    BANKSEL TRISB ; For Button
    bsf TRISB, 7
    BANKSEL ANSELC ; Read analog select bits
    bcf ANSELC, 7  ; Clear bit 2
    
    BANKSEL OPTION_REG
    movlw 0x87
    movwf OPTION_REG
    
    ; My Code ---------
	
    Loop
    
	BANKSEL LATA   ; Selects LATCH A Register
	bsf LATA, 5    ; Sets Bit 5 (RA5) to High (1)
	BANKSEL PORTB  ; Selects PORTB
	btfss PORTB, 7 ; If RB7 is set, execute next intruction
	bra Loop       ; Go back to Loop
        call Delay2    ; Calls Delay2 Function
        BANKSEL LATA   ; Selects LATCH A Register again 
        bcf LATA, 5    ; Clears Bit 5 (RA5) to Low (0)
	BANKSEL PORTB  ; Selects PORTB
	btfss PORTB, 7 ; If RB7 is set, execute next instruction
	bra Loop       ; Go back to Loop
        call Delay2    ; Calls Delay2
	
    goto Loop
	
    ; -----------------
	
    ; DELAY ROUTINE
    Delay
        BANKSEL varA
        movlw 0xFF
        movwf varA

    Outside
        decfsz varA
        goto Goon
        return

    Goon
        movlw 0xFF
        movwf varB

    Inside
        decfsz varB
        goto Inside
        goto Outside
    
	
    Delay2
	BANKSEL INTCON
	bcf INTCON, 2
	BANKSEL TMR0
	movlw 0x01
	movwf TMR0
	BANKSEL INTCON
    TLoop
	btfsc INTCON, 2
	return
	goto TLoop
END
    