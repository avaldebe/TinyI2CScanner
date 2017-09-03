#include <font6x8.h>
#include <font8x16.h>
#include <ssd1306xled.h>
#include <ssd1306xled8x16.h>

#include "img.h"

// OLED 128x64 pixels
#define OLED_MAX_X (128)
#define OLED_MAX_Y ( 64)

// SSD1306Device SSD1306;

void setup() {
  SSD1306.ssd1306_init();

  wellcome("I2C Scanner","7bit addresses");
  delay(1500);

  SSD1306.ssd1306_draw_bmp(0, 0, 128, 8, frame);
}

void loop() {

}

void wellcome(const char *title, const char *subtitle) {
  const byte x_mid = OLED_MAX_X/2;
  const byte y_mid = OLED_MAX_Y/16;

  SSD1306.ssd1306_fillscreen(0x00);

  byte mlen = 0;
  while(title[mlen]) mlen++;
  if(mlen>0){
    SSD1306.ssd1306_setpos(x_mid-mlen*3, y_mid-1);
    SSD1306.ssd1306_string_font6x8(title);
  }

  mlen = 0;
  while(subtitle[mlen]) mlen++;
  if(mlen>0){
    SSD1306.ssd1306_setpos(x_mid-mlen*3, y_mid+1);
    SSD1306.ssd1306_string_font6x8(subtitle);
  }
}
