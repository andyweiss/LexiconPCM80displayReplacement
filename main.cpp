#include <Arduino.h>
#include "main.h"
#include <U8g2lib.h>
#include <SPI.h>

// set up Display
U8G2_SSD1322_NHD_256X64_F_4W_HW_SPI u8g2(U8G2_R0, /* cs=*/ 32, /* dc=*/ 33, /* reset=*/ 35  );    // Enable U8G2_16BIT in u8g2.h
// SPI Clock is on 18
// MOSI is on 23


void setup() {
    u8g2.begin();
    // u8g2.setFont(u8g2_font_etl14thai_t);   
    u8g2.setFont(u8g2_font_courR12_tf);                  
    for(int i =0;i<8;i++){
        pinMode(databus[i],INPUT);
    }
    for(int i =0;i<40;i++){
        character[i]=32;        // clear char buffer
    }
 

    pinMode(DISPDWR, INPUT);     //  interrupt for new data DISPDWR
    pinMode(DISPBSY, OUTPUT);    // generate DISPBSY signal
    attachInterrupt(digitalPinToInterrupt(DISPDWR), NewDataInterrupt, FALLING);
    Serial.begin(115200);
    Serial.println("I'm alive!!!!");
    
}

void loop() {
    
  delay(10);  
  u8g2.sendBuffer();                
}



void ParseData(void * parameter)
{
    switch (data) {
            case 0x04:                          // DIM  not testet (not used by PCM80)
                mode=data;
                break;
            
            case 0x08:                          // Backspace  not testet (not used by PCM80)
                if(position<21)
                {
                   // u8g2.drawStr( 6+9*position, 22, " ");
                }
                else
                { 
                   // u8g2.drawStr( 6+9*(position-20), 50, " ");
                }
                position--;
                if(position==255)
                {
                    position=39;
                }
                
                break;
            
            case 0x09:         // Horizontal Tap not testet (not used by PCM80)
                position++;
                if(position==40)
                {
                    position=0;
                }
                break;
                
            case 0x1d:         // cursor mode  not testet (not used by PCM80)
                u8g2.clearBuffer();
                position=0;
                break;
            
            case 0x10:         // Display position
                mode=data;
                break;
            
            case 0x17:         // cursor mode  not testet (not used by PCM80)
                mode=data;
                break;
                
            case 0x0f:         // all display
                u8g2.clearBuffer();
                u8g2.drawStr( 9, 24, "PCM80 Display replacement:");
                u8g2.drawStr( 9, 46, "2020 by Andy Weiss");
                break;
                
            case 0x1f:         // reset
                u8g2.clearBuffer();
                position=0;
                dimlevel=0;
                cursormode=0;
                break;
    
            default:            // write char
                if(position<20){
                    char tmp[3];
                    sprintf(tmp,"%c",data);
                    u8g2.setDrawColor(0);
                    u8g2.drawBox(3+12*position, 12, 12,16);
                    u8g2.setDrawColor(1);
                    if (data == 159){
                        u8g2.drawBox(5+12*position, 16, 4,4);
                    }
                    else{
                        u8g2.drawStr(3+12*position, 24, tmp);
                    }
                                    
                }
                else
                { 
                    char temp[3];
                    sprintf(temp,"%c",data);
                     u8g2.setDrawColor(0);
                     u8g2.drawBox(3+12*(position-20), 40, 12,16);
                     u8g2.setDrawColor(1);
                     if (data == 159){
                        u8g2.drawBox(3+12*(position-20), 48, 4,4);
                    }
                    else{
                        u8g2.drawStr(3+12*(position-20), 52, temp);
                    }
                }
                position++;
                if(position==40)
                {
                    position=0;
                }
                break;
        }
        digitalWrite(DISPBSY, 0);
        vTaskDelete( NULL );
}

void IRAM_ATTR NewDataInterrupt()
{
    digitalWrite(DISPBSY, 1);           // tell PCM80 we are busy
    uint8_t tmp=0;
    data = 0;
      for(int i =0;i<8;i++){
          tmp = digitalRead(databus[i]);
          data += (tmp << i);
      }

    switch (mode) {

        case 0x04:         // DIM (not used by PCM80)
            u8g2.setContrast(data);
            mode=0;
            digitalWrite(DISPBSY, 0);
            break;
        
        case 0x10:         // Display position
            position=data;
            mode=0;
            digitalWrite(DISPBSY, 0);
            break;
        
        case 0x17:         // cursor mode (not used by PCM80)
            switch (data) {
                case 0xFF:      // lighting
                    cursormode= 2;
                    break;
                case 0x88:      // blinking
                    cursormode= 1;
                    break;
                case 0x00:      //no lighting
                    cursormode= 0;
                    break;
                default:
                    break;
            }
            mode=0;
            digitalWrite(DISPBSY, 0);
            break;
         
        default:           // Parse data
            xTaskCreate(ParseData, "Parse_Task", 10000, NULL, 1, NULL);
            break;
    }
}

/*                      
void DrawCursor()
{
    
    
}
*/