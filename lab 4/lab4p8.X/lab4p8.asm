#include<P18F4620.inc>
config OSC = INTIO67
config WDT = OFF
config LVP = OFF
config BOREN = OFF
    
InA equ 0x20			; declare as memory location
InB equ 0x21
Result equ 0x22

ORG 0x0000

START:
    
MOVLW 0x0F
MOVWF ADCON1			; pins in digital mode
    
MOVLW 0xFF			; inputs
MOVWF TRISA
MOVWF TRISB
    
MOVLW 0x00			; outputs
MOVWF TRISC
MOVWF TRISD

MOVLW 0x07			; input only bit 0-2
MOVWF TRISE 

MAIN_LOOP:
    BTFSC PORTE, 2		; skip next line if PORT E bit 2 is clear (0)
    GOTO PORTE_bit2_EQ1		; go to this if port e bit 2 is 1
    GOTO PORTE_bit2_EQ0		; go to this if port e bit 2 is 0
    
   PORTE_bit2_EQ1:
    BTFSC PORTE, 1
    GOTO PORTE_bit21_EQ11	; go to this if port e bit 21 = 11
    GOTO PORTE_bit21_EQ10	; go to this if port e bit 21 = 10
    
   PORTE_bit2_EQ0:
    BTFSC PORTE, 1
    GOTO PORTE_bit21_EQ01	; go to this if port e bit 21 = 01
    GOTO PORTE_bit21_EQ00	; go to this if port e bit 21 = 00
    
   PORTE_bit21_EQ11:
    GOTO PERFORM_TASK_BCD	; if port e bit 21 = 11, perform task bcd
    
   PORTE_bit21_EQ10:
    GOTO PERFORM_TASK_XOR	; if port e bit 21 = 10, perform task xor
    
   PORTE_bit21_EQ01:
    BTFSC PORTE, 0
    GOTO PERFORM_TASK_AND	; if port e = 011, perform task and
    GOTO PERFORM_TASK_OR	; if port e = 010, perform task or
    
   PORTE_bit21_EQ00:
    BTFSC PORTE, 0     
    GOTO PERFORM_TASK_ADD	; if port e = 001, perform task add
    GOTO PERFORM_TASK_COMP	; if port e = 000, perform task add
    
   PERFORM_TASK_COMP:
    BCF PORTD, 2		; This is to clear the Blue LED of the RGB
    BCF PORTD, 1		; This is to clear the Green LED of the RGB
    BCF PORTD, 0 		; This is to clear the Red LED of the RGB
    CALL SUB_COMP               ; call comp subroutine
    GOTO MAIN_LOOP		; go back to main loop
    
   PERFORM_TASK_ADD:
    BCF PORTD, 2		; clear bit 2 of port d
    BCF PORTD, 1		; clear bit 1 of port d
    BSF PORTD, 0		; set bit 0 of port d
    CALL SUB_ADD		; call add subroutine
    GOTO MAIN_LOOP		; go back to main loop
    
   PERFORM_TASK_OR:
    BCF PORTD, 2		; clear bit 2 of port d
    BSF PORTD, 1		; set bit 1 of port d
    BCF PORTD, 0		; clear bit 0 of port d
    CALL SUB_OR			; call or subroutine
    GOTO MAIN_LOOP		; go back to main loop
    
   PERFORM_TASK_AND:
    BCF PORTD, 2		; clear bit 2 of port d
    BSF PORTD, 1		; set bit 1 of port d
    BSF PORTD, 0		; set bit 0 of port d
    CALL SUB_AND		; call and subroutine
    GOTO MAIN_LOOP		; go back to main loop
   
   PERFORM_TASK_XOR:
    BSF PORTD, 2		; set bit 2 of port d
    BCF PORTD, 1		; clear bit 1 of port d
    BCF PORTD, 0		; clear bit 0 of port d
    CALL SUB_XOR		; call xor subroutine
    GOTO MAIN_LOOP		; go back to main loop
    
   PERFORM_TASK_BCD:
    BSF PORTD, 2                ; set bit 2 of port d
    BSF PORTD, 1		; set bit 1 of port d
    BCF PORTD, 0		; clear bit 0 of port d
    CALL SUB_BCD		; call bcd subroutine
    GOTO MAIN_LOOP		; go back to main loop

; 1's complement subroutine
    
SUB_COMP:		
    MOVF PORTA,			; read content of PORTA and store in W
    ANDLW 0x0F			; and W with 0x0F to mask upper 4 bits
    MOVWF InA			; move W to InA
    COMF InA, W			; 1's complement InA store in W
    ANDLW 0x0F			; and W with 0x0F to mask upper 4 bits
    MOVWF Result		; move W to Result
    MOVFF Result,PORTC		; copy Result to PORTC
    BCF PORTC,5			; set PORTC bit 5 to 0
    BNZ Z_CHECK_DONE1		; if Z flag not set, branch to Z_CHECK_DONE1
    BSF PORTC,5			; set PORTC bit 5 to 1

    Z_CHECK_DONE1:		; return back to main
	RETURN

