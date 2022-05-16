#include <Arduino.h>                    // Arduino Kütüphanesi
#include <MsTimer2.h>                   // Serial output uses timer
#include "ServoEasing.hpp"

unsigned char tcount = 0;
unsigned char Buffer[80];
unsigned char Buffer_Len = 0;

int Header_1, Header_2;                 // Veri katarını MSB ve LSB Başlık değişkenleri
unsigned char icon_on[8] =
{
  //Bslk_1  Bslk_2  Byte_Sysi  Yaz_kmtu  **Adres**   ---Veri---
  0X5A,   0XA5,   0X05,      0X82,      0X30, 0X00,  0X00, 0X01
};//5A A5 05 82 3000 0001

// DWIN de 0x1001 adresinde bulunan 0 indeksli iconu calistiran dizi
unsigned char icon_off[8] =
{
  //Bslk_1  Bslk_2  Byte_Sysi  Yaz_kmtu  **Adres**   ---Veri---
  0X5A,   0XA5,   0X05,      0X82,      0X30, 0X00,  0X00, 0X00
};
int Address = 0;                        //                               (MSB, LSB )
#define SW_1_Address    0x00            // Sadece LSB Adresi Ayıklanacak (0x20 0x00)
#define SW_2_Address    0x01            // Sadece LSB Adresi Ayıklanacak (0x20 0x01)            
#define SW_3_Address    0x02            // Sadece LSB Adresi Ayıklanacak (0x20 0x02)
#define SW_4_Address    0x03            // Sadece LSB Adresi Ayıklanacak (0x20 0x02)

int Data = 0;                           //                               (MSB, LSB )
#define Logic_1   0x01                  // Sadece LSB Datası Ayıklanacak (0x00 0x01)
#define Logic_0   0x00                  // Sadece LSB Datası Ayıklanacak (0x00 0x00)


ServoEasing Servo1;


void Timer2Interrupt()                  // 5ms 'lik kesme ayari
{
  if (tcount > 0)  tcount--;            // Touch sayacını azalt
}

void setup()                            // Haberlesme ve LED pini ayarlari
{
  Serial.begin(115200);
  Serial1.begin(115200);                 // Baud Rate ayari (DWIN ile Aynı olmalı !)
  pinMode(4, OUTPUT);                   // 11. pini cikis olarak aya
  pinMode(5, OUTPUT);                   // 12. pini cikis olarak aya
  pinMode(13, OUTPUT);                   // 13. pini cikis olarak ayarla
  Servo1.attach(9, 90);
  MsTimer2::set(5, Timer2Interrupt);    // 5ms 'lik kesme ayari
  MsTimer2::start();                    // Timeri baslat
  Servo1.setSpeed(50);
}
void blinkLED() {
  digitalWrite(LED_BUILTIN, HIGH);
  delay(100);
  digitalWrite(LED_BUILTIN, LOW);
  delay(100);
}
void loop()                             // Sonsuz dongu
{
  if (Serial1.available())               // Seri Haberleşme Kesmesi
  {
    Buffer[Buffer_Len] = Serial1.read(); // Okunan Byte', diziye kaydet
    Buffer_Len++;                       // Diziyi kayıdır
    tcount = 9;                         // Touch Sayacına 9 değerinin yükle (Çekilecek Byte sayısı)

    Header_1 = Buffer[0];               // Başlık_MSB verisini ayıkla
    Header_2 = Buffer[1];               // Başlık_LSB verisini ayıkla
    Address  = Buffer[5];               // Address_LSB verisini ayıkla
    Data     = Buffer[8];               // Data_LSB verisini ayıkla ve
  }
  else
  {
    if (tcount == 0)
    {
      if (Header_1 == 0X5A && Header_2 == 0xA5)           // Veri basligi geldi ise
      {
        switch (Address)                                  // Adresleri sorgula
        {   Servo1.setSpeed(50);  // This speed is taken if no further speed argument is given.

          case SW_1_Address:                              // Switch_1 adresi geldi ise
            {
              if (Data == Logic_1)   Serial1.write(icon_on, 8); // Data 1 ise LED'i yak
              Servo1.easeTo(0);
              // Data 0 ise LED'i sondur
              for (int i = 0; i < 15; ++i) {
                blinkLED();
              }
            }
            break;

          case SW_2_Address:                              // Switch_2 adresi geldi ise
            {
              if (Data == Logic_1)  Serial1.write(icon_off, 8);            if (Data == Logic_0)   digitalWrite(5, LOW);

            }
            break;

          case SW_3_Address:                              // Switch_2 adresi geldi ise
            {
              Servo1.easeTo(180);
              Serial.println("sw3");
              while (Servo1.isMoving()) {
                blinkLED();
              }
            }
            break;

          case SW_4_Address:                              // Switch_2 adresi geldi ise
            {
              Servo1.easeTo(0);
              Serial.println("sw4");
              while (Servo1.isMoving()) {
                blinkLED();
              }
            }
            break;
        }

        Buffer_Len = 0;                                   // Bufferi Temizle
      }
    }
  }
}
