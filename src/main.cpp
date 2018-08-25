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

#define TACT_PIN   LED_BUILTIN  // same pin for TACT and LED
#define FONT_TEXT  u8g2_font_amstrad_cpc_extended_8u

void setup() {
  pinMode(TACT_PIN, INPUT);         // init TACT switch
  Wire.begin();                // init hardware I2C buss
  oled.begin();                     // init OLED, bitbanged I2C bus
  oled.clear();                     // clear screen
  oled.setFont(FONT_TEXT);
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

#define COL(c,a)        (c)?(a%16):(a/8)
#define ROW(c,a)        (c)?(a/16):(a%8)
#define GLYPH(f,c,x,y)  header[f?16:(x==0||y==0)?c?x+y:x+2*y:17]
#define XPOS(x)         (x+0)*8
#define YPOS(x)         (y+1)*8
const uint8_t header[18] = {
  '0','1','2','3','4','5','6','7',
  '8','9','A','B','C','D','E','F',
  '+', // 16:found
  '.'  // 17:not found
};
void loop() {
  static bool colunmFirst = true;
  const uint8_t AM2321 = 0x5c;
  uint8_t addr, x, y;
  bool found;
  oled.firstPage();
  do {
    for (addr=0; addr<128; addr++) {  // full address spase
       x = COL(colunmFirst, addr);
       y = ROW(colunmFirst, addr);
       found = (addr==AM2321)?i2c_found(addr, 2, 5):i2c_found(addr); // try 2 times for DHT12/AM2320/AM2321
       oled.drawGlyph(XPOS(x), YPOS(y), GLYPH(found,colunmFirst,x,y));
    }
  } while ( oled.nextPage() );
  if (digitalRead(TACT_PIN) == HIGH) {
    colunmFirst = !colunmFirst;
    oled.clear();
  }
}
