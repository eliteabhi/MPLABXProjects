;NAME: Abhi Rangarajan
;LAB 4A POT LED in ASM

; PIC16F1829 Configuration Bit Settings

; Assembly source line config statements

#include "p16f1829.inc"

; CONFIG1
; __config 0x9E4
 __CONFIG _CONFIG1, _FOSC_INTOSC & _WDTE_OFF & _PWRTE_OFF & _MCLRE_ON & _CP_OFF & _CPD_OFF & _BOREN_OFF & _CLKOUTEN_OFF & _IESO_OFF & _FCMEN_OFF
; CONFIG2
; __config 0x3EFF
 __CONFIG _CONFIG2, _WRT_OFF & _PLLEN_OFF & _STVREN_ON & _BORV_LO & _LVP_ON
 
    list p=16F1829, R=DEC

    CBLOCK 0x20 ; Define GPR variable register locations
VarA
VarB
    ENDC
  
    res_vect code 0x000
    goto Start
    main_prog code

Start
 
    Call ADC_Initilize
    Call PWM_Initilize
 
;Clock Setup
    BANKSEL	OSCCON 	
    movlw   	0x6A   	 	
    movwf  	OSCCON
    
    Wait1
	BANKSEL  PIR1
	BTFSS    PIR1, 1
	GOTO     Wait1
	BANKSEL  TRISA
	BCF  TRISA, 5   ;Starts the PWM working
	
;Input Output Setup
    ;Execution Loop
    Loop
	call ADC_Read
	call PWM_Duty
	call Delay
	goto Loop
	
    PWM_Initilize
	BANKSEL APFCON1
	BSF APFCON1, 0
	BANKSEL TRISA
	BSF  TRISA,5 ;This is a safety move, we will turn on later
	BANKSEL PR2
	MOVLW 0XFF
	MOVWF PR2  ;This will give about 1KHz as a pulse frequency
	BANKSEL CCP2CON ;Most of the CCP2 registers are in the same bank 
	MOVLW 0X0C
	MOVWF CCP2CON
	BANKSEL CCPR2L
	CLRF  CCPR2L  ;Set initial duty cycle to 0= ?Green LED full ON?
	BANKSEL CCPTMRS
	CLRF  CCPTMRS ;Select timer 2 for all ccp modules
	BANKSEL PIR2
	BCF  PIR2, 1
	BANKSEL T2CON 
	MOVLW b'00000101'  ;1:1 postscaler, 4:1 prescaler and TMR2 turned on
	MOVWF T2CON
	RETURN
	
    ADC_Initilize
	Banksel  ADCON0    ;Note  ADCON0 and ADCON1 in same bank
	MOVLW 0X2D          ;Set channel to AN11/RB5, turn on ADC 
	MOVWF ADCON0
	MOVLW 0X40       ;Left justified, Fosc/4 and Vss and Vdd as ref
	MOVWF ADCON1
	BANKSEL TRISB       ;Configure RB5 as analog input
	BSF  TRISB, 5
	BANKSEL ANSELB
	BSF  ANSELB, 5
	RETURN

    ADC_Read
	BANKSEL  ADCON0
	BSF  ADCON0, 1 ;This starts a conversion process
	BTFSC  ADCON0, 1
	GOTO  $-1
	RETURN   ;Returns when done with result in ADRESH
	
    PWM_Duty
	BANKSEL ADRESH
	MOVF  ADRESH, W  ;must put in the 2nd operand of W
	BANKSEL CCPR2L
	MOVWF CCPR2L
	RETURN
    
    ;16 bit variable based delay
Delay
    BANKSEL VarA
    movlw   0x20
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
   