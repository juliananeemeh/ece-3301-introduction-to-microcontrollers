#include <stdio.h>
#include <stdlib.h>
#include <xc.h>
#include <math.h>
#include <p18f4620.h> 

#pragma config OSC = INTIO67
#pragma config WDT = OFF
#pragma config LVP = OFF
#pragma config BOREN = OFF


/***********************************************************/
/*                     Main Function                       */
/***********************************************************/
void main()
{
char in_sw;                                                         //Use variable in_sw as char
    TRISA = 0xFF;                                                   //set port A as an input
    TRISB = 0x00;                                                   //set port B as an output
    ADCON1 = 0x0F;                                                  //Set the ADCON1 to this value to force the pins to be in digital mode
    while (1)
    {
        in_sw = PORTA;                                              //read data from PORTA and save it
        in_sw = in_sw & 0x07;                                       //Mask out the unused upper four bits while preserving the lower 3-bits
        PORTB = in_sw;                                              //Output the data to PORTB
    }
}
