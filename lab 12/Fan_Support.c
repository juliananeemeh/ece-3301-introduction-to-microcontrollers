#include <p18f4620.h>
#include "Main.h"
#include "Fan_Support.h"
#include "stdio.h"

extern char FAN;
extern char duty_cycle;


int get_RPM()                                                                   // functioon to get rpm
{
    int RPS = TMR3L / 2;                                                        // read the count. Since there are 2 pulses per rev
                                                                                // then RPS = count /2
    TMR3L = 0;                                                                  // clear out the count
    return (RPS * 60);                                                          // return RPM = 60 * RPS
}

void Toggle_Fan()                                                               // function to turn on and off fan
{
    if(FAN == 0)                                                                // if fan off, turn fan on
    {
        Turn_On_Fan();
    }   
    else                                                                        // if fan on, turn fan off
    {
        Turn_Off_Fan();
    }
}

void Turn_Off_Fan()                                                             // function to turn off fan
{
    FAN = 0;
    FAN_EN = 0;                                                                 // FAN, EN and LED = 0
    FANON_LED = 0;
}

void Turn_On_Fan()                                                              // function to turn on fan
{
    FAN = 1;                                                                    // fan variable = 1
    do_update_pwm(duty_cycle);                                                  // update pwm
    FAN_EN = 1;                                                                 // EN and LED on
    FANON_LED = 1;
}

void Increase_Speed()                                                           // function to increase fan speed
{
    if(duty_cycle == 100)                                                       // if duty cycle = 100, BEEP and keep duty cycle = 100                                                     
    {
        Do_Beep();
        do_update_pwm(duty_cycle);
    }   
    else                                                                        // increase duty cycle by 5 and update pwm
    {
        duty_cycle = duty_cycle + 5;
        do_update_pwm(duty_cycle);
    }

}

void Decrease_Speed()                                                           // function to decrease speed
{
    if(duty_cycle == 0)                                                         // if duty cycle = 0, BEEP and update pwm
    {
        Do_Beep();
        do_update_pwm(duty_cycle);
    }
    else                                                                        // decrease duty cycle by 5 and update pwm
    {
        duty_cycle = duty_cycle - 5;
        do_update_pwm(duty_cycle);
    }
}

void do_update_pwm(char duty_cycle)                                             // function to update pwm
{ 
    float dc_f;
    int dc_I;
    PR2 = 0b00000100 ;                                                          // set the frequency for 25 Khz
    T2CON = 0b00000111 ; 
    dc_f = ( 4.0 * duty_cycle / 20.0) ;                                         // calculate factor of duty cycle versus a 25 Khz
    dc_I = (int) dc_f;                                                          // get the integer part
    if (dc_I > duty_cycle) dc_I++;                                              // round up function
    CCP1CON = ((dc_I & 0x03) << 4) | 0b00001100;
    CCPR1L = (dc_I) >> 2; 

}

void Set_DC_RGB(int duty_cycle)                                                 // function for DC RGB
{
    int x = duty_cycle / 10;                                                    // divide duty cycle by 10
    if (x > 7)
        x = 7;                                                                  // make sure > 7 stays white
    PORTA = (PORTA & 0xF8) | x;                                                 // mask bits and set equal to PORTA
    
}

void Set_RPM_RGB(int rpm)                                                       // function for RPM RGB
{
    char y = (rpm/500) + 1;                                                     // divide by 500 and add 1
    if (rpm == 0)
        y = 0;                                                                  // if rpm = 0, output = 0
    if (y > 7)
        y = 7;                                                                  // make sure > 7 stays white
    PORTA = (PORTA & 0xC7) | (y << 3);                                          // mask bits and set equal to PORTA
}


        
