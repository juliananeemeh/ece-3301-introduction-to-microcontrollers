#include<P18F4620.inc>
config OSC = INTIO67
config WDT = OFF
config LVP = OFF
config BOREN = OFF
    
InA equ 0x20 ; declare as memory location
InB equ 0x21
Result equ 0x22

ORG 0x0000
 
START:
    
MOVLW 0x0F
MOVWF ADCON1 ; pins in digital mode
    
MOVLW 0xFF   ; inputs
MOVWF TRISA
MOVWF TRISB
    
MOVLW 0x00   ; outputs
MOVWF TRISC
MOVWF TRISD

MOVLW 0x07   ; input only bit 0-2
MOVWF TRISE

MAIN_LOOP:
    MOVF PORTA, W ; read content of PORTA and store in W
    ANDLW 0x0F    ; and W with 0x0F to mask upper 4 bits
    MOVWF InA     ; move W to InA
    
    MOVLW 0x09    ; move 0x09 into W
    CPFSGT InA,1  ; compare inA to W value ; addition if InA > 9
    GOTO NGREATER     ; if InA<=9 skip addition
    GOTO GREATER
    
GREATER:
    MOVLW 0x06
    ADDWF InA, W
    GOTO SKIP_ADD
    
NGREATER:
    MOVF InA, W
    
SKIP_ADD:
    MOVWF Result  ; move W to Result
    MOVFF Result,PORTC ; copy Result to PORTC
    
    BCF PORTC,5 ; set PORTC bit 5 to 0
    BNZ Z_CHECK_DONE6    ; if Z flag is set, branch to LABEL1
    BSF PORTC,5 ; set PORTC bit 5 to 1

Z_CHECK_DONE6:
    GOTO MAIN_LOOP
END

















