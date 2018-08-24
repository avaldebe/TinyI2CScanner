#include <Arduino.h>

#include <U8g2lib.h>  // Arduino Monochrome Graphics Library
U8G2_SSD1306_128X64_NONAME_1_SW_I2C
  oled(U8G2_R0, SSD1306_SCL, SSD1306_SDA, U8X8_PIN_NONE); // software I2C

#include <TinyWireM.h>    // Scan haedware I2C bus
#define TACT_PIN LED_PIN  // same pin for TACT and LED

void setup() {
  pinMode(TACT_PIN, INPUT);         // init TACT switch
  TinyWireM.begin();                // init hardware I2C buss
  oled.begin();                     // init OLED, bitbanged I2C bus
  oled.clear();                     // clear screen
  oled.setFont(u8x8_font_5x8_n);
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

void draw_address(uint8_t addr, bool colunmFirst=true){
  if (colunmFirst) {  // 7bit mode: show all addresses
    oled.setCursor(addr%16, addr/16); // col, row
  } else {            // 8bit mode: show only even addresses
    oled.setCursor(addr/8, addr%8);   // col, row
  }
  bool found = i2c_found(addr, 2, 5); // try 2 times for DHT12/AM2320/AM2321
  oled.print(found?'*':'+');
}

void loop() {
  static bool colunmFirst = true;
  oled.firstPage();
  do {
    for (uint8_t addr=8; addr<120; addr++) {  // valid address space
      draw_address(addr, colunmFirst);
    }
  } while ( oled.nextPage() );
  if (digitalRead(TACT_PIN) == HIGH) {
    colunmFirst = !colunmFirst;
    oled.clear();                     // clear screen
  } else {
    delay(2000);
  }
}
