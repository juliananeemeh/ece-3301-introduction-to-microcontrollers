#include <P18F4620.inc>
 config OSC = INTIO67
 config WDT = OFF
 config LVP = OFF
 config BOREN = OFF
 
 ; CONSTANT DECLARATIONS
 
 Counter_L equ 0x20		; set Counter L starting address
 Counter_H equ 0x21		; set Counter H starting address
 
 Color_Value equ 0x28		; set color value starting address
 Loop_Count equ 0x29		; set loop count starting address
 
 Delay1 equ 0xFF		; set delays equal to 0xFF
 Delay2 equ 0xFF
 ORG 0x0000
 
				
START:
 ORG 0x0000
START:
    MOVLW 0x0F			; move 0x0F into W
    MOVWF ADCON1		; move contents of W into ADCON1 ; pins in digital mode
    MOVLW 0x00			; move 0x00 into W
    MOVWF TRISC			; move contents of W into TRISC ; outputs

WHILE_LOOP:			; continuous loop
    MOVLW 0x00			; move 0x00 into W  
    MOVWF Color_Value		; move W into Color Value
    MOVLW 0x08			; move 0x08 into W
    MOVWF Loop_Count		; move W into Loop Count

FOR_LOOP:
    MOVF Color_Value,W		; move Color Value into W
    MOVWF PORTC			; move W into PortC
    CALL DELAY_ONE_SEC		; one second delay
    INCF Color_Value,F		; increment color value by 1
    DECF Loop_Count,F		; decrement loop count by 1
    BNZ FOR_LOOP		; if loop count not zero repeat for loop

    GOTO WHILE_LOOP		; go back to while loop

; DELAY FUNCTION				
				
DELAY_ONE_SEC:
    MOVLW Delay1		; move Delay1 into W
    MOVWF Counter_H		; move contents of W into Counter_H
    LOOP_OUTER:
	NOP	
	MOVLW Delay2		; move Delay2 into W
	MOVWF Counter_L		; move contents of W into Counter_L
    LOOP_INNER:
	NOP 
	DECF Counter_L,F	; decrement Counter_L by 1
	BNZ LOOP_INNER		; if counter not at zero go back to loop inner
	DECF Counter_H,F	; decrement Counter_H by 1
	BNZ LOOP_OUTER		; if Counter_H not at zero go back to loop outer
	RETURN
				
 END


