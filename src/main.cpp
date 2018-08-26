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

bool scann(uint8_t addr){
  const uint8_t AM2321 = 0x5c;
  const uint8_t noError = 0x00;
  switch (addr) {
    case 0x00 ... 0x07: // first 8 addresses are reserved
    case 0x78 ... 0xFF: // last  8 addresses are reserved
      return false;
    case AM2321:      // try 2 times for DHT12/AM2320/AM2321
      TinyWireM.beginTransmission(addr);
      if (TinyWireM.endTransmission(1)==noError) { return true; }
      delay(5);
    default:
      TinyWireM.beginTransmission(addr);
      return (TinyWireM.endTransmission(1)==noError);
  }
}

#define HEX1(n)         ((n>9)?(n-10+'A'):(n+'0'))  // 0 .. 15 --> '0' .. 'F'
#define TEXT(c,x,y)     c?HEX1(x+y):HEX1(x+2*y)     // use FONT_TEXT
#define ICON(f)         f?'+':'.'                   // use FONT_TEXT
#define GLYPH(f,c,x,y)  f?'+':(x==0||y==0)?TEXT(c,x,y):'.'
#define COL(c,a)        (c)?(a%16):(a/8)
#define ROW(c,a)        (c)?(a/16):(a%8)
void loop() {
  static bool colunmFirst = true;
  uint8_t addr, x, y;
  bool found;
  for (addr=0; addr<128; addr++) {  // full address spase
    x = COL(colunmFirst, addr);
    y = ROW(colunmFirst, addr);
    found = scann(addr);
    oled.drawGlyph(x, y, GLYPH(found,colunmFirst,x,y));
  }
  if (digitalRead(TACT_PIN) == HIGH) {
    colunmFirst = !colunmFirst;
    oled.clear();
  }
}
