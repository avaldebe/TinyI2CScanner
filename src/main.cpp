#include <Arduino.h>

// Arduino Monochrome Graphics Library
// OLED 128x64 displays
#define SSD1306 1306
#define SH1106 1106
#ifndef DISPLAY_128X64
#define DISPLAY_128X64 SSD1306
#endif
// software/hardware constructors for character/graphic mode
#if DISPLAY_128X64 == SSD1306
#define U8X8_SW_I2C U8X8_SSD1306_128X64_NONAME_SW_I2C   // character software I2C
#define U8X8_HW_I2C U8X8_SSD1306_128X64_NONAME_HW_I2C   // character hardware I2C
#define U8G2_SW_I2C U8G2_SSD1306_128X64_NONAME_1_SW_I2C // graphic software I2C
#define U8G2_HW_I2C U8G2_SSD1306_128X64_NONAME_1_HW_I2C // graphic hardware I2C
#elif DISPLAY_128X64 == SH1106
#define U8X8_SW_I2C U8X8_SH1106_128X64_NONAME_SW_I2C   // character software I2C
#define U8X8_HW_I2C U8X8_SH1106_128X64_NONAME_HW_I2C   // character hardware I2C
#define U8G2_SW_I2C U8G2_SH1106_128X64_NONAME_1_SW_I2C // graphic software I2C
#define U8G2_HW_I2C U8G2_SH1106_128X64_NONAME_1_HW_I2C // graphic hardware I2C
#else
#error "Unsupported DISPLAY_128X64"
#endif
// character/graphic mode
#ifdef USE_U8X8
#include <U8x8lib.h>
#define FONT_TEXT u8x8_font_chroma48medium8_u
#if defined(DISPLAY_SCL) && defined(DISPLAY_SDA)
U8X8_SW_I2C oled(DISPLAY_SCL, DISPLAY_SDA, U8X8_PIN_NONE);
#else
U8X8_HW_I2C oled(U8X8_PIN_NONE);
#endif
#else
#include <U8g2lib.h>
#define FONT_TEXT u8g2_font_5x7_mr
#if defined(DISPLAY_SCL) && defined(DISPLAY_SDA)
U8G2_SW_I2C oled(U8G2_R0, DISPLAY_SCL, DISPLAY_SDA, U8X8_PIN_NONE);
#else
U8G2_HW_I2C oled(U8G2_R0, U8X8_PIN_NONE);
#endif
#endif

// Scan hardware I2C bus
#ifdef __AVR_ATtiny85__
#include <TinyWireM.h>
#define _Wire TinyWireM
#else
#include <Wire.h>
#define _Wire Wire
#endif

class Scanner
{
private:
  uint8_t found[16] = {0}; // 16 * 8 bits = 128 bits to cover the full address space
  inline static uint8_t lnb(uint8_t addr) { return addr & 0x0F; }
  inline static uint8_t hnb(uint8_t addr) { return addr >> 4; }

protected:
  inline static bool check(uint8_t addr)
  {
    const uint8_t noError = 0x00;
    _Wire.beginTransmission(addr);
    return (_Wire.endTransmission() == noError);
  }

public:
  inline void begin(void) { _Wire.begin(); }
  inline void set(uint8_t addr)
  {
    bitWrite(found[lnb(addr)], hnb(addr), scan(addr));
  }
  inline bool get(uint8_t addr)
  {
    return bitRead(found[lnb(addr)], hnb(addr));
  }
  inline void scan(void)
  {
    for (uint8_t addr = 0; addr < 128; addr++)
    { // full address space
      set(addr);
    }
  }
  static bool scan(uint8_t addr)
  {
    const uint8_t AM2321 = 0x5c;
    switch (addr)
    {
    case 0x00 ... 0x07: // first 8 addresses are reserved
    case 0x78 ... 0xFF: // last  8 addresses are reserved
      return false;
    case AM2321: // try 2 times for DHT12/AM2320/AM2321
      if (check(addr))
      {
        return true;
      }
      delay(5);
    default:
      return check(addr);
    }
  }
} scanner;

void setup()
{
#ifdef TACT_PIN
  pinMode(TACT_PIN, INPUT); // init TACT switch
#endif
  scanner.begin(); // init hardware I2C buss
  oled.begin();    // init OLED, bitbanged I2C bus
  oled.clear();    // clear screen
  oled.setFont(FONT_TEXT);
#ifdef USE_U8X8
  oled.setCursor(0, 1);
  oled.print(F("TINY I2C SCANNER"));
  delay(1000);
#endif
}

#define HEX1(n) ((n > 9) ? (n - 10 + 'A') : (n + '0'))  // 0 .. 15 --> '0' .. 'F'
#define TEXT(c, x, y) c ? HEX1(x + y) : HEX1(x + 2 * y) // use FONT_TEXT
#define GLYPH(f, c, x, y) f ? '+' : (x == 0 || y == 0) ? TEXT(c, x, y) : '.'
#define COL(c, a) (c) ? (a % 16) : (a / 8) // low nibble  7b addr || high nibble 8b addr/2
#define ROW(c, a) (c) ? (a / 16) : (a % 8) // high nibble 7b addr || low nibble  8b addr/2
#define XPOS(n) (n + 0) * 7                // text/icon col --> x coord
#define YPOS(n) (n + 1) * 7                // text/icon row --> y coord
void loop()
{
#ifdef TACT_PIN
  static bool columnFirst = true;
  if (digitalRead(TACT_PIN) == HIGH)
  {
    columnFirst = !columnFirst;
    oled.clear();
  }
#else
  const bool columnFirst = true;
#endif
  scanner.scan(); // scan I2C bus outside the page/display loop
  uint8_t addr, x, y;
#ifdef USE_U8X8
  for (addr = 0; addr < 128; addr++)
  { // full address space
    x = COL(columnFirst, addr);
    y = ROW(columnFirst, addr);
    oled.drawGlyph(x, y, GLYPH(scanner.get(addr), columnFirst, x, y));
  }
#else
  oled.firstPage();
  do
  {
    oled.setFontDirection(1);
    oled.drawStr(XPOS(17), YPOS(0), "I2C SCANNER");
    oled.setFontDirection(0);
    for (x = 0, y = 0; x < 16; x++)
    { // row0: header
      oled.drawGlyph(XPOS(x + 1), YPOS(y), TEXT(columnFirst, x, y));
    }
    for (x = 0, y = 0; y < 8; y++)
    { // col0: index
      oled.drawGlyph(XPOS(x), YPOS(y + 1), TEXT(columnFirst, x, y));
    }
    for (addr = 0; addr < 128; addr++)
    { // full address space
      x = COL(columnFirst, addr);
      y = ROW(columnFirst, addr);
      if (scanner.get(addr))
      {
        oled.drawBox(XPOS(x + 1), YPOS(y) + 1, 6, 6);
      }
      else
      {
        oled.drawFrame(XPOS(x + 1), YPOS(y) + 1, 6, 6);
      }
    }
  } while (oled.nextPage());
#endif
}
