#include <p18f4620.h>
#include <stdio.h>
#include <math.h>
#include <usart.h>
#pragma config OSC = INTIO67
#pragma config WDT=OFF
#pragma config LVP=OFF
#pragma config BOREN =OFF 

#define D3_RED PORTEbits.RE0                                                                // define RGB LED D3 color bits
#define D3_GREEN PORTEbits.RE1
#define D3_BLUE PORTEbits.RE2

#define D2_RED PORTCbits.RC3                                                                // define RGB LED D2 color bits
#define D2_GREEN PORTCbits.RC4
#define D2_BLUE PORTCbits.RC5

#define D1_RED PORTCbits.RC0                                                                // define RGB LED D1 color bits
#define D1_GREEN PORTCbits.RC1
#define D1_BLUE PORTCbits.RC2

                                                                                            // function prototypes 

void SET_D3_NL(void);
void SET_D3_RED(void);
void SET_D3_GREEN(void);
void SET_D3_YELLOW(void);
void SET_D3_BLUE(void);
void SET_D3_PURPLE(void);
void SET_D3_CYAN(void);
void SET_D3_WHITE(void);
void SET_D2_NL(void);
void SET_D2_RED(void);
void SET_D2_GREEN(void);
void SET_D2_YELLOW(void);
void SET_D2_BLUE(void);
void SET_D2_PURPLE(void);
void SET_D2_CYAN(void);
void SET_D2_WHITE(void);
void SET_D1_NL(void);
void SET_D1_RED(void);
void SET_D1_GREEN(void);
void SET_D1_YELLOW(void);
void SET_D1_BLUE(void);
void SET_D1_PURPLE(void);
void SET_D1_CYAN(void);
void SET_D1_WHITE(void);

void init_UART()                                                                           // UART function to print to tera term
{
    OpenUSART (USART_TX_INT_OFF & USART_RX_INT_OFF &
    USART_ASYNCH_MODE & USART_EIGHT_BIT & USART_CONT_RX &
    USART_BRGH_HIGH, 25);
    OSCCON = 0x60;                                                                         // clock 4MHz
}
void putch (char c)                                                                        // putch function also for printing to tera term
{
    while (!TRMT);
    TXREG = c;
} 

unsigned int get_full_ADC(void)                                                            // function to get ADC value
{
 int result;
    ADCON0bits.GO=1;                                                                       // Start Conversion
    while(ADCON0bits.DONE==1);                                                             // wait for conversion to be completed
    result = (ADRESH * 0x100) + ADRESL;                                                    // combine result of upper byte and
                                                                                           // lower byte into result
     return result;                                                                        // return the result.
} 

void main(void)
{
    
    ADCON1=0x1A;                                                                           // select pins AN0 through AN3 as analog signal, VDD-VSS as  0000 1011
    ADCON2=0xA9;                                                                           // Set the bit conversion time (TAD) and acquisition time                                                                                 
    TRISA = 0x29;                                                                          // define PORTS and specific bits as input or output
    TRISB = 0x00;
    TRISC = 0x00;
    TRISD = 0x00;
    TRISE = 0x00;
    init_UART();                                                                           // call function to use tera term
    
    
    while(1)
    {
        ADCON0=0x01;                                                                       // select channel AN0, and turn on the ADC subsystem, 0000 0001
 
        int num_steps = get_full_ADC();                                                    // read ADC result
        float voltage = num_steps * 4.0 - 32;                                              // convert steps to voltage in mV
        float tempC = (1035.0 - voltage) / 5.5;                                            // convert voltage to temperature in C 
        
        char TempC_int = (int)tempC;                                                       // make tempC integer
        float tempF = (tempC * 9.0 / 5.0 )+ 32.0;                                          // convert to F
        char TempF_int = (int)tempF;                                                       // make tempF integer
        char lowerDig = TempF_int % 10;                                                    // get lower dig for 7seg
        char upperDig = TempF_int / 10;                                                    // get upper dig for 7seg
        char LED_7seg[10] = { 0x40, 0x79, 0x24, 0x30, 0x19, 0x12, 0x02, 0x78, 0x00, 0x10}; // 7seg array
        PORTD = LED_7seg[upperDig];                                                        // send upper pattern to portD
        PORTB = LED_7seg[lowerDig];                                                        // send lower pattern to portE
            
        printf("Temperature (F) %f \r\n\n", tempF );                                       // print temp in F to tera term
        printf("Voltage (mV) %f \r\n", voltage);                                           // print voltage to tera term
        
                                                                                           // photo resistor code
        
        ADCON0 = 0x11;                                                                     // set ADCON0
        int STEP2 = get_full_ADC();                                                        // read AN4 ADC result
        float voltage2 = STEP2 * 4.0;                                                      // each step = 4mV
        printf ("voltage light (mV) = %f \r\n\n", voltage2);                               // print voltage to tera term
        
                                                                                          
        PORTC = TempF_int / 10;                                                            // set D1 LED color


        
        
                                                                                           // set D2 LED color
        if (TempF_int < 32)
            SET_D2_NL();
        else if (TempF_int <42 && TempF_int >=32)
            SET_D2_RED();
        else if (TempF_int <52 && TempF_int >=42)
            SET_D2_GREEN();
        else if (TempF_int <62 && TempF_int >=52)
            SET_D2_YELLOW();
        else if (TempF_int <72 && TempF_int >=62)
            SET_D2_BLUE();
        else if (TempF_int <77 && TempF_int >=72)
            SET_D2_PURPLE();
        else if (TempF_int <=80 && TempF_int >=77)
            SET_D2_CYAN();
        else
            SET_D2_WHITE();
        
        
                                                                                           // set D3 LED color
        if (voltage2 < 2.6*1000)
            SET_D3_WHITE();
        else if (voltage2 >=2.6*1000 && voltage2 < 2.9*1000)
            SET_D3_BLUE();
        else if (voltage2 >=2.9*1000 && voltage2 <3.2*1000)
            SET_D3_GREEN();
        else if (voltage2 >=3.2*1000 && voltage2 <3.6*1000)
            SET_D3_RED();
        else
            SET_D3_NL();
    }
}

                                                                                           // functions to display specific color on LED's
