#include <Arduino.h>

#include <U8g2lib.h>  // Arduino Monochrome Graphics Library
U8G2_SSD1306_128X64_NONAME_1_SW_I2C
  oled(U8G2_R0, SSD1306_SCL, SSD1306_SDA, U8X8_PIN_NONE); // software I2C

#define FONT_TEXT     u8g2_font_5x7_mr
#define FONT_ICON     u8g2_font_m2icon_7_tf
#define COL(col)      (7*(col))
#define ROW(row)      (7*(row+1))

#include <TinyWireM.h>    // Scan haedware I2C bus
#define TACT_PIN LED_PIN  // same pin for TACT and LED

void setup() {
  pinMode(TACT_PIN, INPUT_PULLUP);  // init TACT switch
  TinyWireM.begin();                // init hardware I2C buss
  oled.begin();                     // init OLED, bitbanged I2C bus
  oled.clear();                     // clear screen
}

bool i2c_found(uint8_t addr, uint8_t ntry=1, uint16_t msec=0){
  const uint8_t noError = 0x00;
  uint8_t n = 0;
  bool found;
  do { // test at least once
    TinyWireM.beginTransmission(addr);
    found = (TinyWireM.endTransmission(1) == noError);
    if (msec>0) { delay(msec); }
  } while (ntry>n++ && not found);
  return found;
}

void loop() {
  const char header[] = "0123456789ABCDEF";
  static bool colunmFirst = true;
  uint8_t n, addr;
  bool found;

  oled.firstPage();
  do {
    // header/index
    oled.setFont(FONT_TEXT);
    // print header (1st row)
    for (n = 0; n<16; n++){
      oled.drawGlyph(COL(n+1), ROW(0), header[n]);
    }
    // print index (1st column)
    for (n = 0; n<8; n++){
      oled.drawGlyph(COL(0), ROW(n+1), colunmFirst?header[n]:header[n*2]);
    }
    oled.setFont(FONT_ICON);
    for (addr=8; addr<120; addr++) {  // full address spase
      found = i2c_found(addr, 2, 5);  // try 2 times for DHT12/AM2320/AM2321
      oled.drawGlyph(
        colunmFirst?COL(addr%16+1):COL(addr/8+1),
        colunmFirst?ROW(addr/16+1):ROW(addr%8+1),
        found?'*':'+'
      );
    }
  } while ( oled.nextPage() );
  if (digitalRead(TACT_PIN) == LOW) {
    colunmFirst = !colunmFirst;
    oled.clear();                     // clear screen
  } else {
    delay(2000);
  }
}