; add subroutine
    
SUB_ADD:
    MOVF PORTA, W		; read content of PORTA and store in W
    ANDLW 0x0F			; and W with 0x0F to mask upper 4 bits
    MOVWF InA			; move W to InA  
    MOVF PORTB, W		; read content of PORTB and store in W
    ANDLW 0x0F			; and W with 0x0F to mask upper 4 bits
    MOVWF InB			; move W to InB    
    ADDWF InA,W			; W = W+ InA = InB + InA
    MOVWF Result		; move W to Result
    MOVFF Result,PORTC		; copy Result to PORTC
    BCF PORTC,5			; set PORTC bit 5 to 0
    BNZ Z_CHECK_DONE2		; if Z flag not set, branch to Z_CHECK_DONE2
    BSF PORTC,5			; set PORTC bit 5 to 1

    Z_CHECK_DONE2:
	RETURN			; return back to main

; or subroutine
    
SUB_OR:
    MOVF PORTA, W		; read content of PORTA and store in W
    ANDLW 0x0F			; and W with 0x0F to mask upper 4 bits
    MOVWF InA			; move W to InA
    MOVF PORTB, W		; read content of PORTB and store in W
    ANDLW 0x0F			; and W with 0x0F to mask upper 4 bits
    MOVWF InB			; move W to InB
    IORWF InA,W			; W = InA | InB
    MOVWF Result		; move W to Result
    MOVFF Result,PORTC		; copy Result to PORTC
    BCF PORTC,5			; set PORTC bit 5 to 0
    BNZ Z_CHECK_DONE3		; if Z flag not set, branch to Z_CHECK_DONE1
    BSF PORTC,5			; set PORTC bit 5 to 1

    Z_CHECK_DONE3:
	RETURN			; return back to main

; and subroutine
    
SUB_AND:
    MOVF PORTA, W		; read content of PORTA and store in W
    ANDLW 0x0F			; and W with 0x0F to mask upper 4 bits
    MOVWF InA			; move W to InA
    MOVF PORTB, W		; read content of PORTB and store in W
    ANDLW 0x0F			; and W with 0x0F to mask upper 4 bits
    MOVWF InB			; move W to InB
    ANDWF InA,W			; W = InA & InB
    MOVWF Result		; move W to Result
    MOVFF Result,PORTC		; copy Result to PORTC
    BCF PORTC,5			; set PORTC bit 5 to 0
    BNZ Z_CHECK_DONE4		; if Z flag not set, branch to Z_CHECK_DONE4
    BSF PORTC,5			; set PORTC bit 5 to 1

    Z_CHECK_DONE4:
	RETURN			; return back to main

; xor subroutine
    
SUB_XOR:
    MOVF PORTA, W		; read content of PORTA and store in W
    ANDLW 0x0F			; and W with 0x0F to mask upper 4 bits
    MOVWF InA			; move W to InA
    MOVF PORTB, W		; read content of PORTB and store in W
    ANDLW 0x0F			; and W with 0x0F to mask upper 4 bits
    MOVWF InB			; move W to InB
    XORWF InA,W			; W = InA ^ InB
    MOVWF Result		; move W to Result
    MOVFF Result,PORTC		; copy Result to PORTC
    BCF PORTC,5			; set PORTC bit 5 to 0
    BNZ Z_CHECK_DONE5		; if Z flag not set, branch to Z_CHECK_DONE5
    BSF PORTC,5			; set PORTC bit 5 to 1

    Z_CHECK_DONE5:
	RETURN			; return back to main
    
; bcd subroutine
    
SUB_BCD:
    MOVF PORTA, W		; read content of PORTA and store in W
    ANDLW 0x0F			; and W with 0x0F to mask upper 4 bits
    MOVWF InA			; move W to InA
    MOVLW 0x09			; move 0x09 into W
    CPFSGT InA,1		; compare InA to W value 
    GOTO NGREATER		; if InA <=9, go to NGREATER
    GOTO GREATER		; if InA > 9, go to GREATER

    GREATER:
	MOVLW 0x06		; move 0x06 to W
	ADDWF InA, W		; add InA to W
	GOTO SKIP_ADD		; go to Skip ADD

    NGREATER:
	MOVF InA, W		; move InA to W

    SKIP_ADD:
	MOVWF Result		; move W to Result
	MOVFF Result,PORTC	; copy Result to PORTC

	BCF PORTC,5		; set PORTC bit 5 to 0
	BNZ Z_CHECK_DONE6	; if Z flag not set, branch to Z_CHECK_DONE6
	BSF PORTC,5		; set PORTC bit 5 to 1

    Z_CHECK_DONE6:
	RETURN			; return back to main
END


