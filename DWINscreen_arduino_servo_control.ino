#include <Arduino.h>                  
#include <MsTimer2.h>                   // Serial output uses timer
#include "ServoEasing.hpp"

unsigned char tcount = 0;
unsigned char Buffer[80];
unsigned char Buffer_Len = 0;

int Header_1, Header_2;                 // Include data string MSB and LSB Header variables
unsigned char icon_on[8] =
{
  //header_1  header_2  Byte_Num  Write_Command  **Addres**   ---Data---
  0X5A,         0XA5,     0X05,      0X82,        0X30, 0X00,  0X00, 0X01
};//5A A5 05 82 3000 0001


unsigned char icon_off[8] =
{
  //header_1  header_2  Byte_Num  Write_Command  **Addres**   ---Data---
  0X5A,         0XA5,      0X05,      0X82,      0X30, 0X00,  0X00, 0X00
};
int Address = 0;                        //                                    (MSB, LSB )
#define SW_1_Address    0x00            // Only LSB Address Will Be Extracted (0x30 0x00)
#define SW_2_Address    0x01            // Only LSB Address Will Be Extracted (0x30 0x01)            
#define SW_3_Address    0x02            // Only LSB Address Will Be Extracted (0x30 0x02)
#define SW_4_Address    0x03            // Only LSB Address Will Be Extracted (0x30 0x02)

int Data = 0;                           //                                    (MSB, LSB )
#define Logic_1   0x01                  // Only LSB Address Will Be Extracted (0x00 0x01)
#define Logic_0   0x00                  // Only LSB Address Will Be Extracted (0x00 0x00)


ServoEasing Servo1;


void Timer2Interrupt()                  // 5ms interrupt
{
  if (tcount > 0)  tcount--;          
}

void setup()                            
{
  Serial.begin(115200);
  Serial1.begin(115200);                 //Must be Same as DWIN !
  pinMode(4, OUTPUT);                  
  pinMode(5, OUTPUT);                  
  pinMode(13, OUTPUT);                
  Servo1.attach(9, 90);
  MsTimer2::set(5, Timer2Interrupt);    // 5ms interupt
  MsTimer2::start();                    
  Servo1.setSpeed(50);
}
void blinkLED() {
  digitalWrite(LED_BUILTIN, HIGH);
  delay(100);
  digitalWrite(LED_BUILTIN, LOW);
  delay(100);
}
void loop()                           
{
  if (Serial1.available())               
  {
    Buffer[Buffer_Len] = Serial1.read(); 
    Buffer_Len++;                      
    tcount = 9;                         // number of bytes

    Header_1 = Buffer[0];             
    Header_2 = Buffer[1];               
    Address  = Buffer[5];               
    Data     = Buffer[8];            
  }
  else
  {
    if (tcount == 0)
    {
      if (Header_1 == 0X5A && Header_2 == 0xA5)           
      {
        switch (Address)                                 
        {   Servo1.setSpeed(50);  // This speed is taken if no further speed argument is given.

          case SW_1_Address:                             
            {
              if (Data == Logic_1)   Serial1.write(icon_on, 8);
              Servo1.easeTo(0);
              // Data 0 ise LED'i sondur
              for (int i = 0; i < 15; ++i) {
                blinkLED();
              }
            }
            break;

          case SW_2_Address:                     
            {
              if (Data == Logic_1)  Serial1.write(icon_off, 8);            
              if (Data == Logic_0)   digitalWrite(5, LOW);

            }
            break;

          case SW_3_Address:                      
            {
              Servo1.easeTo(180);
              Serial.println("sw3");
              while (Servo1.isMoving()) {
                blinkLED();
              }
            }
            break;

          case SW_4_Address:                            
            {
              Servo1.easeTo(0);
              Serial.println("sw4");
              while (Servo1.isMoving()) {
                blinkLED();
              }
            }
            break;
        }

        Buffer_Len = 0;                             //    clear buffer
      }
    }
  }
}
