#include <Arduino.h>

// Arduino Monochrome Graphics Library
#ifdef USE_U8X8
  #include <U8x8lib.h>
  #define FONT_TEXT  u8x8_font_chroma48medium8_u
  #if defined(SSD1306_SCL) && defined(SSD1306_SDA)
    U8X8_SSD1306_128X64_NONAME_SW_I2C   // software I2C
      oled(SSD1306_SCL, SSD1306_SDA, U8X8_PIN_NONE);
  #else
    U8X8_SSD1306_128X64_NONAME_HW_I2C   // hardware I2C
      oled(U8X8_PIN_NONE);
  #endif
#else
  #include <U8g2lib.h>
  #define FONT_TEXT  u8g2_font_5x7_mr
  #if defined(SSD1306_SCL) && defined(SSD1306_SDA)
    U8G2_SSD1306_128X64_NONAME_1_SW_I2C // software I2C
      oled(U8G2_R0, SSD1306_SCL, SSD1306_SDA, U8X8_PIN_NONE);
  #else
    U8G2_SSD1306_128X64_NONAME_1_HW_I2C // hardware I2C
      oled(U8G2_R0, U8X8_PIN_NONE);
  #endif
#endif

// Scan haedware I2C bus
#ifdef __AVR_ATtiny85__
  #include <TinyWireM.h>
  #define _Wire TinyWireM
#else
  #include <Wire.h>
  #define _Wire Wire
#endif

class Scanner {
private: 
  uint8_t found[16]; // 16 * 8 bits = 128 bits to cover the full address space
  inline uint8_t lnb (uint8_t addr) { return addr&0x0F; }
  inline uint8_t hnb (uint8_t addr) { return addr>>4; }

public:
  inline void set (uint8_t addr) {
    bitWrite(found[lnb(addr)], hnb(addr), scann(addr));
  }
  inline bool get (uint8_t addr) {
    return bitRead(found[lnb(addr)], hnb(addr));
  }
  inline void begin (void) { _Wire.begin(); }
  static bool scann (uint8_t addr) {
    const uint8_t AM2321 = 0x5c;
    const uint8_t noError = 0x00;
    switch (addr) {
      case 0x00 ... 0x07: // first 8 addresses are reserved
      case 0x78 ... 0xFF: // last  8 addresses are reserved
        return false;
      case AM2321:        // try 2 times for DHT12/AM2320/AM2321
        _Wire.beginTransmission(addr);
        if (_Wire.endTransmission()==noError) { return true; }
        delay(5);
      default:
        _Wire.beginTransmission(addr);
        return (_Wire.endTransmission()==noError);
    }
  }
  inline void scann (void){
    for (uint8_t addr=0; addr<128; addr++) {  // full address spase
      set(addr);
    }
  }
} scanner;

void setup() {
#ifdef TACT_PIN
  pinMode(TACT_PIN, INPUT);         // init TACT switch
#endif
  scanner.begin();                  // init hardware I2C buss
  oled.begin();                     // init OLED, bitbanged I2C bus
  oled.clear();                     // clear screen
  oled.setFont(FONT_TEXT);
#ifdef USE_U8X8
  oled.setCursor(0,1);
  oled.print(F("TINY I2C SCANNER"));
  delay(1000);
#endif
}

#define HEX1(n)         ((n>9)?(n-10+'A'):(n+'0'))  // 0 .. 15 --> '0' .. 'F'
#define TEXT(c,x,y)     c?HEX1(x+y):HEX1(x+2*y)     // use FONT_TEXT
#define GLYPH(f,c,x,y)  f?'+':(x==0||y==0)?TEXT(c,x,y):'.'
#define COL(c,a)        (c)?(a%16):(a/8)   // low nibble  7b addr || high nibble 8b addr/2
#define ROW(c,a)        (c)?(a/16):(a%8)   // high nibble 7b addr || low nibble  8b addr/2
#define XPOS(n)         (n+0)*7            // text/icon col --> x coord
#define YPOS(n)         (n+1)*7            // text/icon row --> y coord
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
  scanner.scann();  // scann I2C bus outside the page/display loop
  uint8_t addr, x, y;
#ifdef USE_U8X8
  for (addr=0; addr<128; addr++) {  // full address spase
    x = COL(colunmFirst, addr);
    y = ROW(colunmFirst, addr);
    oled.drawGlyph(x, y, GLYPH(scanner.get(addr),colunmFirst,x,y));
  }
#else
  oled.firstPage();
  do {
    oled.setFontDirection(1);
    oled.drawStr(XPOS(17), YPOS(0), "I2C SCANNER");
    oled.setFontDirection(0);
    for (x=0, y=0; x<16; x++) {       // row0: header
      oled.drawGlyph(XPOS(x+1), YPOS(y), TEXT(colunmFirst,x,y));
    }
    for (x=0, y=0; y<8; y++) {        // col0: index
      oled.drawGlyph(XPOS(x), YPOS(y+1), TEXT(colunmFirst,x,y));
    }
    for (addr=0; addr<128; addr++) {  // full address spase
      x = COL(colunmFirst, addr);
      y = ROW(colunmFirst, addr);
      if (scanner.get(addr)) {
        oled.drawBox  (XPOS(x+1), YPOS(y)+1, 6, 6);
      } else {
        oled.drawFrame(XPOS(x+1), YPOS(y)+1, 6, 6);
      }
    }
  } while ( oled.nextPage() );
#endif
}
