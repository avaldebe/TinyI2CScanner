#include <Arduino.h>

#include <U8x8lib.h>  // Arduino Monochrome Graphics Library
U8X8_SSD1306_128X64_NONAME_SW_I2C
  oled(SSD1306_SCL, SSD1306_SDA, U8X8_PIN_NONE); // software I2C

void setup() {
  oled.begin();                     // init OLED, bitbanged I2C bus
  oled.clear();                     // clear screen
  oled.setFont(u8x8_font_chroma48medium8_r);
  oled.println("OK");
}

void loop() {
}
