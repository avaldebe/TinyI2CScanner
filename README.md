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

The [tinyFont][] branch re-implements the exact functionality of the [elektor labs project][labs].
It uses the backround bitmap and some lines of code from original project, and therefore the original lisence is kept.

The master branch contains a simpler implementation wich does not require a background bitmap,
and relies on printed text (on a sticker) placed around of the OLED display.

[labs]: http://www.elektormagazine.com/labs/micro-i2c-scanner-with-096-oled-and-attiny44-1
[oled]: https://bitbucket.org/tinusaur/ssd1306xled
[tinyFont]: https://github.com/avaldebe/TinyI2CScanner/tree/tinyFont
