#pragma config OSC = INTIO67
#pragma config WDT = OFF
#pragma config LVP = OFF
#pragma config BOREN = OFF

#include <stdio.h>
#include <stdlib.h>
#include <xc.h>
#include <math.h>
#include <p18f4620.h>
#include <string.h>

#include "main.h"
#include "utils.h"
#include "Main_Screen.h"
#include "ST7735_TFT.h"

void init_UART(void);                                                   // function declarations
void Set_EW(char color);
void Set_EW_LT(char color);
void Set_NS(char color);
void Set_NS_LT(char color);
void PED_Control( char Direction, char Num_Sec);
void Day_Mode();
void Night_Mode();
void Wait_N_Seconds (char);
void Wait_One_Second_With_Beep();
void Wait_One_Second();
void Do_Flashing();
void putch (char c);
#define _XTAL_FREQ  8000000                                            // Set operation for 8 Mhz
#define TMR_CLOCK   _XTAL_FREQ/4                                       // Timer Clock 2 Mhz
                                                                       // variable definitions
char INT0_Flag;
char INT1_Flag;
char INT2_Flag;

char dir;
char Count;                                                           // RAM variable for Second Count
char PED_Count;                                                       // RAM variable for Second Pedestrian Count

char  MODE;
char direction;
float volt;
                                                                      // initialize switches at 0
char NS_PED_SW = 0;
char EW_PED_SW = 0;
extern char Light_Sensor;
char Flashing_Request = 0;
char Flashing_Status = 0;
void main(void)
{
    OSCCON = 0x70;                                                  // set the system clock to be 1MHz 1/4 of the 4MHz
    TRISA = 0x1F;                                                   // inputs RA0/AN0-RA4 inputs,RA5 output
    TRISB = 0x07;                                                   // outputs
    TRISC = 0x00;                                                   // set PORTC as outputs
    TRISD = 0x00;                                                   // set PORTD as outputs
    TRISE = 0x00;                                                   // set PORTE as outputs
    Init_ADC();
    Init_UART();

Initialize_LCD_Screen();                                            // Initialize the TFT screen
RBPU = 0;
init_INTERRUPT();
//                                                                  // while loop test if interrupts work
// while (1)                                                    
//{ // Do nothing,
// if (INT0_Flag == 1)
//{
// INT0_Flag = 0; // clear the flag
// printf ("INT0 interrupt pin detected \r\n");
//// print a message that INT0 has
//// occurred
// }
// if (INT1_Flag == 1)
//{
// INT1_Flag = 0; // clear the flag
// printf ("INT1 interrupt pin detected \r\n");
//// print a message that INT1 has
//// occurred
// }
// if (INT2_Flag == 1)
//{
// INT2_Flag = 0; // clear the flag
// printf ("INT2 interrupt pin detected \r\n");
//// print a message that INT2 has
//// occurred
// }
//} 

//    while (1)
//    {
//        Rcmd2red();
//        delay_ms(2);
//        
//    }

    volt = Read_Volt(0);                        // 

    Light_Sensor = volt < 2.5 ? 1:0;                            // Mode = 1, Day_mode, Mode = 0 Night_mode

    while(1)                                                    // forever loop
    {

        if (Light_Sensor == 1)                                 // if voltage < 2.5
        {
            MODE = 1;                                          // mode = 1
            Day_Mode();                                        // calls Day_Mode() function
        }
        else
        {
            MODE = 0;                                          // mode = 0
            Night_Mode();                                      // calls Night_Mode() function
        }
        
        if(Flashing_Request == 1)                              // if flashing request interrupt = 1
        {
            Flashing_Request = 0;                              // set request = 0
            Flashing_Status = 1;                               // set status = 1
            Do_Flashing();                                     // do flashing routine
        }
    } 
}

