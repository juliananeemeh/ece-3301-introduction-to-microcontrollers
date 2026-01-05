#include <xc.h>
#include <p18f4620.h>
#include "Interrupt.h"
                                                        // define variables
extern char INT0_Flag;
extern char INT1_Flag;
extern char INT2_Flag;
extern char EW_PED_SW;
extern char NS_PED_SW;
extern char MODE;
extern char Flashing_Request;

void init_INTERRUPT()                                   // interrupt function
{
    INTCONbits.INT0IF = 0;                              // clear flags
    INTCON3bits.INT1IF = 0;
    INTCON3bits.INT2IF = 0;
    
    INTCON2bits.INTEDG0 = 0;                           // high to low
    INTCON2bits.INTEDG1 = 0;
    INTCON2bits.INTEDG2 = 0;
    
    INTCONbits.INT0IE = 1;                             // enable interrupt
    INTCON3bits.INT1IE = 1;
    INTCON3bits.INT2IE = 1;
    
    INTCONbits.GIE = 1;                                // global enable
}

void interrupt  high_priority chkisr()
{
    if (INTCONbits.INT0IF == 1) INT0_ISR();           // check if INT0 has occurred			
    if (INTCON3bits.INT1IF == 1) INT1_ISR();          // check if INT1 has occurred	
    if (INTCON3bits.INT2IF == 1) INT2_ISR();          // check if INT2 has occurred	
}

void INT0_ISR()                                       // interrupt 0 function
{
    INTCONbits.INT0IF=0;                              // Clear the interrupt flag
    //INT0_Flag= 1;    
    if(MODE == 1)
        EW_PED_SW = 1;                                // if in day mode can use EW PED
}

void INT1_ISR()
{
    INTCON3bits.INT1IF=0;                             // Clear the interrupt flag
    //INT1_Flag= 1;  
    if(MODE == 1)
        NS_PED_SW = 1;                                // if day mode can use NS PED
}

void INT2_ISR()
{
    INTCON3bits.INT2IF=0;                             // Clear the interrupt flag
    //INT2_Flag= 1; 
    Flashing_Request = 1;                             // if press interrupt 2, flashing request = 1
}

