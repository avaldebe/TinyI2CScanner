# TinyI2CScanner
Micro I2C-Scanner with 0.96" Oled and ATtiny85

This is a re-implementation of the elektor labs project [Micro I2C-Scanner with 0.96" Oled and ATtiny44][labs].
At first I tried to port the tiny44 project to a tiny85, but was unable to make it work on a digispark clone.
The original implementation used some clever coding to fit the application on the tiny44.
Unfortunatelly, this made it hard to modify.

Besides the different uC,
this implementation intends to address some issues I had the original tiny44 proyect:

1. swap between 7bit I2C addresses and even 8bit addresses (original)
1. use an external library to handle the oled

The `tinyFont` branch re-implements the exact functionality of the [elektor labs project][labs].
It uses the backround bitmap and some lines of code from original project, and therefore the original lisence is kept.

The `tinusaur` branch contains a simpler implementation wich does not require a background bitmap,
and relies on printed text (on a sticker) placed around of the OLED display.

The `tinyFont` and `tinusaur` branches rely on the [tinusaur ssd1306xled][tinusaur]
library. They ose only a small fraction of the tiny85 memory.

The `master` branch uses the [u8g2][] library. It uses nearly all the tiny85 memory
(~80% data, ~95% program) to fit the headers for the address upper/lower nibbles.
On 8x8 character mode (`USE_U8X8` flag) the memory consuption is reduced
(~40% data, ~60% program), but the address headers share the space with the
first row/coulumns of the found/no-found indicators.

On all branches the I2C scanning function on the tiny85 is built araund the [TinyWireM][] library.

The `master branch` also contains buld envirnments fot the 3.3V/8Mhz version fo the
arduino pro-mini. See the [project configuration file](platformio.ini) for more details.

[labs]: http://www.elektormagazine.com/labs/micro-i2c-scanner-with-096-oled-and-attiny44-1
[u8g2]: https://github.com/olikraus/u8g2/wiki/u8g2reference
[tinusaur]: https://bitbucket.org/tinusaur/ssd1306xled
[TinyWireM]: https://github.com/adafruit/TinyWireM
