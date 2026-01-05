#include <stdio.h>
#include <stdlib.h>
#include <xc.h>
#include <math.h>
#include <p18f4620.h>
#include <usart.h>
#include <string.h>

#pragma config OSC = INTIO67
#pragma config WDT = OFF
#pragma config LVP = OFF
#pragma config BOREN = OFF
#pragma config CCP2MX = PORTC

#include "ST7735_TFT.h"
#include "Main_Screen.h"
#include "utils.h"
#include "Interrupt.h"
#include "main.h"
#include "I2C.h"
#include "I2C_Support.h"
#include "Fan_Support.h"
#define _XTAL_FREQ  8000000             // Set operation for 8 Mhz

char tempSecond = 0xff;
char second = 0x00;
char minute = 0x00;
char hour = 0x00;
char dow = 0x00;
char day = 0x00;
char month = 0x00;
char year = 0x00;
char setup_second, setup_minute, setup_hour, setup_day, setup_month, setup_year;
char alarm_second, alarm_minute, alarm_hour, alarm_date;
char setup_alarm_second, setup_alarm_minute, setup_alarm_hour;
short Nec_OK = 0;
char Nec_Button;
extern unsigned long long Nec_code;



char array1[21]={0xa2,0x62,0xe2,0x22,0x02,0xc2,0xe0,0xa8,0x90,0x68,0x98,0xb0,0x30,0x18,0x7a,0x10,0x38,0x5a,0x42,0x4a,0x52};             
char txt1[21][4] ={"CH-\0","CH \0","CH+\0","|<<\0",">>|\0",">||\0"," - \0"," + \0","EQ \0"," 0 \0","100\0","200\0"," 1 \0"," 2 \0"," 3 \0"," 4 \0"," 5 \0"," 6 \0"," 7 \0"," 8 \0"," 9 \0"};   
int color[21]={RD,RD,RD,CY,CY,GR,BU,BU,MA,BK,BK,BK,BK,BK,BK,BK,BK,BK,BK,BK};                               
char LED1[21] = {0x01,0x01,0x01,0x06,0x06,0x02,0x04,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
char LED2[21] = {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x20,0x28,0x38,0x38,0x38,0x38,0x38,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
char LED3[21] = {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x38,0x38,0x38,0x38,0x38,0x38,0x38};
char LED4[21] = {0,0,0,0,0,0,0,0,0,1,0,0,1,1,1,1,1,1,1,1,1};

char FAN = 0;
char duty_cycle; 
void main()
{
    Init_UART();
    OSCCON = 0x70;                          
    nRBPU = 0; 
    
    PORTA = 0x00;                                                               // clear ports
    PORTB = 0x00;
    PORTC = 0x00;
    PORTD = 0x00;
    PORTE = 0x00;
    
    TRISA = 0x00;                                                               // set inputs and outputs
    TRISB = 0x01;
    TRISC = 0x01;                           
    TRISD = 0x00;                         
    TRISE = 0x00;
    
    ADCON1 = 0x0F; 
    
    Initialize_LCD_Screen();
    T3CON = 0x03;                                                               // initialize timer 3
    I2C_Init(100000);                                                           // initialize I2C and interrupt
    DS1621_Init();
    Init_Interrupt();

    Nec_code = 0x0;                                                             // Clear code
    FAN_EN = 1;
    FANON_LED = 1;
//    FAN_PWM = 1;                            // part a
//    char duty_cycle = 100;                  // part a
    duty_cycle = 50;                                                            // initialize duty cycle and update PWM
    do_update_pwm(duty_cycle);              
    while(1)
    {
        DS3231_Read_Time();
        if(tempSecond != second)
        {
            tempSecond = second;
            signed char tempC = DS1621_Read_Temp();                             // read temperature in C   
            signed char tempF = (tempC * 9 / 5) + 32;                           // calculate temperature in F
            int rpm = get_RPM();                                                // get RPM
            Set_DC_RGB(duty_cycle);                                             // display duty cycle on LED
            Set_RPM_RGB(rpm);                                                   // display rpm on LED
            printf ("%02x:%02x:%02x %02x/%02x/%02x",hour,minute,second,month,day,year);
            printf (" Temperature = %d degreesC = %d degreesF\r\n", tempC, tempF);
            printf ("RPM = %d dc = %d\r\n", rpm, duty_cycle);                   // print time, temperature, rpm and duty cycle to tera term
        } 
        
        if (Nec_OK == 1)
        {
            Nec_OK = 0;
            Enable_INT_Interrupt();
            printf ("NEC_Button = %x \r\n", Nec_Button);  

            char found = 0xff;
            
            // add code here to look for code using a FOR loop
            for (char i = 0; i< 21; i++)
            {
                if(Nec_Button == array1[i])
                {
                    found = i;
                }
            }
            
            if (found != 0xff) 
            {
				printf ("Key Location = %d \r\n\n", found);      
                fillCircle(Circle_X, Circle_Y, Circle_Size, color[found]); 
                drawCircle(Circle_X, Circle_Y, Circle_Size, ST7735_WHITE);  
                drawtext(Text_X, Text_Y, txt1[found], ST7735_WHITE, ST7735_BLACK,TS_1);
   
 
            
                KEY_PRESSED = 1;                                                // make a beep with the buzzer
                Activate_Buzzer();
                Wait_One_Sec();
                Deactivate_Buzzer();
                do_update_pwm(duty_cycle);                                      // update pwm 
                KEY_PRESSED = 0;
            
                if (Nec_Button == 0xa2)
                {
                    DS3231_Setup_Time();                                        // if button code 0xa2 pressed, setup time
                }
                
                // part c
                if (Nec_Button == 0xc2)
                {
                    Toggle_Fan();                                               // if button code 0xc2 pressed, toggle fan
                }
                
                if (Nec_Button == 0xe0)
                {
                    Decrease_Speed();                                           // if button code 0xe0 pressed, decrease speed
                }
                
                if (Nec_Button == 0xa8)
                {
                    Increase_Speed();                                           // if button code 0xa8 pressed, increase speed
                }
            }
        }
    }
}


