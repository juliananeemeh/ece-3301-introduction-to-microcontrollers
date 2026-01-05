				; THIS SECOND ASSEMBLY LANGUAGE PROGRAM WILL READ THE VALUES OF
				; ALL THE BITS 0-2 OF PORT A AND OUTPUT THEM
				; TO THE PINS 0 THROUGH 2 OF PORT B
#include <P18F4620.inc>
 config OSC = INTIO67
 config WDT = OFF
 config LVP = OFF
 config BOREN = OFF
 
 ORG 0x0000
START:
    MOVLW 0x0F			; move 0x0F to W
    MOVWF ADCON1		; move W to ADCON1 ; pins in digital mode
    MOVLW 0xFF			; move 0xFF to W
    MOVWF TRISA			; move W to TRISA ; inputs
    MOVLW 0x00			; move 0x00 to W
    MOVWF TRISC			; move W to TRISC ; outputs
 
MAIN_LOOP: 
    MOVF PORTA, W		; move PORTA to W
    ANDLW 0x07			; AND W with 0x07 to mask upper 5 bits
    MOVWF PORTC			; move W to PORT C
    GOTO MAIN_LOOP		; repeat main loop
 END 



