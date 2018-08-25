#include <Arduino.h>

#include <U8x8lib.h>  // Arduino Monochrome Graphics Library
U8X8_SSD1306_128X64_NONAME_SW_I2C
  oled(SSD1306_SCL, SSD1306_SDA, U8X8_PIN_NONE); // software I2C

#define FONT_TEXT     u8x8_font_chroma48medium8_r
void setup() {
  oled.begin();                     // init OLED, bitbanged I2C bus
  oled.clear();                     // clear screen
  oled.setFont(FONT_TEXT);
}

#define COL(c,a)        (c)?(a%16):(a/8)
#define ROW(c,a)        (c)?(a/16):(a%8)
#define GLYPH(f,c,x,y)  f?'+':(x==0||y==0)?c?header[x+y]:header[x+2*y]:'.'
const char header[] = "0123456789ABCDEF";
void loop() {
  static bool columnfirst = true;
  uint8_t addr, x, y;
  bool found;
  for (addr=0; addr<128; addr++) {  // full address spase
     x = COL(columnfirst, addr);
     y = ROW(columnfirst, addr);
     if (addr>7 && addr < 120) {
       found = addr%10 == 0;
     } else {
       found = false;
     }
     oled.drawGlyph(x, y, GLYPH(found,columnfirst,x,y));
   }
   oled.clear();
   columnfirst = !columnfirst;
}
