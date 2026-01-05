
#include <xc.h>
#include <p18f4620.h>
#include "Interrupt.h"
#include <stdio.h>

unsigned char bit_count;                                        // variable declarations
unsigned int Time_Elapsed;

unsigned long long Nec_code;
unsigned char Nec_State = 0;

extern char Nec_Button;
extern short Nec_OK;

void Init_Interrupt()
{
    INTCONbits.INT0IF = 0;                                      // Clear external interrupt INT0IF
    INTCON2bits.INTEDG0 = 0;                                    // Edge programming for INT 0 falling edge H to L
    INTCONbits.INT0IE = 1;                                      // Enable external interrupt INT0IE

    TMR1H = 0;                                                  // Reset Timer1
    TMR1L = 0;                                 
    PIR1bits.TMR1IF = 0;                                        // Clear timer 1 interrupt flag
    PIE1bits.TMR1IE = 1;                                        // Enable Timer 1 interrupt
    INTCONbits.PEIE = 1;                                        // Enable Peripheral interrupt
    INTCONbits.GIE = 1;                                         // Enable global interrupts
}

void Enable_INT_Interrupt()
{
    INTCONbits.INT0IE = 1;                                      // Enable external  interrupt 0
    INTCON2bits.INTEDG0 = 0;                                    // Edge programming for INT0 falling edge
}    

void interrupt high_priority chkisr()
{
    if (PIR1bits.TMR1IF == 1) TIMER1_isr();                     // if TMR1IF = 1, call TIMER1 interrupt
    if (INTCONbits.INT0IF == 1) INTx_isr();                     // if INT0IF = 1, call interrupt function
}

void TIMER1_isr(void)
{
    Nec_State = 0;                                              // Reset decoding process
    INTCON2bits.INTEDG0 = 0;                                    // Edge programming for INT 0 falling edge
    T1CONbits.TMR1ON = 0;                                       // Disable T1 Timer
    PIR1bits.TMR1IF = 0;                                        // Clear interrupt flag
}

void Reset_Nec_State()
{
    Nec_State = 0;                                              // Nec state back to 0
    T1CONbits.TMR1ON = 0;                                       // TMR1ON = 0
}

void INTx_isr(void)
{
    INTCONbits.INT0IF = 0;                                      // Clear external interrupt INT0IF
    if (Nec_State != 0)
    {
        Time_Elapsed = (TMR1H << 8) | TMR1L;                    // Store Timer1 value
        TMR1H = 0;                                              // Reset Timer1
        TMR1L = 0;
    }

        switch(Nec_State)
        {
            case 0 :
            {

                // Clear Timer 1
                TMR1H = 0;                                      // Reset Timer1
                TMR1L = 0;                      
                PIR1bits.TMR1IF = 0;            
                T1CON = 0x90;                                   // Program Timer1 mode with count = 1usec using System clock running at 8Mhz
                T1CONbits.TMR1ON = 1;                           // Enable Timer 1
                bit_count = 0;                                  // Force bit count (bit_count) to 0
                Nec_code = 0;                                   // Set Nec_code = 0
                Nec_State = 1;                                  // Set Nec_State to state 1
                PORTE = Nec_State;                              // output nec state to port e
                INTCON2bits.INTEDG0 = 1;                        // Change Edge interrupt of INT 0 to Low to High            
                return;
            }

            case 1 :
            {
                if(Time_Elapsed >=8500 && Time_Elapsed <= 9500) // if time elapsed between 8500 and 9500 us
                {
                    Nec_State = 2;                              // move to Nec state 2
                    PORTE = Nec_State;                          // output to PORTE
                }
                else
                {
                    Reset_Nec_State();                          // call Reset Nec State
                }
                INTCON2bits.INTEDG0 = 0;                        // edge interrupt from high to low
                return;
            }

            case 2 :                            
            {
                if(Time_Elapsed >=4000 && Time_Elapsed <= 5000) // if time elapsed between 4000 and 5000
                {
                    Nec_State = 3;                              // move to Nec state 3
                    PORTE = Nec_State;                          // output to PORTE
                }
                else
                {
                    Reset_Nec_State();                          // call reset nec state
                }
                INTCON2bits.INTEDG0 = 1;                        // edge interrupt from low to high
                return;
            }

            case 3 :                            
            {
                if(Time_Elapsed >=400 && Time_Elapsed <= 700)   // if time elapsed between 400 and 700
                {
                    Nec_State = 4;                              // move to Nec state 4
                    PORTE = Nec_State;                          // output to PORTE
                }
                else
                {
                    Reset_Nec_State();                         // call reset nec state
                }
                INTCON2bits.INTEDG0 = 0;                       // edge interrupt from high to low
                return; 
            }

            case 4 :                            
            {
                if(Time_Elapsed >=400 && Time_Elapsed <= 1800) // if time elapsed betewen 400 and 1800
                {
                    Nec_code = Nec_code << 1;                  // shift nec code left 1 bit
                    if(Time_Elapsed > 1000)                    // if time elapsed greater than 1000
                    {
                        Nec_code = Nec_code + 1;               // add 1 to Nec code
                    }
                    bit_count = bit_count + 1;                 // add 1 to bit count
                    if(bit_count > 31)                         // if bit count greater than 31
                    {
                        Nec_Button = Nec_code >> 8;            // nec button = nec code shift right 8 times
                        Nec_State = 0;                         // nec state = 0
                        PORTE = Nec_State;                     // output to PORTE
                        Nec_OK = 1;                            // Nec_OK = 1
                        INTCONbits.INT0IE = 0;                 // edge interrupt from high to low
                    }
                    else
                    {
                        Nec_State = 3;                         // nec state = 3
                        PORTE = Nec_State;                     // output to PORTE
                    }
                }
                else
                {       
                    Reset_Nec_State();                         // call reset nec state function
                }
                INTCON2bits.INTEDG0 = 1;                       // edge interrupt from low to high
                return;
            }
        }
    
}
    