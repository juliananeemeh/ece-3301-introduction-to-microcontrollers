#include <P18F4620.inc>
 config OSC = INTIO67
 config WDT = OFF
 config LVP = OFF
 config BOREN = OFF
 
 ; CONSTANT DECLARATIONS
 
 Counter_L equ 0x20			    ; set Counter_L starting address
 Counter_H equ 0x21			    ; set Counter_H starting address
 
 Loop_Count equ 0x29			    ; set Loop_Count starting address
 Color_Value equ 0x28			    ; set Color_Value starting address
 
 Delay1 equ 0xFF			    ; set Delay1 value
 Delay2 equ 0xFF			    ; set Delay2 value
 
 START:
    ORG 0x0000

    MOVLW 0x0F				    ; set pins in digital mode
    MOVWF ADCON1 
    MOVLW 0x00 
    MOVWF TRISC				    ; set PORT C and PORT D as outputs
    MOvWF TRISD

					    ; initialize array1 for LED 1
    MOVLW 0x07				    ; white
    MOVWF 0x40
    MOVLW 0x05				    ; purple
    MOVWF 0x41
    MOVLW 0x06				    ; cyan
    MOVWF 0x42
    MOVLW 0x03				    ; yellow
    MOVWF 0x43
    MOVLW 0x02				    ; green
    MOVWF 0x44
    MOVLW 0x00				    ; no light
    MOVWF 0x45
    MOVLW 0x04				    ; blue
    MOVWF 0x46
    MOVLW 0x01				    ; red
    MOVWF 0x47

					    ; initialize array2 for LED 2
    MOVLW 0xC0				    ; cyan
    MOVWF 0x50
    MOVLW 0xE0				    ; white
    MOVWF 0x51
    MOVLW 0x20				    ; red
    MOVWF 0x52
    MOVLW 0x60				    ; yellow
    MOVWF 0x53 
    MOVLW 0x80				    ; blue
    MOVWF 0x54
    MOVLW 0x00				    ; no light
    MOVWF 0x55
    MOVLW 0x40				    ; green
    MOVWF 0x56
    MOVLW 0xA0				    ; purple
    MOVWF 0x57
 

 
 WHILE_LOOP: ;
					    ; point registers to correct addresses
    LFSR 0,0x0040
    LFSR 1,0x0050
    
    MOVLW 0x08				    ; move 0x08 into W
    MOVWF Loop_Count			    ; move W into Loop_Count for 8 different colors
 
 FOR_LOOP:
					    ; move data to W then to correct port
    MOVF INDF0,W			    ; move INDF0 to W
    MOVWF PORTC				    ; move contents of W to PORTC
    MOVF INDF1,W			    ; move INDF1 to W
    MOVWF PORTD				    ; move contents of W to PORTD
    
    CALL DELAY_ONE_SEC			    ; delay one second
    
    INCF FSR0L				    ; increase pointer by 1
    INCF FSR1L				    ; increase pointer by 1
 
    DECF Loop_Count,F			    ; decrement Loop_Count by 1
    BNZ FOR_LOOP			    ; if loop count not zero go back to for loop
     
    GOTO WHILE_LOOP			    ; go back to while loop
 

    ; DELAY FUNCTION
    
DELAY_ONE_SEC:
    MOVLW Delay1			    ; move Delay 1 to W
    MOVWF Counter_H			    ; move contents of W to Counter_H
LOOP_OUTER:
    NOP 
    MOVLW Delay2			    ; move Delay2 to W
    MOVWF Counter_L			    ; move contents of W to Counter_L
LOOP_INNER:
    NOP 
    DECF Counter_L,F			    ; decrement Counter L by 1
    BNZ LOOP_INNER			    ; if counter L not zero go back to loop inner
    DECF Counter_H,F			    ; decrement Counter H by 1
    BNZ LOOP_OUTER			    ; if counter H not zero go back to loop outer
    RETURN
					    
 END