				; THIS FIRST ASSEMBLY LANGUAGE PROGRAM WILL FLASH LEDS
				; CONNECTED TO THE PINS 0 THROUGH 3 OF PORT B
#include<P18F4620.inc>
config OSC = INTIO67
config WDT = OFF
config LVP = OFF
config BOREN = OFF

; CONSTANT DECLARATIONS
				
Delay1 equ 0xFF			; correct delay values
Delay2 equ 0XF5
Counter_L equ 0x20		; counter starting addresses
Counter_H equ 0x21
ORG 0x0000
				; CODE STARTS FROM THE NEXT LINE
START:
    MOVLW 0x0F 			; set ADCON1 = 0x0F & pins are in digital mode
    MOVWF ADCON1 
    MOVLW 0x00			; set TRISB = 0x00 so it is an output
    MOVWF TRISB 

MAIN_LOOP:
    MOVLW 0x05			; move literal to W
    MOVWF PORTB			; move contents of W to PORTB
   
    CALL DELAY_ONE_SEC		; delay one sec
    MOVLW 0x0A			; move literal to W
    MOVWF PORTB			; move contents of W to PORTB
    CALL DELAY_ONE_SEC		; delay one sec
    GOTO MAIN_LOOP		; repeat main loop


; DELAY FUNCTION

DELAY_ONE_SEC:			; the routine below is a subroutine, and it is called ?DELAY_ONE_SEC?.
    MOVLW Delay1		; move value of Delay1 to W
    MOVWF Counter_H		; move contents of W to Counter_H
LOOP_OUTER:
    NOP 
    MOVLW Delay2		; move value of Delay2 to W
    MOVWF Counter_L		; move contents of W to Counter_L
LOOP_INNER:
    NOP 
    DECF Counter_L,F		; decrement Counter_L by 1
    BNZ LOOP_INNER		; check if counter at 0 if not continue loop
    DECF Counter_H,F		; decrement Counter_H by 1
    BNZ LOOP_OUTER		; check if counter at 0 if not continue loop
    RETURN
				
 END 