void Do_Flashing()                                             // flashing routine
{
    while(Flashing_Status == 1)                                // do while status = 1
    {
        if(Flashing_Request == 0)                              // if request = 0
        {
            Set_EW(RED);                                       // set all led RED
            Set_EW_LT(RED);
            Set_NS(RED);
            Set_NS_LT(RED);
            Wait_One_Second();                                 // wait one sec
            Set_EW(OFF);                                       // set all led OFF
            Set_EW_LT(OFF);
            Set_NS(OFF);
            Set_NS_LT(OFF);
            Wait_One_Second();                                 // wait one sec
        }
        else
        {
            Flashing_Request = 0;                              // if request = 1 set request to 0
            Flashing_Status = 0;                               // status = 0
        }
    }
}
void Set_EW(char color)                                        // set EW color
{
    direction = EW;
    update_LCD_color (direction, color);                       // call function from main screen
    switch(color)
    {
        case OFF: EW_RED = 0; EW_GREEN=0;break;                // turn off the EW LED
        case RED: EW_RED = 1; EW_GREEN=0;break;                // sets EW LED RED
        case GREEN: EW_RED = 0; EW_GREEN=1;break;              // sets EW LED GREEN
        case YELLOW: EW_RED = 1; EW_GREEN=1;break;             // sets EW LED YELLOW
    }
}

void Set_EW_LT(char color)                                      // set EWLT color
{
    direction = EWLT;
    update_LCD_color (direction, color);                        // call function from main screen
     switch(color)
    {
        case OFF: EWLT_RED = 0; EWLT_GREEN=0;break;             // turn off the EWLT LED
        case RED: EWLT_RED = 1; EWLT_GREEN=0;break;             // sets EWLT LED RED
        case GREEN: EWLT_RED = 0; EWLT_GREEN=1;break;           // sets EWLT LED GREEN
        case YELLOW: EWLT_RED = 1; EWLT_GREEN=1;break;          // sets EWLT LED YELLOW
    }
}


void Set_NS(char color)                                         // set NS color
{
    direction = NS;
    update_LCD_color (direction, color);                        // call function from main screen 
    switch(color)
    {
        case OFF: NS_RED = 0; NS_GREEN=0;break;                 // turn off the NS LED
        case RED: NS_RED = 1; NS_GREEN=0;break;                 // sets NS LED RED
        case GREEN: NS_RED = 0; NS_GREEN=1;break;               // sets NS LED GREEN
        case YELLOW: NS_RED = 1; NS_GREEN=1;break;              // sets NS LED YELLOW
    } 
}

void Set_NS_LT(char color)                                      // set NSLT color
{
    direction = NSLT;
    update_LCD_color (direction, color);                        // call function from main screen 
    switch(color)
    {
        case OFF: NSLT_RED = 0; NSLT_GREEN=0;break;             // turn off the NSLT LED
        case RED: NSLT_RED = 1; NSLT_GREEN=0;break;             // sets NSLT LED RED
        case GREEN: NSLT_RED = 0; NSLT_GREEN=1;break;           // sets NSLT LED GREEN
        case YELLOW: NSLT_RED = 1; NSLT_GREEN=1;break;          // sets NSLT LED YELLOW
    }
}


void PED_Control(char direction, char Num_Sec)                 // ped control function
{ 
    for(char i = Num_Sec-1;i>0; i--)                           // for loop from num_sec - 1 to off
    {
        update_LCD_PED_Count(direction,i);                     // PED count function from main screen 
        Wait_One_Second_With_Beep();                           // wait one second with beep
    }
    update_LCD_PED_Count(direction,0);                         // display 0
    Wait_One_Second_With_Beep();                               // wait one second with beep
}

