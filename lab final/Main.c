#include <stdio.h>
#include <stdlib.h>
#include <xc.h>
#include <math.h>
#include <p18f4620.h>
#include <usart.h>
#include <string.h>

#include "I2C.h"
#include "I2C_Support.h"
#include "Interrupt.h"
#include "Fan_Support.h"
#include "Main.h"
#include "ST7735_TFT.h"
#include "utils.h"
#include "Main_Screen.h"
#include "Setup_Time.h"
#include "Setup_Fan_Temp.h"

#pragma config OSC = INTIO67
#pragma config BOREN =OFF
#pragma config WDT=OFF
#pragma config LVP=OFF


char second = 0x00;
char minute = 0x00;
char hour = 0x00;
char dow = 0x00;
char day = 0x00;
char month = 0x00;
char year = 0x00;

char found;
char tempSecond = 0xff; 
signed char tempC, tempF;
char setup_second, setup_minute, setup_hour, setup_day, setup_month, setup_year;

unsigned char fan_set_temp = 16;
unsigned char Nec_state = 0;
float volt;
char INT0_flag, INT1_flag, INT2_flag;
char light_flag;

short Nec_ok = 0;
char Nec_Button;
char FAN;
char duty_cycle;
int rps;
int rpm;

char FANMODE = 0;
char FAN_TIMER_MODE = 0;
char FAN_TIMER_TIME = 0;
char SYSTEM_ON = 0;

char buffer[35]     = "                     \0";
char *nbr;
char *txt;
char tempC_Txt[]    = "+25";
char tempF_Txt[]    = "+077";
char time_Txt[]     = "00:00:00";
char date_Txt[]     = "00/00/00";
char Fan_Timer_Mode_Txt[] = "OFF";
char Fan_Timer_Txt[] = "000";
char Fan_Set_Temp_Txt[] = "77F";
char Fan_Mode_Txt[]  = "MANU";                                                  // text storage for Fan Mode
char Fan_SW_Txt[]   = "OFF";                                                    // text storage for Fan On/Off

    
char DC_Txt[]       = "000";                                                    // text storage for Duty Cycle value
char Volt_Txt[]     = "0.00V";
char RTC_ALARM_Txt[]= "0";                  
char RPM_Txt[]      = "0000";                                                   // text storage for RPM

char setup_time[]       = "00:00:00";
char setup_date[]       = "01/01/00";

char setup_fan_set_text[]   = "077F";

void Do_Init()                                                                  // Initialize the ports 
{ 
    Init_UART();                                                                // Initialize the uart
    Init_ADC();
    OSCCON=0x70;                                                                // Set oscillator to 8 MHz 
    
    ADCON1= 0x0E;
    TRISA = 0x01;
    TRISB = 0x07;
    TRISC = 0x01;
    TRISD = 0x00;
    TRISE = 0x00;
    PORTE = 0x00;

    FAN = 0;
    RBPU =0;

    I2C_Init(100000); 

    DS1621_Init();
    Init_Interrupt();
    duty_cycle = 50;
    Turn_Off_Fan();
    fan_set_temp = 77;
}


