#include <p18f4620.h>
#include <stdio.h>
#include <math.h>
//#include <usart.h>
#pragma config OSC = INTIO67
#pragma config WDT=OFF
#pragma config LVP=OFF
#pragma config BOREN =OFF 

char LED_7seg[10] = { 0x40, 0x79, 0x24, 0x30, 0x19, 0x12, 0x02, 0x78, 0x00, 0x10};
void WAIT_ONE_SEC()
{
    for(int i=0;i<18000;i++);
}

void main(void)
{
    ADCON1=0x1A; // select pins AN0 through AN3 as analog signal, VDD-VSS as  0000 1011
    TRISA = 0x1F;
    TRISB = 0x00;
    TRISC = 0x00;
    TRISD = 0x00;
    TRISE = 0x00;
    
    while (1)
    {
        for (char i=0; i<10;i++)
        {
            PORTD = LED_7seg[i];
            PORTB = LED_7seg[i];
            WAIT_ONE_SEC();
        }
    }
}

