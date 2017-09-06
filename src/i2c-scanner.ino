#include <ssd1306xled.h>  // OLED library, bitbanged I2C
#include <TinyWireM.h>    // Scan haedware I2C bus


// Background image from tiny44 project
const uint8_t img8x8[2][8] PROGMEM ={        // squares
  {0x00,0x7E,0x42,0x42,0x42,0x42,0x7E,0x00}, // empty
  {0x00,0x7E,0x7E,0x7E,0x7E,0x7E,0x7E,0x00}  // full
};

void setup() {
  TinyWireM.begin();                // init hardware I2C buss
  SSD1306.ssd1306_init();           // init OLED, bitbanged I2C bus
  SSD1306.ssd1306_fillscreen(0x00); // clear screen
}

bool i2c_found(uint8_t addr){
  TinyWireM.beginTransmission(addr);
  uint8_t error = TinyWireM.endTransmission(1);
  return (error==0x00);  
}

void loop() {
  uint8_t addr, col, row;
  bool found;
  for(addr=8; addr<120; addr++){    // valid address space
    col = addr%16*8;
    row = addr/16;
    found = i2c_found(addr);
    SSD1306.ssd1306_draw_bmp(col, row, col+8, row+1, img8x8[found?1:0]);
  }
  delay(2000);
}