void main() 
{
    Do_Init();                                                                  // Initialization 

    Initialize_Screen();                                                        // initialize screen
    
    TMR3L = 0x00;                                                               // set timer 3
    T3CON = 0x03;


    tempSecond = 0xff;
    
    while (1)
    {
        DS3231_Read_Time();                                                     // read time

        if(tempSecond != second)
        {
            tempSecond = second;
            rpm = get_RPM();                                                    // get rpm       
 

            volt = Read_Volt(0);                                                // read voltage
            if (volt > 3.000) light_flag = 1;                                   // set light flag accordingly
            tempC = DS1621_Read_Temp();                                         // get tempC
            tempF = (tempC * 9 / 5) + 32;                                       // calculate tempF
            Set_D1_RGB(duty_cycle);                                             // set leds
            Set_D2_RGB(rpm);
            Set_D3_RGB(FAN_TIMER_MODE);    
                                                                                // print information to teraterm
            printf ("%02x:%02x:%02x %02x/%02x/%02x",hour,minute,second,month,day,year);
            printf (" Temp = %d C = %d F ", tempC, tempF);
            printf ("Light Sensor = %f V", volt);
            printf (" RPM = %d  dc = %d ls = %d\r\n", rpm, duty_cycle, light_flag);
            Monitor_Fan();

            Update_Screen();                                                    // update screen
        }
 
// add code here to do the following tasks:

// Task #1
// The function 'check_for_button_input()' is provided to check if a remote button has been pressed. Write a conditional if statement to check when that mentioned function
// returns a 1 and the variable 'SYSTEM_ON' is 1, then do the following check:
        if(check_for_button_input() == 1 && (SYSTEM_ON == 1))                   // if check for button input and system on = 1
        {
            printf ("button %d\r\n", found);                                    // print button found on teraterm
            Nec_ok = 0;                                                         // nec ok = 0
            switch(found)                                                       
            {
                case Ch_Minus:                                                  // case Ch minus --> do beep good and do setup time
                    Do_Beep_Good();
                    Do_Setup_Time();
                    break;
                case Ch_Plus:                                                   // case Ch plus --> do beep good and setup fan temp
                    Do_Beep_Good();
                    Setup_Fan_Temp();
                    break;
                case Prev:                                                      // case prev --> do beep good and do change fan timer mode minus
                    Do_Beep_Good();
                    Do_Change_Fan_Timer_Mode_Minus();
                    break;
                case Next:                                                      // case next --> do beep good and do change fan timer mode plus
                    Do_Beep_Good();
                    Do_Change_Fan_Timer_Mode_Plus();
                    break;
                case Play_Pause:                                                // case play pause --> do beep good and toggle fan
                    Do_Beep_Good();
                    Toggle_Fan();
                    break;
                case Minus:                                                     // case minus --> do beep good and decrease duty cycle
                    Do_Beep_Good();
                    Decrease_Duty_Cycle();
                    break;
                case Plus:                                                      // case plus --> do beep good and increase duty cycle                                          
                    Do_Beep_Good();
                    Increase_Duty_Cycle();
                    break;
                case EQ:                                                        // case eq --> do beep good and toggle fan mode
                    Do_Beep_Good();
                    if(FANMODE == 1)
                    {
                        FANMODE = 0;
                    }
                    else
                    {
                        FANMODE = 1;
                        
                    }
                    break;
                default:
                    Do_Beep_Bad();                                              // default do beep bad
            }
        }
// first printout using printf the value of 'found'
// Next, set the variable Nec_ok = 0 so acknowledge receipt of the just pressed remote button
// Then, use a case statement that will decide using the variable 'found'
//   case Ch_Minus: generate a good beep (use DO_Beep_GOOD(), then call the function Do_Setup_Time();
//   case Channel: generate a good beep, then call the function Setup_Fan_Temp();  
//   case Prev: generate a good beep, then call the function Do_Change_Fan_Timer_Mode_Minus();
//   case Next: generate a good beep, then call the function Do_Change_Fan_Timer_Mode_Plus();  
//   case Play/Pause: generate a good beep, then call the function Toggle_Fan();
//   case Minus: generate a good beep, then call the function Decrease_Duty_Cycle(); 
//   case Plus: generate a good beep, then call the function Increase_Duty_Cycle();
//	 case EQ: check if FANMODE is on. If so, change it to 0 else change it to 1. In other words, toggle FANMODE;
//   case default: call Do_Beep_Bad(); to indicate illegal remote button
        if(INT1_flag == 1 || light_flag == 1)                                   // if software interrupt flag and light flag = 1
        {
            INT1_flag = 0;                                                      // clear software interrupt flag and light flag
            light_flag = 0;
            if(SYSTEM_ON == 1)                                                  // if system on, print system is now turn off
            {
                printf("System is now turn OFF\r\n");
                SYSTEM_ON = 0;                                                  // set system on, fan, fan en, fan on led, fan timer mode, and fan timer time to 0
                FAN = 0;
                FAN_EN = 0;
                FANON_LED = 0;
                FAN_TIMER_MODE = 0;
                FAN_TIMER_TIME = 0;
                Initialize_Screen();                                            // initialize screen
            }
            else                                            
            {
                printf("System is now turn ON\r\n");                            // print system now turn on
                SYSTEM_ON = 1;                                                  // set system on = 1
                Initialize_Screen();                                            // initialize screen
            }
        }
// Task #2
// using another if statement to check if both either variable INT1flag or light_flag  is 1.
// If either variable is 1, clear both variables to 0. Then check if the SYSTEM_ON is 1. 
// If 1, print a message on TeraTerm that the system is now turn off. See SYSTEM_ON to 0, turn off
// the fan, clear the variables FAN_TIMER_MODE and FAN_TIMER_TIME to 0, printf ("System is now turn OFF\r\n") 
// and call the function Initialize_Screen() to change the display.
// On the other hand, if SYSTEM_ON is 0, then print a message on TT to say that the system is now on and then set the
// variable SYSTEM_ON to 1 and also call the function Initialize_Screen()


        
    }
}


void Set_D1_RGB(char duty_cycle)
{
   // add code from lab 12 for RGB D1
    int x = duty_cycle / 10;                                                    // formula for led 1
    if (x > 7)
        x = 7;                                                                  // make sure > 7 stays white
    PORTA = (PORTA & 0x07) | (x<<3);                                            // mask, shift, and output
}

void Set_D2_RGB(int rpm)
{
   // add code from lab 12 for RGB D2
       // find formula and shift bits. mask bits
    char y = (rpm/500) + 1;                                                     // formula for led 2
    if (rpm == 0)
        y = 0;                                                                  // special case
    if (y > 7)
        y = 7;                                                                  // make sure > 7 stays white
    PORTB = (PORTB & 0x07) | (y << 3);                                          // mask, shift, and output
    // need to mask
}

void Set_D3_RGB(char FAN_TIMER_MODE)
{
    int j;
    if(FAN_TIMER_MODE == FAN_TIMER_MODE0)                                       // if fan timer mode in timer mode 0, j = 0
        j = 0;
    else if (FAN_TIMER_MODE == FAN_TIMER_MODE1)                                 // if fan timer mode in timer mode 1, j = 1
        j = 1;
    else if (FAN_TIMER_MODE == FAN_TIMER_MODE2)                                 // if fan timer mode in timer mode 2, j = 2
        j = 2;
    else if (FAN_TIMER_MODE == FAN_TIMER_MODE3)                                 // if fan timer mode in timer mode 3, j = 3
        j = 3;
    PORTA = (PORTA & 0xF9) | (j<<1);                                            // mask, shift, and output
}
