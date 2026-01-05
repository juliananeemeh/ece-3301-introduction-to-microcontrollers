#include <xc.h>
#include <p18f4620.h>
#include "Interrupt.h"
#include "stdio.h"

unsigned char bit_count;
unsigned int Time_Elapsed;

extern unsigned char Nec_state;
extern short Nec_ok;
unsigned long long Nec_code;

extern char Nec_Button;
extern char INT0_flag;
extern char INT1_flag;
extern char INT2_flag;

void Init_Interrupt(void)
{s
                                                                                // clear interrupt flags
    INTCONbits.INT0IF = 0 ;                         
    INTCON3bits.INT1IF = 0;             
    INTCON3bits.INT2IF = 0; 
    
    INTCONbits.INT0IE = 1;                                                      // set interrupt enables
    INTCON3bits.INT1IE = 1;            
    INTCON3bits.INT2IE = 1;   
    
    INTCON2bits.INTEDG0 = 0;                                                    // set edges to falling edge
    INTCON2bits.INTEDG1 = 0;          
    INTCON2bits.INTEDG2 = 0;  
    TMR1H = 0;                                                                  // Reset Timer1
    TMR1L = 0;                              
    PIR1bits.TMR1IF = 0;                                                        // Clear timer 1 interrupt flag
    PIE1bits.TMR1IE = 1;                                                        // Enable Timer 1 interrupt
    INTCONbits.PEIE = 1;                                                        // Enable Peripheral interrupt
    INTCONbits.GIE = 1;                                                         // Enable global interrupts

}

void Enable_INT_Interrupt()
{
    INTCONbits.INT0IE = 1;                                                      // Enable external  interrupt 0
    INTCON2bits.INTEDG0 = 0;                                                    // Edge programming for INT0 falling edge
}    

void interrupt  high_priority chkisr() 
{    
    if (PIR1bits.TMR1IF == 1) TIMER1_isr();                                     // check timer 1 interrupt
    if (INTCONbits.INT0IF == 1) INT0_isr();                                     // check the INT0 interrupt
    if (INTCON3bits.INT1IF == 1) INT1_isr();                                    // check the INT1 interrupt
    if (INTCON3bits.INT2IF == 1) INT2_isr();                                    // check the INT2 interrupt
}

void TIMER1_isr(void)
{
    Nec_state = 0;                                                              // Reset decoding process
    INTCON2bits.INTEDG1 = 0;                                                    // Edge programming for INT0 falling edge
    T1CONbits.TMR1ON = 0;                                                       // Disable T1 Timer
    PIR1bits.TMR1IF = 0;                                                        // Clear interrupt flag
}

void force_nec_state0()
{
    Nec_state=0;                                                                // set nec state to 0 
    T1CONbits.TMR1ON = 0;                                                       // TMR1ON = 0
}


void INT0_isr() 
{  
// put your code from your IR lab here
    INTCONbits.INT0IF = 0;                                                      // Clear external interrupt INT0IF
    if (Nec_state != 0)
    {
        Time_Elapsed = (TMR1H << 8) | TMR1L;                                    // Store Timer1 value
        TMR1H = 0;                                                              // Reset Timer1
        TMR1L = 0;
    }

        switch(Nec_state)
        {
            case 0 :                                                            // case 0
            {

                // Clear Timer 1
                TMR1H = 0;                                                      // Reset Timer1
                TMR1L = 0;                      
                PIR1bits.TMR1IF = 0;            
                T1CON = 0x90;                                                   // Program Timer1 mode with count = 1usec using System clock running at 8Mhz
                T1CONbits.TMR1ON = 1;                                           // Enable Timer 1
                bit_count = 0;                                                  // Force bit count (bit_count) to 0
                Nec_code = 0;                                                   // Set Nec_code = 0
                Nec_state = 1;                                                  // Set Nec_State to state 1
                PORTE = Nec_state;                                              // output nec state to port e
                INTCON2bits.INTEDG0 = 1;                                        // Change Edge interrupt of INT 0 to Low to High            
                return;
            }

            case 1 :                                                            // case 1
            {
  //              printf("hello 1\r\n");  
                if(Time_Elapsed >=8500 && Time_Elapsed <= 9500)                 // if time elapsed in between 8500 and 9500
                {
                    Nec_state = 2;                                              // go to nec state 2
                    PORTE = Nec_state;                                          // output nec state to PORTE
                }
                else
                {
                    force_nec_state0();                                         // go to nec state 0
                }
                INTCON2bits.INTEDG0 = 0;                                        // high to low
                return;
            }

            case 2 :                                                            // case 2                        
            {
                if(Time_Elapsed >=4000 && Time_Elapsed <= 5000)                 // if time elapsed in between 4000 and 5000
                {   
                    Nec_state = 3;                                              // go to nec state 3
                    PORTE = Nec_state;                                          // output nec state to PORTE
                }
                else
                {
                    force_nec_state0;                                           // go to nec state 0
                }
                INTCON2bits.INTEDG0 = 1;                                        // low to high 
                return;
            }

            case 3 :                                                            // case 3            
            {
                if(Time_Elapsed >=400 && Time_Elapsed <= 700)                   // if time elapsed in between 400 and 700
                {
                    Nec_state = 4;                                              // go to nec state 4
                    PORTE = Nec_state;                                          // output nec state to PORTE
                }
                else
                {
                    force_nec_state0();                                         // go to nec state 0
                }
                INTCON2bits.INTEDG0 = 0;                                        // high to low
                return; 
            }

            case 4 :                                                            // case 4
            {
                if(Time_Elapsed >=400 && Time_Elapsed <= 1800)                  // if time elapsed in between 400 and 1800
                {
                    Nec_code = Nec_code << 1;                                   // shift nec state left 1
                    if(Time_Elapsed > 1000)                                     // if time elapsed greater than 1000, increment nec code
                    {
                        Nec_code = Nec_code + 1;
                    }
                    bit_count = bit_count + 1;                                  // increment bit count
                    if(bit_count > 31)                                          // if bit count greater than 31
                    {
                        Nec_Button = Nec_code >> 8;                             // nec button = nec code shift right 8 times
                        Nec_state = 0;                                          // nec state = 0
                        PORTE = Nec_state;                                      // output nec state to PORTE
                        Nec_ok = 1;                                             // nec ok = 1
                        INTCONbits.INT0IE = 0;                                  // clear interrupt enable
                    }
                    else
                    {
                        Nec_state = 3;                                          // go to nec state 3
                        PORTE = Nec_state;                                      // output nec state to PORTE
                    }
                }
                else
                {       
                    force_nec_state0();                                         // go to nec state 0
                }
                INTCON2bits.INTEDG0 = 1;                                        // low to high
                return;
            }
        }

}

void INT1_isr() 
{
    int k;
    for (k=0; k<20000;k++);
    INTCON3bits.INT1IF = 0;                                                     // clear hardware flag and set software flag
    INT1_flag = 1;
} 

void INT2_isr() 
{  
    int k;
    for (k=0; k<20000;k++);
    INTCON3bits.INT2IF == 0;                                                    // clear hardware flag and set software flag
    INT2_flag = 1;
    
} 





