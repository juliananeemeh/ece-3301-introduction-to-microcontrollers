
#include <stdio.h>
#include <stdlib.h>
#include <xc.h>
#include <math.h>
#include <p18f4620.h> 


#define delay 17000


void Delay_One_Sec()
{
 for (int I=0; I <delay; I++)
     for(int i=0; i <= 7; i++)
         PORTC = i;
 } 


void main()
{
    TRISC = 0x00;
    ADCON1 = 0x0F;
    Delay_One_Sec();
}



