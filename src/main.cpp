#include <Arduino.h>
#include <Wire.h>         // Scan haedware I2C bus
#include <U8g2lib.h>      // Arduino Monochrome Graphics Library
#if defined(SSD1306_SCL) && defined(SSD1306_SDA)
U8G2_SSD1306_128X64_NONAME_1_SW_I2C // software I2C
  oled(U8G2_R0, SSD1306_SCL, SSD1306_SDA, U8X8_PIN_NONE);
#else
U8G2_SSD1306_128X64_NONAME_1_HW_I2C // hardware I2C
  oled(U8G2_R0, U8X8_PIN_NONE);
#endif

void setup() {
#ifdef TACT_PIN
  pinMode(TACT_PIN, INPUT);         // init TACT switch
#endif
  Wire.begin();                     // init hardware I2C buss
  oled.begin();                     // init OLED, bitbanged I2C bus
  oled.clear();                     // clear screen
}

bool i2c_found(uint8_t addr, uint8_t ntry=1, uint16_t msec=0){
  if (addr<8||addr>119) { return false; }  // firs/last 8 addr are reserved
  const uint8_t noError = 0x00;
  uint8_t n = 0;
  do { // test at least once
    Wire.beginTransmission(addr);
    if (Wire.endTransmission(1) == noError) { return true; }
    if(msec>0) delay(msec);
  } while (ntry>n++);
  return false;
}

#define FONT_TEXT       u8g2_font_5x7_mr
#define FONT_ICON       u8g2_font_m2icon_7_tf
#define HEX1(n)         ((n>9)?(n-10+'A'):(n+'0'))  // 0 .. 15 --> '0' .. 'F'
#define TEXT(c,x,y)     c?HEX1(x+y):HEX1(x+2*y)     // use FONT_TEXT
#define ICON(f)         f?0x46:0x45                 // use FONT_ICON
#define COL(c,a)        (c)?(a%16):(a/8)            // low nibble  7b addr || high nibble 8b addr/2
#define ROW(c,a)        (c)?(a/16):(a%8)            // high nibble 7b addr || low nibble  8b addr/2
#define XPOS(n)         (n+0)*7                     // text/icon col --> x coord
#define YPOS(n)         (n+1)*7                     // text/icon row --> y coord
void loop() {
#ifdef TACT_PIN
  static bool colunmFirst = true;
  if (digitalRead(TACT_PIN) == HIGH) {
    colunmFirst = !colunmFirst;
    oled.clear();
  }
#else
  const bool colunmFirst = true;
#endif
  const uint8_t AM2321 = 0x5c;
  uint8_t addr, x, y;
  bool found;
  oled.firstPage();
  do {
    oled.setFont(FONT_TEXT);
    for (x=0, y=0; x<16; x++) {       // row0: header
      oled.drawGlyph(XPOS(x+1), YPOS(y), TEXT(colunmFirst,x,y));
    }
    for (x=0, y=0; y<8; y++) {        // col0: index
      oled.drawGlyph(XPOS(x), YPOS(y+1), TEXT(colunmFirst,x,y));
    }
    oled.setFont(FONT_ICON);
    for (addr=0; addr<128; addr++) {  // full address spase
      x = COL(colunmFirst, addr);
      y = ROW(colunmFirst, addr);
      found = (addr==AM2321)?i2c_found(addr, 2, 5):i2c_found(addr); // try 2 times for DHT12/AM2320/AM2321
      oled.drawGlyph(XPOS(x+1), YPOS(y+1), ICON(found));
    }
  } while ( oled.nextPage() );
}