void SET_D3_NL()                                                                           // function to set D3 no light
{
    D3_RED = 0;
    D3_GREEN = 0;
    D3_BLUE = 0;
}
void SET_D3_RED()                                                                          // function to get D3 red
{
    D3_RED = 1;
    D3_GREEN = 0;
    D3_BLUE = 0;
}
void SET_D3_GREEN()                                                                        // function to set D3 green
{
    D3_RED = 0;
    D3_GREEN = 1;
    D3_BLUE = 0;
}
void SET_D3_YELLOW()                                                                       // function to set D3 yellow
{
    D3_RED = 1;
    D3_GREEN = 1;
    D3_BLUE = 0;
}
void SET_D3_BLUE()                                                                         // function to set D3 blue
{
    D3_RED = 0;
    D3_GREEN = 0;
    D3_BLUE = 1;
}
void SET_D3_PURPLE()                                                                       // function to set D3 purple
{
    D3_RED = 1;
    D3_GREEN = 0;
    D3_BLUE = 1;
}
void SET_D3_CYAN()                                                                         // function to set D3 cyan
{
    D3_RED = 0;
    D3_GREEN = 1;
    D3_BLUE = 1;
}
void SET_D3_WHITE()                                                                        // function to set D3 white
{
    D3_RED = 1;
    D3_GREEN = 1;
    D3_BLUE = 1;
}
void SET_D2_NL()                                                                           // function to set D2 no light
{
    D2_RED = 0;
    D2_GREEN = 0;
    D2_BLUE = 0;
}
void SET_D2_RED()                                                                          // function to set D2 red
{
    D2_RED = 1;
    D2_GREEN = 0;
    D2_BLUE = 0;
}
void SET_D2_GREEN()                                                                        // function to set D2 green
{
    D2_RED = 0;
    D2_GREEN = 1;
    D2_BLUE = 0;
}
void SET_D2_YELLOW()                                                                       // function to set D2 yellow
{
    D2_RED = 1;
    D2_GREEN = 1;
    D2_BLUE = 0;
}
void SET_D2_BLUE()                                                                         // function to set D2 blue
{
    D2_RED = 0;
    D2_GREEN = 0;
    D2_BLUE = 1;
}
void SET_D2_PURPLE()                                                                       // function to set D2 purple
{
    D2_RED = 1;
    D2_GREEN = 0;
    D2_BLUE = 1;
}
void SET_D2_CYAN()                                                                         // function to set D2 cyan
{
    D2_RED = 0;
    D2_GREEN = 1;
    D2_BLUE = 1;
}
void SET_D2_WHITE()                                                                        // function to set D2 white
{
    D2_RED = 1;
    D2_GREEN = 1;
    D2_BLUE = 1;
}
void SET_D1_NL()                                                                           // function to set D1 no light
{
    D1_RED = 0;
    D1_GREEN = 0;
    D1_BLUE = 0;
}
void SET_D1_RED()                                                                          // function to set D1 red
{
    D1_RED = 1;
    D1_GREEN = 0;
    D1_BLUE = 0;
}
void SET_D1_GREEN()                                                                        // function to set D1 green
{
    D1_RED = 0;
    D1_GREEN = 1;
    D1_BLUE = 0;
}
void SET_D1_YELLOW()                                                                       // function to set D1 yellow
{
    D1_RED = 1;
    D1_GREEN = 1;
    D1_BLUE = 0;
}
void SET_D1_BLUE()                                                                         // function to set D1 blue
{
    D1_RED = 0;
    D1_GREEN = 0;
    D1_BLUE = 1;
}
void SET_D1_PURPLE()                                                                       // function to set D1 purple
{   
    D1_RED = 1;
    D1_GREEN = 0;
    D1_BLUE = 1;
}
void SET_D1_CYAN()                                                                         // function to set D1 cyan
{
    D1_RED = 0;
    D1_GREEN = 1;
    D1_BLUE = 1;
}
void SET_D1_WHITE()                                                                        // function to set D1 white
{
    D1_RED = 1;
    D1_GREEN = 1;
    D1_BLUE = 1;
}