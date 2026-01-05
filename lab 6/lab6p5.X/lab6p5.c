#include <p18f4620.h>
#include <stdio.h>
#include <math.h>
#include <usart.h>
#pragma config OSC = INTIO67
#pragma config WDT=OFF
#pragma config LVP=OFF
#pragma config BOREN =OFF 
                                                                                        // sec led bit definition
#define SEC_LED PORTDbits.RD7
                                                                                        // led color bit definitions
#define EW_RED PORTAbits.RA5
#define EW_GREEN PORTCbits.RC0
#define EWLT_RED PORTCbits.RC2
#define EWLT_GREEN PORTCbits.RC3
#define NS_RED PORTCbits.RC4
#define NS_GREEN PORTCbits.RC5
#define NSLT_RED PORTEbits.RE0
#define NSLT_GREEN PORTEbits.RE1
                                                                                        // color definitions
#define OFF 0
#define RED 1
#define GREEN 2
#define YELLOW 3
                                                                                        // switch bit definitions
#define NSPED_SW PORTAbits.RA2
#define NSLT_SW PORTAbits.RA1
#define EWPED_SW PORTAbits.RA3
#define EWLT_SW PORTAbits.RA4

#define MODE_LED PORTEbits.RE2
                                                                                        // function prototypes
void PED_Control(char Direction, char Num_Sec);
void Deactivate_Buzzer();
void Activate_Buzzer();
void Wait_One_Second_With_Beep();
void Wait_Half_Second();
void Wait_One_Second(void);
void Wait_N_Seconds(char seconds);
void Set_EW(char color);
void Set_EWLT(char color);
void Set_NS(char color);
void Set_NSLT(char color);
void Day_Mode();
void Night_Mode();
unsigned int get_full_ADC(void);
void init_UART(void); 
void Init_ADC(void);
void select_ADC_CH(char ch);

char LED_7seg[10] = { 0x40, 0x79, 0x24, 0x30, 0x19, 0x12, 0x02, 0x78, 0x00, 0x10};      // seven segment display hex values

void main()
{                                                                                       // define ports as inputs and outputs
    TRISA = 0x1F;
    TRISB = 0x00;
    TRISC = 0x00;
    TRISD = 0x00;
    TRISE = 0x00;
                        
    int MODE;                                                                           // initialize mode variable
    init_UART();                                                                        // call UART function for tera term
    Init_ADC();                                                                         // call Init ADC function
    while(1)
    {
        select_ADC_CH(0);                                                              // select ADC channel 0
        //read voltage and set = to mode
        int STEP2 = get_full_ADC(); //read AN4 ADC result                              // get steps from full ADC function
        float voltage2 = STEP2 * 5 / 1025.0; // each step = 4mV                        // calculate voltage from steps
        printf ("voltage photo resistor = %f \r\n", voltage2);                         // print voltages on tera term
        float MODE = voltage2 < 2.5 ? 1:0;                                             // if voltage < 2.5 mode = 1, else mode = 0
        
        
        if(MODE)
        {
            Day_Mode();                                                                // if mode = 1 --> day mode
            MODE_LED == 1;                                                             // mode led ON
        }
        else
        {
            Night_Mode();                                                              // if mode = 0 --> night mode
            MODE_LED == 0;                                                             // mode led OFF
        }
    }
}
void select_ADC_CH(char ch)
{
    ADCON0 = ch*4 + 1;                                                                // select ADC channel
}
void Init_ADC()
{
    ADCON0 = 0x01;                                                                    // initialize ACON0 1 and 2
    ADCON1 = 0x0E;
    ADCON2 = 0xA9;
}
void putch (char c)
{
    while (!TRMT);                                                                    // function to print values on teraterm
    TXREG = c;
} 

void init_UART()                                                    
{ 
    OpenUSART (USART_TX_INT_OFF & USART_RX_INT_OFF &                                 // function to print values on teraterm
    USART_ASYNCH_MODE & USART_EIGHT_BIT & USART_CONT_RX & 
    USART_BRGH_HIGH, 25); 
    OSCCON = 0x70; 
} 
unsigned int get_full_ADC(void)
{
 int result;                                                                        // function to get ADC step values
    ADCON0bits.GO=1; // Start Conversion
    while(ADCON0bits.DONE==1); // wait for conversion to be completed
    result = (ADRESH * 0x100) + ADRESL; // combine result of upper byte and
    //lower byte into result
     return result; // return the result.
} 

