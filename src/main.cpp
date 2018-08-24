#include <Arduino.h>

#include <U8x8lib.h>  // Arduino Monochrome Graphics Library
U8X8_SSD1306_128X64_NONAME_SW_I2C
  u8x8(SSD1306_SCL, SSD1306_SDA, U8X8_PIN_NONE); // software I2C

#include <TinyWireM.h>    // Scan haedware I2C bus
#define TACT_PIN LED_PIN  // same pin for TACT and LED

const uint8_t img8x8[2][8] = {       // squares
  {0x00,0x7E,0x42,0x42,0x42,0x42,0x7E,0x00}, // empty
  {0x00,0x7E,0x7E,0x7E,0x7E,0x7E,0x7E,0x00}  // full
};

void setup() {
  pinMode(TACT_PIN, INPUT);         // init TACT switch
  TinyWireM.begin();                // init hardware I2C buss
  u8x8.begin();                     // init OLED, bitbanged I2C bus
  u8x8.clear();                     // clear screen
}

bool i2c_found(uint8_t addr, uint8_t ntry=1, uint16_t msec=0){
  const uint8_t noError = 0x00;
  uint8_t n = 0;
  bool found;
  do { // test at least once
    TinyWireM.beginTransmission(addr);
    found = (TinyWireM.endTransmission(1) == noError);
    if(msec>0) delay(msec);
  } while(ntry>n++ && not found);
  return found;
}

void draw_address(uint8_t addr, bool colunmFirst=true){
  uint8_t col, row;
  if (colunmFirst) {  // 7bit mode: show all addresses
    col = addr%16;
    row = addr/16;
  } else {            // 8bit mode: show only even addresses
    col = addr/8;
    row = addr%8;
  }
  bool found = i2c_found(addr, 2, 5); // try 2 times for DHT12/AM2320/AM2321
  u8x8.drawTile(col, row, 1, img8x8[found?1:0]);
}

void loop() {
  static bool colunmFirst = true;
  for(uint8_t addr=8; addr<120; addr++){ // valid address space
    draw_address(addr, colunmFirst);
  }
  if(digitalRead(TACT_PIN) == LOW){
    colunmFirst = !colunmFirst;
    u8x8.clear();                     // clear screen
  }else{
    delay(2000);
  }
}