void Day_Mode()                                                // day mode routine from lab 6
{
    //EW_PED_SW = 0;
    //NS_PED_SW = 0;
    PORTEbits.RE2 = 1; //mode led on
    Set_NS(RED);
    Set_NS_LT(RED);
    Set_EW_LT(RED);
    Set_EW(GREEN);
    if(EW_PED_SW == 1)
    {
        PED_Control(0, PEDESTRIAN_EW_WAIT);                    // use defined variable from wait times
        EW_PED_SW = 0;
    }
    Wait_N_Seconds(EW_WAIT);                                   // use defined variable from wait times
    Set_EW(YELLOW);
    Wait_N_Seconds(2);
    Set_EW(RED);
    if(NS_LT_SW == 1)
    {
        Set_NS_LT(GREEN);
        Wait_N_Seconds(NS_LT_WAIT);                           // use defined variable from wait times
        Set_NS_LT(YELLOW);
        Wait_N_Seconds(2);
        Set_NS_LT(RED);
    }
    Set_NS(GREEN);
    if(NS_PED_SW == 1)
    {
        PED_Control(1,PEDESTRIAN_NS_WAIT);                   // use defined variable from wait times
        NS_PED_SW= 0;
    }
    Set_NS(GREEN);
    Wait_N_Seconds(NS_WAIT);                                // use defined variable from wait times
    Set_NS(YELLOW);
    Wait_N_Seconds(2);
    Set_NS(RED);
    if(EW_LT_SW == 1)
    {
        Set_EW_LT(GREEN);
        Wait_N_Seconds(EW_LT_WAIT);                         // use defined variable from wait times
        Set_EW_LT(YELLOW);
        Wait_N_Seconds(2);
        Set_EW_LT(RED);
    }
}

void Night_Mode()                                               // night mode routine from lab 6
{ 
    NS_PED_SW = 0;
    EW_PED_SW = 0;
    PORTEbits.RE2 = 0; // mode led off
    Set_NS(RED);
    Set_NS_LT(RED);
    Set_EW_LT(RED);
    Set_EW(GREEN);
    Wait_N_Seconds(NIGHT_EW_WAIT);                             // use defined variable from wait times
    Set_EW(YELLOW);
    Wait_N_Seconds(2);
    Set_EW(RED);
    if(NS_LT_SW == 1)
    {
        Set_NS_LT(GREEN);
        Wait_N_Seconds(NIGHT_NS_LT_WAIT);                     // use defined variable from wait times
        Set_NS_LT(YELLOW);
        Wait_N_Seconds(2);
        Set_NS_LT(RED);
    }
    
    Set_NS(GREEN);
    Wait_N_Seconds(NIGHT_NS_WAIT);                             // use defined variable from wait times
    Set_NS(YELLOW);
    Wait_N_Seconds(2);
    Set_NS(RED);
    if(EW_LT_SW == 1)
    {
        Set_EW_LT(GREEN);
        Wait_N_Seconds(NIGHT_EW_LT_WAIT);                     // use defined variable from wait times                    
        Set_EW_LT(YELLOW);
        Wait_N_Seconds(2);
        Set_EW_LT(RED);
    }
}

void Wait_One_Second()							//creates one second delay and blinking asterisk
{
    SEC_LED = 1;
    Draw_Star();
    Wait_Half_Second();                         // Wait for half second (or 500 msec)
    SEC_LED = 0;
    Erase_Star();
    Wait_Half_Second();                         // Wait for half second (or 500 msec)
	update_LCD_misc();                          // update misc from main screen
}

void Wait_One_Second_With_Beep()				//creates one second delay as well as sound buzzer
{
    SEC_LED = 1;
    Draw_Star();
    Activate_Buzzer();
    Wait_Half_Second();                         // Wait for half second (or 500 msec)

    SEC_LED = 0;
    Erase_Star();
    Deactivate_Buzzer();
    Wait_Half_Second();                         // Wait for half second (or 500 msec)
	update_LCD_misc();                          // update misc from main screen
}
 

void Wait_N_Seconds (char seconds)
{
    char I;
    for (I = seconds; I> 0; I--)
    {
		update_LCD_count(direction, I);         // count from main screen
        Wait_One_Second();          			// calls Wait_One_Second for x number of times
    }
	update_LCD_count(direction,0);              // display 0 on count
}
  