void Day_Mode()
{
    PORTEbits.RE2 = 1;                                                              // mode led ON
    Set_NS(RED);                                                                    // set NS red
    Set_NSLT(RED);                                                                  // set NSLT red
    Set_EWLT(RED);                                                                  // set EWLT red
    Set_EW(GREEN);                                                                  // set EW green
    if(EWPED_SW == 1)
    {
        PED_Control(0, 8);                                                          // if EWPED switch = 1, EW PED countdown from 7 to off
    }
    Wait_N_Seconds(6);                                                              // wait 6 seconds
    Set_EW(YELLOW);                                                                 // set EW yellow
    Wait_N_Seconds(2);                                                              // wait 2 seconds
    Set_EW(RED);                                                                    // set EW red
    if(NSLT_SW == 1)                                                                // if NSLT switch = 1, do the following
    {
        Set_NSLT(GREEN);                                                            // set NSLT green
        Wait_N_Seconds(7);                                                          // wait 7 seconds
        Set_NSLT(YELLOW);                                                           // set NSLT yellow
        Wait_N_Seconds(2);                                                          // wait 2 seconds
        Set_NSLT(RED);                                                              // set NSLT red
    }
    Set_NS(GREEN);                                                                  // set NS green
    if(NSPED_SW == 1)                                   
    {
        PED_Control(1,7);                                                           // if NSPED switch = 1, NS PED countdown from 6 to off
    }
    Set_NS(GREEN);                                                                  // set NS green
    Wait_N_Seconds(5);                                                              // wait 5 seconds
    Set_NS(YELLOW);                                                                 // set NS yellow
    Wait_N_Seconds(2);                                                              // wait 2 seconds
    Set_NS(RED);                                                                    // set NS red
    if(EWLT_SW == 1)                                                                // if EWLT switch = 1, do the following
    {
        Set_EWLT(GREEN);                                                            // set EWLT green
        Wait_N_Seconds(6);                                                          // wait 6 seconds
        Set_EWLT(YELLOW);                                                           // set EWLT yellow
        Wait_N_Seconds(2);                                                          // wait 2 seconds
        Set_EWLT(RED);                                                              // set EWLT red
    }   
}

void Night_Mode()
{
    PORTB = 0xFF;                                                                   // 7 seg OFF
    PORTD = 0xFF;                                                                   // other 7 seg OFF
    PORTEbits.RE2 = 0;                                                              // MODE LED OFF
    Set_NS(RED);                                                                    // set NS red
    Set_NSLT(RED);                                                                  // set NSLT red
    Set_EWLT(RED);                                                                  // set EWLT red
    Set_EW(GREEN);                                                                  // set EW green
    Wait_N_Seconds(6);                                                              // wait 6 seconds
    Set_EW(YELLOW);                                                                 // set EW yellow
    Wait_N_Seconds(2);                                                              // wait 2 seconds
    Set_EW(RED);                                                                    // set EW red
    if(NSLT_SW == 1)                                                                // if NSLT switch = 1, do the following
    {
        Set_NSLT(GREEN);                                                            // set NSLT green
        Wait_N_Seconds(5);                                                          // wait 5 seconds
        Set_NSLT(YELLOW);                                                           // set NSLT yellow
        Wait_N_Seconds(2);                                                          // wait 2 seconds
        Set_NSLT(RED);                                                              // set NSLT red
    }
    Set_NS(GREEN);                                                                  // set NS green
    Wait_N_Seconds(7);                                                              // wait 7 seconds
    Set_NS(YELLOW);                                                                 // set NS yellow
    Wait_N_Seconds(2);                                                              // wait 2 seconds
    Set_NS(RED);                                                                    // set NS red
    if(EWLT_SW == 1)                                                                // if EWLT switch = 1, do the following
    {
        Set_EWLT(GREEN);                                                            // set EWLT green
        Wait_N_Seconds(5);                                                          // wait 5 seconds
        Set_EWLT(YELLOW);                                                           // set EWLT yellow
        Wait_N_Seconds(2);                                                          // wait 2 seconds
        Set_EWLT(RED);                                                              // set EWLT red
    }
}

void Set_EW(char color)                                                             // function to set EW led color
{
    switch(color)
    {
        case OFF: EW_RED = 0; EW_GREEN=0;break;                                    // turn off the EW LED
        case RED: EW_RED = 1; EW_GREEN=0;break;                                    // sets EW LED RED
        case GREEN: EW_RED = 0; EW_GREEN=1;break;                                  // sets EW LED GREEN
        case YELLOW: EW_RED = 1; EW_GREEN=1;break;                                 // sets EW LED YELLOW
    }
}

void Set_EWLT(char color)                                                          // function to set EWLT led color
{
    switch(color)
    {
        case OFF: EWLT_RED = 0; EWLT_GREEN=0;break;                                // turn off the EWLT LED
        case RED: EWLT_RED = 1; EWLT_GREEN=0;break;                                // sets EWLT LED RED
        case GREEN: EWLT_RED = 0; EWLT_GREEN=1;break;                              // sets EWLT LED GREEN
        case YELLOW: EWLT_RED = 1; EWLT_GREEN=1;break;                             // sets EWLT LED YELLOW
    }
}

