// 128x64 OLED module, bitbanged I2C
#define OLED_MAX_X (128)
#define OLED_MAX_Y ( 64)

// OLED library, bitbanged I2C
#include <font6x8.h>
#include <font8x16.h>
#include <ssd1306xled.h>
#include <ssd1306xled8x16.h>

// Scan haedware I2C bus
#include <TinyWireM.h>

// Background image from tiny44 project
#include "img.h"

void setup() {
  // init hardware I2C buss
  TinyWireM.begin();

  // blink
  pinMode(LED_PIN, OUTPUT);

  // init OLED, bitbanged I2C bus
  SSD1306.ssd1306_init();

  // welcome message
  const byte x_mid = OLED_MAX_X/2;
  const byte y_mid = OLED_MAX_Y/(8*2);

  SSD1306.ssd1306_fillscreen(0x00);
  SSD1306.ssd1306_char_f8x16(x_mid-12*(8/2), y_mid-1,"I2C Scanner");
  SSD1306.ssd1306_setpos(x_mid-14*(6/2), y_mid+1);
  SSD1306.ssd1306_string_font6x8((char*)"7bit addresses");
}

void blink(uint16_t msec){
  digitalWrite(LED_PIN, HIGH);
  delay(msec);
  digitalWrite(LED_PIN, LOW);
}  

void loop() {
  delay(2000);

  bool found[128] = {};
  uint8_t addr, error;
  for(addr=0; addr<128; addr++){
    switch(addr){
    case 8 ... 119:
      TinyWireM.beginTransmission(addr);
      error = TinyWireM.endTransmission(1);
      found[addr] = (error == 0x00);
      if(found[addr]) blink(5000);
      break;
    default:
      // mark invalid addresses found, only for debugging
      found[addr] = true;
    }
  }
  
  SSD1306.ssd1306_setpos(0, 0);
  uint8_t da=0, nx=17, fill=0, fillcn=4;
  SSD1306.ssd1306_send_data_start();
  for(uint16_t y=0; y<8; y++){
    nx = 17;
    for(uint16_t x=0; x<128; x++){
      da = pgm_read_byte(&frame[x+(y*128)]);
      if((x == nx) && (y > 0)){
        addr = (((nx-17) / 7)<<3)+y;
        nx = nx+7;
        fillcn = 4;
        switch(y){
        case 1 :
          if(found[addr-1]) fill = 0b00011110;
          break;
        case 2 :
          if(found[addr-1]) fill = 0b00001111;
          if(found[addr])   fill = 0b10000000;
          break;
        case 3 :
          if(found[addr-1]) fill = 0b00000111;
          if(found[addr])   fill = 0b11000000;
          break;
        case 4 :
          if(found[addr-1]) fill = 0b00000011;
          if(found[addr])   fill = 0b11100000;
          break;
        case 5 :
          if(found[addr-1]) fill = 0b00000001;
          if(found[addr])   fill = 0b11110000;
          break;
        case 6 :
          if(found[addr])   fill = 0b01111000;
          break;
        case 7 :
          if(found[addr])   fill = 0b00111100;
          break;
        default:
          fillcn = 0;
        }
      }
      if(fillcn){
        da |= fill;
        fillcn--;
      }else{
        fill = 0;
      }
      SSD1306.ssd1306_send_byte(da);
    }
  }
  SSD1306.ssd1306_send_data_stop();
}


