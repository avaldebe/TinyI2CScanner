#include <Arduino.h>
#include <TinyWireM.h>    // Scan haedware I2C bus
#include <U8x8lib.h>      // Arduino Monochrome Graphics Library
U8X8_SSD1306_128X64_NONAME_SW_I2C
  oled(SSD1306_SCL, SSD1306_SDA, U8X8_PIN_NONE); // software I2C

#define TACT_PIN   LED_PIN  // same pin for TACT and LED
#define FONT_TEXT  u8x8_font_chroma48medium8_u

void setup() {
  pinMode(TACT_PIN, INPUT);         // init TACT switch
  TinyWireM.begin();                // init hardware I2C buss
  oled.begin();                     // init OLED, bitbanged I2C bus
  oled.clear();                     // clear screen
  oled.setFont(FONT_TEXT);
  oled.println(F("TINY I2C SCANNER"));
}

bool i2c_found(uint8_t addr, uint8_t ntry=1, uint16_t msec=0){
  if (addr<8||addr>119) { return false; }  // firs/last 8 addr are reserved
  const uint8_t noError = 0x00;
  uint8_t n = 0;
  do { // test at least once
    TinyWireM.beginTransmission(addr);
    if (TinyWireM.endTransmission(1) == noError) { return true; }
    if(msec>0) delay(msec);
  } while (ntry>n++);
  return false;
}

#define HEX1(n)         ((n>9)?(n-10+'A'):(n+'0'))  // 0 .. 15 --> '0' .. 'F'
#define TEXT(c,x,y)     c?HEX1(x+y):HEX1(x+2*y)     // use FONT_TEXT
#define ICON(f)         f?'+':'.'                   // use FONT_TEXT
#define GLYPH(f,c,x,y)  f?'+':(x==0||y==0)?TEXT(c,x,y):'.'
#define COL(c,a)        (c)?(a%16):(a/8)
#define ROW(c,a)        (c)?(a/16):(a%8)
void loop() {
  static bool colunmFirst = true;
  const uint8_t AM2321 = 0x5c;
  uint8_t addr, x, y;
  bool found;
  for (addr=0; addr<128; addr++) {  // full address spase
     x = COL(colunmFirst, addr);
     y = ROW(colunmFirst, addr);
     found = (addr==AM2321)?i2c_found(addr, 2, 5):i2c_found(addr); // try 2 times for DHT12/AM2320/AM2321
     oled.drawGlyph(x, y, GLYPH(found,colunmFirst,x,y));
  }
  if (digitalRead(TACT_PIN) == HIGH) {
    colunmFirst = !colunmFirst;
    oled.clear();
  }
}
