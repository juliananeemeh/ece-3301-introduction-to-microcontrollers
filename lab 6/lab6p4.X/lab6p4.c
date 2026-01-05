#include <p18f4620.h>
#include <stdio.h>
#include <math.h>
#include <usart.h>
#pragma config OSC = INTIO67
#pragma config WDT=OFF
#pragma config LVP=OFF
#pragma config BOREN =OFF 

#define SEC_LED PORTDbits.RD7

void PED_Control(char Direction, char Num_Sec);
void Deactivate_Buzzer(void);
void Activate_Buzzer(void);
void Wait_Half_Second(void);
void Wait_One_Second_With_Beep(void);
void Init_ADC(void);
void init_UART(void);
char LED_7seg[10] = { 0xFF, 0x79, 0x24, 0x30, 0x19, 0x12, 0x02, 0x78, 0x00, 0x10};
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
        PED_Control(0,8); //set direction 0 (east west)for 8 seconds
        PED_Control(1,6); //set direction 1 (north south) for 6 seconds
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
void PED_Control(char Direction, char Num_Sec)
{
    char count;
    int i;
        PORTB = 0xFF;
        PORTD = 0xFF;

    if (Direction == 0) //ew
    {    
        count = Num_Sec - 1;
        PORTD = 0xFF; // turn off lower digit
        for(i=count; i>0; i--)
        {
            PORTB = LED_7seg[i];
            Wait_One_Second_With_Beep();
        }
        PORTB = 0xFF;
        Wait_One_Second_With_Beep();
    }
    else //ns
    {
        count = Num_Sec - 1;
        PORTB = 0xFF;//turn off upper digit
        for(i=count; i>0; i--)
        {
            PORTD = LED_7seg[i];
            Wait_One_Second_With_Beep();
        }
        PORTD = 0xFF;
        Wait_One_Second_With_Beep();
    }

}

void Wait_One_Second_With_Beep()
{
    SEC_LED = 1;            //turn on SEC LED
    Activate_Buzzer();      //activate buzzer
    Wait_Half_Second();     //wait half second
    SEC_LED = 0;            //turn off SEC_LED
    Deactivate_Buzzer();    //deactivate buzzer
    Wait_Half_Second();     //wait half second
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

void Activate_Buzzer()
{
    PR2 = 0b11111001;
    T2CON = 0b00000101;
    CCPR2L = 0b01001010;
    CCP2CON = 0b00111100;
}

void Deactivate_Buzzer()
{
    CCP2CON = 0x00;
    PORTCbits.RC1 = 0;
    
}
