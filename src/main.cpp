#include <Arduino.h>

#include <U8x8lib.h>  // Arduino Monochrome Graphics Library
U8X8_SSD1306_128X64_NONAME_SW_I2C
  oled(SSD1306_SCL, SSD1306_SDA, U8X8_PIN_NONE); // software I2C

#define TACT_PIN   LED_PIN  // same pin for TACT and LED
#define FONT_TEXT  u8x8_font_chroma48medium8_r

void setup() {
  pinMode(TACT_PIN, INPUT);  // init TACT switch
  oled.begin();                     // init OLED, bitbanged I2C bus
  oled.clear();                     // clear screen
  oled.setFont(FONT_TEXT);
}

#define COL(c,a)        (c)?(a%16):(a/8)
#define ROW(c,a)        (c)?(a/16):(a%8)
#define GLYPH(f,c,x,y)  header[f?16:(x==0||y==0)?c?x+y:x+2*y:17]
const uint8_t header[18] = {
  '0','1','2','3','4','5','6','7',
  '8','9','A','B','C','D','E','F',
  '+', // 16:found
  '.'  // 17:not found
};

void loop() {
  static bool colunmFirst = true;
  uint8_t addr, x, y;
  bool found;
  for (addr=0; addr<128; addr++) {  // full address spase
     x = COL(colunmFirst, addr);
     y = ROW(colunmFirst, addr);
     if (addr>7 && addr < 120) {
       found = addr%10 == 0;
     } else {
       found = false;
     }
     oled.drawGlyph(x, y, GLYPH(found,colunmFirst,x,y));
  }
  if (digitalRead(TACT_PIN) == HIGH) {
    colunmFirst = !colunmFirst;
    oled.clear();
  }
}
