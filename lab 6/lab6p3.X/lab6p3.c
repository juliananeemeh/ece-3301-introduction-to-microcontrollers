#include <p18f4620.h>
#include <stdio.h>
#include <math.h>
#include <usart.h>
#pragma config OSC = INTIO67
#pragma config WDT=OFF
#pragma config LVP=OFF
#pragma config BOREN =OFF 

#define EW_RED PORTAbits.RA5
#define EW_GREEN PORTCbits.RC0
#define EWLT_RED PORTCbits.RC2
#define EWLT_GREEN PORTCbits.RC3
#define NS_RED PORTCbits.RC4
#define NS_GREEN PORTCbits.RC5
#define NSLT_RED PORTEbits.RE0
#define NSLT_GREEN PORTEbits.RE1

#define OFF 0
#define RED 1
#define GREEN 2
#define YELLOW 3

#define SEC_LED PORTDbits.RD7

void Set_EW(char color);
void Set_EWLT(char color);
void Set_NS(char color);
void Set_NSLT(char color);
void Wait_One_Second(void);
void Wait_Half_Second(void);
void Init_ADC(void);
void init_UART(void);
void main()
{
    TRISA = 0x1F;
    TRISB = 0x00;
    TRISC = 0x00;
    TRISD = 0x00;
    TRISE = 0x00;
    Init_ADC();
    init_UART();
    while(1)
    {
    int i;
        for(i=0;i<4;i++)
        {
            Set_EW(i);          //set color for east-west direction
            Set_EWLT(i);        //set color for the east-west left turn direction
            Set_NS(i);          //set color for the north-south direction
            Set_NSLT(i);        //set color for the north-south left turn direction
            Wait_One_Second();   //call wait one second routine to wait for 1 second
        }
    }
}
void Init_ADC()
{
    ADCON0 = 0x01;
    ADCON1 = 0x0E;
    ADCON2 = 0xA9;
}
void init_UART() 
{ 
    OpenUSART (USART_TX_INT_OFF & USART_RX_INT_OFF & 
    USART_ASYNCH_MODE & USART_EIGHT_BIT & USART_CONT_RX & 
    USART_BRGH_HIGH, 25); 
    OSCCON = 0x70; 
} 
void Set_EW(char color)
{
    switch(color)
    {
        case OFF: EW_RED = 0; EW_GREEN=0;break;         //turn off the EW LED
        case RED: EW_RED = 1; EW_GREEN=0;break;         //sets EW LED RED
        case GREEN: EW_RED = 0; EW_GREEN=1;break;         //sets EW LED GREEN
        case YELLOW: EW_RED = 1; EW_GREEN=1;break;         //sets EW LED YELLOW
    }
}

void Set_EWLT(char color)
{
    switch(color)
    {
        case OFF: EWLT_RED = 0; EWLT_GREEN=0;break;         //turn off the EWLT LED
        case RED: EWLT_RED = 1; EWLT_GREEN=0;break;         //sets EWLT LED RED
        case GREEN: EWLT_RED = 0; EWLT_GREEN=1;break;         //sets EWLT LED GREEN
        case YELLOW: EWLT_RED = 1; EWLT_GREEN=1;break;         //sets EWLT LED YELLOW
    }
}

void Set_NS(char color)
{
    switch(color)
    {
        case OFF: NS_RED = 0; NS_GREEN=0;break;         //turn off the NS LED
        case RED: NS_RED = 1; NS_GREEN=0;break;         //sets NS LED RED
        case GREEN: NS_RED = 0; NS_GREEN=1;break;         //sets NS LED GREEN
        case YELLOW: NS_RED = 1; NS_GREEN=1;break;         //sets NS LED YELLOW
    }
}

void Set_NSLT(char color)
{
    switch(color)
    {
        case OFF: NSLT_RED = 0; NSLT_GREEN=0;break;         //turn off the NSLT LED
        case RED: NSLT_RED = 1; NSLT_GREEN=0;break;         //sets NSLT LED RED
        case GREEN: NSLT_RED = 0; NSLT_GREEN=1;break;         //sets NSLT LED GREEN
        case YELLOW: NSLT_RED = 1; NSLT_GREEN=1;break;         //sets NSLT LED YELLOW
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

