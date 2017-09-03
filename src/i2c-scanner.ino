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

void loop() {
  delay(2000);

  // background image
  SSD1306.ssd1306_draw_bmp(0, 0, 128, 8, frame);

  digitalWrite(LED_PIN, HIGH); // start scan

  uint8_t addr, error;
  for(addr=8; addr<120; addr++){
    TinyWireM.beginTransmission(addr);
    error = TinyWireM.endTransmission(1);
    if(error == 0x00) {
      SSD1306.ssd1306_char_f8x16(0,0,"OK");
    }
  }

  digitalWrite(LED_PIN, LOW); // end scan
}
