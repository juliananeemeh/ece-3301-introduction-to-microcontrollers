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
                                                                                        // include header files
#include "ST7735_TFT.h"
#include "Main_Screen.h"
#include "utils.h"
#include "Interrupt.h"
#include "main.h"
#include "I2C.h"
#include "I2C_Support.h"
#define _XTAL_FREQ  8000000                                                             // Set operation for 8 Mhz
                                                                                        // initialize variables
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



char array1[21]={0xa2,0x62,0xe2,0x22,0x02,0xc2,0xe0,0xa8,0x90,0x68,0x98,0xb0,0x30,0x18,0x7a,0x10,0x38,0x5a,0x42,0x4a,0x52};                               // find the code from pressing the buttons    TO DO
char txt1[21][4] ={"CH-\0","CH \0","CH+\0","|<<\0",">>|\0",">||\0"," - \0"," + \0","EQ \0"," 0 \0","100\0","200\0"," 1 \0"," 2 \0"," 3 \0"," 4 \0"," 5 \0"," 6 \0"," 7 \0"," 8 \0"," 9 \0"};   // insert text values for each button         TO DO
int color[21]={RD,RD,RD,CY,CY,GR,BU,BU,MA,BK,BK,BK,BK,BK,BK,BK,BK,BK,BK,BK};                                     // add the color of each button               TO DO
char LED1[21] = {0x01,0x01,0x01,0x06,0x06,0x02,0x04,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
char LED2[21] = {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x20,0x28,0x38,0x38,0x38,0x38,0x38,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
char LED3[21] = {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x38,0x38,0x38,0x38,0x38,0x38,0x38};
char LED4[21] = {0,0,0,0,0,0,0,0,0,1,0,0,1,1,1,1,1,1,1,1,1};
//   00000101   00101000 00000100 00100000
void main()
{
    Init_UART();
    OSCCON = 0x70;                                                                          // 8 Mhz
    nRBPU = 0;                                                                              // Enable PORTB internal pull up resistor
    TRISB = 0x01;                                                                           // set ports as inputs or outputs
    TRISC = 0x00;                           
    TRISD = 0x00;
    ADCON1 = 0x0F;                                                                          // set as digital inputs
    PORTE = 0x00;
    TRISE = 0x00;
    TRISA = 0x00;
    Initialize_LCD_Screen();                                                                // initialize LCD screen
    I2C_Init(100000);                                                                       // initialize I2C
    DS1621_Init();                          
    Init_Interrupt();                                                                       // initialize interrupt

    Nec_code = 0x0;                                                                         // Clear code
    //DS3231_Setup_Time();
    while(1)
    {
        // part 1
//        signed char tempC = DS1621_Read_Temp();
//        char tempC1 = DS1621_Read_Temp_Bad(); 
//        signed char tempF = (tempC * 9 / 5) + 32;
//        printf (" Temperature = %d degreesC = %d degreesF\r\n", tempC, tempF);
//        Wait_One_Sec_Soft(); 

        // part 2
        DS3231_Read_Time();                                                                 // read time from device
        if(tempSecond != second)
        {
            tempSecond = second;
            signed char tempC = DS1621_Read_Temp();                                         // read temperature in C
            signed char tempF = (tempC * 9 / 5) + 32;                                       // calculate temperature in F
            printf ("%02x:%02x:%02x %02x/%02x/%02x",hour,minute,second,month,day,year);     // print time
            printf (" Temperature = %d degreesC = %d degreesF\r\n", tempC, tempF);          // print temp
 } 


        if (Nec_OK == 1)
        {
            Nec_OK = 0;
            Enable_INT_Interrupt();
            printf ("NEC_Button = %x \r\n", Nec_Button);                                    // print button

            char found = 0xff;                                                              // initialize found
            
            // add code here to look for code using a FOR loop
            for (char i = 0; i< 21; i++)                                                    // loop through all the buttons
            {
                if(Nec_Button == array1[i])
                {
                    found = i;                                                              // find index of button pressed and set = found
                }
            }
            
            if (found != 0xff) 
            {
				printf ("Key Location = %d \r\n\n", found);                                 // print key pressed on teraterm  
                fillCircle(Circle_X, Circle_Y, Circle_Size, color[found]);                  // display button pressed on LCD
                drawCircle(Circle_X, Circle_Y, Circle_Size, ST7735_WHITE);  
                drawtext(Text_X, Text_Y, txt1[found], ST7735_WHITE, ST7735_BLACK,TS_1);
   
			// add code to output color for the RGB LEDS
            PORTA = LED1[found] | LED2[found];                                             // output color of button pressed to LED
            PORTB = LED3[found];
            PORTCbits.RC2 = LED4[found];                                                   // turn on LED if number is pressed
 
            
                KEY_PRESSED = 1;                                                            // beep routine when button pressed
                Activate_Buzzer();
                Wait_One_Sec();
                Deactivate_Buzzer();
                KEY_PRESSED = 0;
            
                if (Nec_Button == 0xa2)                                                     // if first button pressed, setup time
                {
                    DS3231_Setup_Time();
                }
            }
        }
    }
}


