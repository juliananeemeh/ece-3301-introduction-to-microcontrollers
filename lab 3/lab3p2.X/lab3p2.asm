
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
    MOVLW 0x0F			; move literal to W
    MOVWF ADCON1		; move contents of W to ADCON1 ; pins are in digital mode
    MOVLW 0xFF			; move literal to W 
    MOVWF TRISA			; move contents of W to TRISA  ; 0xFF so all ports of A are 1/inputs
    MOVLW 0x00			; move literal to W
    MOVWF TRISB			; move contents of W to TRISB ; 0x00 so all ports of B are 0/outputs
 
MAIN_LOOP: 
    MOVF PORTA, W		; move contents of PORTA to W
    ANDLW 0x07			; AND W with 0x07 to mask upper 5 bits
    MOVWF PORTB			; move contents of W to PORTB
    GOTO MAIN_LOOP		; repeat main loop
 END 