void Set_NS(char color)                                                           // function to set NS led color
{
    switch(color)
    {
        case OFF: NS_RED = 0; NS_GREEN=0;break;                                   // turn off the NS LED
        case RED: NS_RED = 1; NS_GREEN=0;break;                                   // sets NS LED RED
        case GREEN: NS_RED = 0; NS_GREEN=1;break;                                 // sets NS LED GREEN
        case YELLOW: NS_RED = 1; NS_GREEN=1;break;                                // sets NS LED YELLOW
    }
}

void Set_NSLT(char color)                                                         // function to set NSLT led color
{
    switch(color)
    {
        case OFF: NSLT_RED = 0; NSLT_GREEN=0;break;                               // turn off the NSLT LED
        case RED: NSLT_RED = 1; NSLT_GREEN=0;break;                               // sets NSLT LED RED
        case GREEN: NSLT_RED = 0; NSLT_GREEN=1;break;                             // sets NSLT LED GREEN
        case YELLOW: NSLT_RED = 1; NSLT_GREEN=1;break;                            // sets NSLT LED YELLOW
    }
}

void PED_Control(char Direction, char Num_Sec)                                    // function to control 7 seg for pedestrian crossing
{
    char count;                                                                   // define variable count
    int i;                                                                        // define variable i for loop
        PORTB = 0xFF;                                                             // PORTB 7 seg OFF
        PORTD = 0xFF;                                                             // PORTD 7 seg OFF
    if (Direction == 0)                                                           // if direction = 0, EW 7 seg
    {    
        count = Num_Sec - 1;                                                      // count is input parameter - 1
        PORTD = 0xFF;                                                             // turn off lower digit                                     
        for(i=count; i>0; i--)                                                    // loop through number of seconds
        {
            PORTB = LED_7seg[i];                                                  // output to PORTB from 7 seg array
            Wait_One_Second_With_Beep();                                          // wait one second with buzzer beep
        }
        PORTB = 0xFF;                                                             // PORTB 7 seg off
        Wait_One_Second_With_Beep();                                              // wait one second with buzzer beep
    }
    else                                                                          // NS 7 seg
    {
        count = Num_Sec - 1;                                                      
        PORTB = 0xFF;                                                             // turn off upper digit
        for(i=count; i>0; i--)                                                    // loop through number of seconds
        {
            PORTD = LED_7seg[i];                                                  // output to PORTB from 7 seg array
            Wait_One_Second_With_Beep();                                          // wait one second with buzzer beep
        }
        PORTD = 0xFF;                                                             // PORTD 7 seg off
        Wait_One_Second_With_Beep();                                              // wait one second with buzzer beep
    }

}

void Wait_One_Second_With_Beep()                                                  // function to wait one second with buzzer beep
{
    SEC_LED = 1;                                                                  // turn on SEC LED
    Activate_Buzzer();                                                            // activate buzzer
    Wait_Half_Second();                                                           // wait half second
    SEC_LED = 0;                                                                  // turn off SEC_LED
    Deactivate_Buzzer();                                                          // deactivate buzzer
    Wait_Half_Second();                                                           // wait half second
}

void Wait_Half_Second()                                                           // function to wait half second
{
    T0CON = 0x03;                                                                 // timer 0, 16-bit mode, prescaler 1:16
    TMR0L = 0x01;                                                                 // set the lower byte of TMR
    TMR0H = 0x0A;                                                                 // set the upper byte of TMR
    INTCONbits.TMR0IF = 0;                                                        // clear the Timer 0 flag
    T0CONbits.TMR0ON = 1;                                                         // turn on the Timer 0
    while(INTCONbits.TMR0IF == 0);                                                // wait for the Timer Flag to be 1 for done
    T0CONbits.TMR0ON =0;                                                          // turn off the Timer 0
}

void Activate_Buzzer()                                                            // function to activate buzzer
{
    PR2 = 0b11111001;
    T2CON = 0b00000101;
    CCPR2L = 0b01001010;
    CCP2CON = 0b00111100;
}

void Deactivate_Buzzer()                                                          // function to de-activate buzzer
{
    CCP2CON = 0x00;
    PORTCbits.RC1 = 0;
    
}

void Wait_One_Second()                                                            // function to wait one second
{
    SEC_LED =1;                                                                   // turn off the SEC LED
    Wait_Half_Second();                                                           // wait for half second 
    SEC_LED = 0;                                                                  // turn off the SEC LED
    Wait_Half_Second();                                                           // wait for half second 
}

void Wait_N_Seconds(char seconds)                                                 // function to wait N seconds
{ 
char I;                                                                           // define character I
    for (I=0; I<seconds; I++)                                                     // loop from 0 til seconds input
    {
        Wait_One_Second();                                                        // wait one second
    }
}