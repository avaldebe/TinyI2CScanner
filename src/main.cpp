#include <Arduino.h>
#include <ssd1306xled.h>  // OLED library, bitbanged I2C
#include <TinyWireM.h>    // Scan haedware I2C bus
#define TACT_PIN LED_PIN  // same pin for TACT and LED

const uint8_t img8x8[2][8] PROGMEM ={        // squares
  {0x00,0x7E,0x42,0x42,0x42,0x42,0x7E,0x00}, // empty
  {0x00,0x7E,0x7E,0x7E,0x7E,0x7E,0x7E,0x00}  // full
};

void setup() {
  pinMode(TACT_PIN, INPUT);         // init TACT switch
  TinyWireM.begin();                // init hardware I2C buss
  SSD1306.ssd1306_init();           // init OLED, bitbanged I2C bus
  SSD1306.ssd1306_fillscreen(0x00); // clear screen
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
  if(colunmFirst){  // 7bit mode: show all addresses
    col = (addr&0x0F)<<3; // equiv (addr%16)*8
    row = addr>>4;        // equiv  addr/16
  }else{            // 8bit mode: show only even addresses
    col = addr&0xF8;      // equiv (addr/8)*8
    row = addr&0x07;      // equiv  addr%8
  }
  bool found = i2c_found(addr, 2, 5); // try 2 times for DHT12/AM2320/AM2321
  SSD1306.ssd1306_draw_bmp(col, row, col+8, row+1, img8x8[found?1:0]);
}

void loop() {
  static bool colunmFirst = true;
  for(uint8_t addr=8; addr<120; addr++){ // valid address space
    draw_address(addr, colunmFirst);
  }
  if(digitalRead(TACT_PIN) == HIGH){
    colunmFirst = !colunmFirst;
    SSD1306.ssd1306_fillscreen(0x00); // clear screen
  }else{
    delay(2000);
  }
}

