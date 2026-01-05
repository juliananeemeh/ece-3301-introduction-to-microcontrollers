#include <stdio.h>
#include <stdlib.h>
#include <xc.h>
#include <math.h>
#include <p18f4620.h> 


void main()
{
char in_sw; // Use variable ?in_sw? as char
 TRISA = 0x01; // fill out the ?? with the proper values
 TRISB = 0x00; // fill out the ?? with the proper values
 ADCON1 = 0x0F; // Set the ADCON1 to this value to force
 // the pins to be in digital mode

 while (1)
{
in_sw = PORTA; // read data from PORTA and save it
 // into ?in?
 in_sw = in_sw & 0x07;// Mask out the unused upper four bits
 // while preserving the lower 3-bits
 PORTB = in_sw; // Output the data to PORTB
}
 }
