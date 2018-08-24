#include <Arduino.h>

#include <U8g2lib.h>  // Arduino Monochrome Graphics Library
U8G2_SSD1306_128X64_NONAME_1_SW_I2C
  oled(U8G2_R0, SSD1306_SCL, SSD1306_SDA, U8X8_PIN_NONE); // software I2C

#define FONT          u8g2_font_5x7_mr
#define FONT_HEIGHT   7
#define FONT_WIDTH    5

#include <TinyWireM.h>    // Scan haedware I2C bus
#define TACT_PIN LED_PIN  // same pin for TACT and LED

void setup() {
  pinMode(TACT_PIN, INPUT);         // init TACT switch
  TinyWireM.begin();                // init hardware I2C buss
  oled.begin();                     // init OLED, bitbanged I2C bus
  oled.clear();                     // clear screen
  oled.setFont(FONT);
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
  uint8_t addr, col, row;
  oled.firstPage();
  do {
    for (addr=8; addr<120; addr++) {  // valid address space
      col = addr%16;
      row = addr/16;
      oled.setCursor(col*FONT_HEIGHT, row*FONT_WIDTH);
      bool found = i2c_found(addr, 2, 5);     // try 2 times for DHT12/AM2320/AM2321
      oled.print(found?'*':'+');
    }
  } while ( oled.nextPage() );
  delay(2000);
}
