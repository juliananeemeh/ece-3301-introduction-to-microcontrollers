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
#define _XTAL_FREQ  8000000                                                             // Set operation for 8 Mhz


short Nec_OK = 0;                                                                       // variable declarations
char Nec_Button;
extern unsigned long long Nec_code;



char array1[21]={0xa2,0x62,0xe2,0x22,0x02,0xc2,0xe0,0xa8,0x90,0x68,0x98,0xb0,0x30,0x18,0x7a,0x10,0x38,0x5a,0x42,0x4a,0x52};     // array of button codes                          
char txt1[21][4] ={"CH-\0","CH \0","CH+\0","|<<\0",">>|\0",">||\0"," - \0"," + \0","EQ \0"," 0 \0","100\0","200\0"," 1 \0"," 2 \0"," 3 \0"," 4 \0"," 5 \0"," 6 \0"," 7 \0"," 8 \0"," 9 \0"};    // array of button text  
int color[21]={RD,RD,RD,CY,CY,GR,BU,BU,MA,BK,BK,BK,BK,BK,BK,BK,BK,BK,BK,BK};                                                    // array of button colors
char LED1[21] = {0x01,0x01,0x01,0x06,0x06,0x02,0x04,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};     // array of D1 colors
char LED2[21] = {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x20,0x28,0x38,0x38,0x38,0x38,0x38,0x00,0x00,0x00,0x00,0x00,0x00,0x00};     // array of D2 colors
char LED3[21] = {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x38,0x38,0x38,0x38,0x38,0x38,0x38};     // array of D3 colors
char LED4[21] = {0,0,0,0,0,0,0,0,0,1,0,0,1,1,1,1,1,1,1,1,1};                                                                    // array of D4 colors

void main()
{
    Init_UART();
    OSCCON = 0x70;                                                                      // 8 Mhz
    nRBPU = 0;                                                                          // Enable PORTB internal pull up resistor
    TRISB = 0x01;                                                                       // PORTB bit 1 input
    TRISC = 0x00;                                                                       // PORTC as output
    TRISD = 0x00;                                                                       // PORTD output
    ADCON1 = 0x0F;                                                                      // digital inputs
    PORTE = 0x00;                                                                       // PORTE = 0
    TRISE = 0x00;                                                                       // PORTE output
    TRISA = 0x00;                                                                       // PORTA output
    Initialize_LCD_Screen();                                                            // initialize screen
    Init_Interrupt();                                                                   // initialize interrupt

    Nec_code = 0x0;                                                                     // Clear code
    
    while(1)
    {
        if (Nec_OK == 1)                                                                // if nec ok = 1
        {
            Nec_OK = 0;                                                                 // set Nec ok = 0
            Enable_INT_Interrupt();                                                     // enable interrupt
            printf ("NEC_Button = %x \r\n", Nec_Button);                                // print button pressed and code

            char found = 0xff;                                                          // set found = 0xff
                
            // add code here to look for code using a FOR loop
            for (char i = 0; i< 21; i++)                                                // look for code of button pressed 
            {
                if(Nec_Button == array1[i])                                             // if button matches value in array of codes
                {
                    found = i;                                                          // found = array index
                }
            }
            
            if (found != 0xff) 
            {
				printf ("Key Location = %d \r\n\n", found);                             // print key location of button pressed
                fillCircle(Circle_X, Circle_Y, Circle_Size, color[found]);              // display button pressed on screen
                drawCircle(Circle_X, Circle_Y, Circle_Size, ST7735_WHITE);  
                drawtext(Text_X, Text_Y, txt1[found], ST7735_WHITE, ST7735_BLACK,TS_1);
   
			// add code to output color for the RGB LEDS
            PORTA = LED1[found] | LED2[found];                                          // output button pressed to correct led using led arrays
            PORTB = LED3[found];
            PORTCbits.RC2 = LED4[found];
            //or D1 and D2. shift D2 by 3 bits, D3 shift by 3 bits
			// add code to handle the KEY_PRESSED LED and do the buzzer sound
            
                KEY_PRESSED = 1;                                                       // key pressed led on
                Activate_Buzzer();                                                     // activate buzzer
                Wait_One_Sec();                                                        // wait one second
                Deactivate_Buzzer();                                                   // deactivate
                KEY_PRESSED = 0;                                                       // key pressed led off
                
            }
        }
    }
}


