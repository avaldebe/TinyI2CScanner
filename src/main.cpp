#include <Arduino.h>

#include <U8x8lib.h>  // Arduino Monochrome Graphics Library
U8X8_SSD1306_128X64_NONAME_SW_I2C
  u8x8(SSD1306_SCL, SSD1306_SDA, U8X8_PIN_NONE); // software I2C

void setup() {
  u8x8.begin();                     // init OLED, bitbanged I2C bus
  u8x8.clear();                     // clear screen
  u8x8.setFont(u8x8_font_chroma48medium8_r);
  u8x8.println("OK");
}

void loop() {
}
