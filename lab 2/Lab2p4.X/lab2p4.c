#include <stdio.h>
#include <stdlib.h>
#include <xc.h>
#include <math.h>
#include <p18f4620.h> 

#pragma config OSC = INTIO67
#pragma config WDT = OFF
#pragma config LVP = OFF
#pragma config BOREN = OFF

#define delay 17000                                                  //define delay

/***********************************************************/
/*                     Prototype Area                      */
/***********************************************************/
void Delay_One_Sec(void);                                            //declare one second function


/***********************************************************/
/*                     Main Function                       */
/***********************************************************/
void main()
{

    TRISC = 0x00;                                                    //set port C as output
    TRISD = 0x00;                                                    //set port D as output
    ADCON1 = 0x0F;                                                   //make sure pins are in digital mode
    char array1[8] = {0x07,0x05,0x06,0x03,0x02,0x00,0x04,0x01};      //array of colors for led 1
    char array2[8] = {0x06,0x07,0x01,0x03,0x04,0x00,0x02,0x05};      //array of colors for led 2

    for(char i=0; i<8; i++)                                          //loop through colors for both leds
    {  
        PORTC = array1[i];                                           //output each color to led 1
        PORTD = (array2[i])<<5;                                      //output each color to led 2, shift left 5 to output to bits 5-7
        Delay_One_Sec();                                             //delay one second
    }   

}

/***********************************************************/
/*   Delay_One_Sec() : Function to delay one second        */
/***********************************************************/
void Delay_One_Sec() 
{
    for (int I=0; I <delay; I++);                                    //loop from 0 to defined delay above
} 
