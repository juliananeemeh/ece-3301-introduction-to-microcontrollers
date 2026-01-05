#include <p18f4620.h>
#include <stdio.h>
#include <math.h>
#include <usart.h>
#pragma config OSC = INTIO67
#pragma config WDT=OFF
#pragma config LVP=OFF
#pragma config BOREN =OFF 

#define SEC_LED PORTDbits.RD7

void Wait_One_Second(void);
void Wait_Half_Second(void);
void init_UART(void); 


void main()
{
    TRISA = 0x1F;
    TRISB = 0x00;
    TRISC = 0x00;
    TRISD = 0x00;
    TRISE = 0x00;
    init_UART();
    while(1)
    {
        Wait_One_Second();
    }
}

void Wait_One_Second()
{
    SEC_LED =1;             //First, turn off the SEC LED
    Wait_Half_Second();     //Wait for half second (or 500 msec)
    SEC_LED = 0;            //then turn off the SEC LED
    Wait_Half_Second();     //Wait for half second (or 500 msec)
}

void Wait_Half_Second()
{
    T0CON = 0x03;           //Timer 0, 16-bit mode, prescaler 1:16
    TMR0L = 0x01;           //set the lower byte of TMR
    TMR0H = 0x0A;           //set the upper byte of TMR
    INTCONbits.TMR0IF = 0;  //clear the Timer 0 flag
    T0CONbits.TMR0ON = 1;   //Turn on the Timer 0
    
    while(INTCONbits.TMR0IF == 0); //wait for the Timer Flag to be 1 for done
    T0CONbits.TMR0ON =0;           //turn off the Timer 0
}

void init_UART() 
{ 
    OpenUSART (USART_TX_INT_OFF & USART_RX_INT_OFF & 
    USART_ASYNCH_MODE & USART_EIGHT_BIT & USART_CONT_RX & 
    USART_BRGH_HIGH, 25); 
    OSCCON = 0x70; 
} 

