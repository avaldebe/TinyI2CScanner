#include <Arduino.h>

// OLED library, bitbanged I2C
#include <ssd1306xled.h>

// Scan haedware I2C bus
#include <TinyWireM.h>

// Background image from tiny44 project
#include "img.h"

void setup() {
  // init hardware I2C buss
  TinyWireM.begin();

  // init OLED, bitbanged I2C bus
  SSD1306.ssd1306_init();

  // clean screen
  SSD1306.ssd1306_fillscreen(0x00);
}

void loop() {
  delay(2000);

  bool found[128] = {};
  uint8_t addr, error;
  for(addr=8; addr<120; addr++){ // valid address space
    TinyWireM.beginTransmission(addr);
    error = TinyWireM.endTransmission(1);
    found[addr] = (error == 0x00);
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
          if(found[addr])   fill|= 0b10000000;
          break;
        case 3 :
          if(found[addr-1]) fill = 0b00000111;
          if(found[addr])   fill|= 0b11000000;
          break;
        case 4 :
          if(found[addr-1]) fill = 0b00000011;
          if(found[addr])   fill|= 0b11100000;
          break;
        case 5 :
          if(found[addr-1]) fill = 0b00000001;
          if(found[addr])   fill|= 0b11110000;
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


