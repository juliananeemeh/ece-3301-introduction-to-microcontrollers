
#include <stdio.h>
#include <stdlib.h>
#include <xc.h>
#include <math.h>
#include <p18f4620.h> 

#pragma config OSC = INTIO67
#pragma config WDT = OFF
#pragma config LVP = OFF
#pragma config BOREN = OFF

#define delay 17000                                                 //define delay

/***********************************************************/
/*                     Prototype Area                      */
/***********************************************************/
void Delay_One_Sec(void);                                           //declare one second function


/***********************************************************/
/*                     Main Function                       */
/***********************************************************/
void main()
{
    TRISC = 0x00;                                                   //set port C as an output
    ADCON1 = 0x0F;                                                  //make sure pins are in digital mode
    while(1)                                                        //endless loop
    {
        for(char i=0; i <8; i++)                                    //loop through all of colors of LED (0-7)
        {     
            PORTC = i;                                              //output color to PORT C according to i
            Delay_One_Sec();                                        //delay one second to see each color
        }
    }
}


/***********************************************************/
/*   Delay_One_Sec() : Function to delay one second        */
/***********************************************************/
void Delay_One_Sec() 
{
    for (int I=0; I <delay; I++);                                   //loop from 0 to defined delay above
} 


