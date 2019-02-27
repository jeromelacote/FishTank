
**------------ FishTank -------------**

https://github.com/jeromelacote/FishTank

By mail@jeromelacote.com - more at www.thenewburo.com

and check www.victorise.com


This script:

- reads temperature
- stores temperature on a SD CARD
- trigger a relay (heater) if the temperature is within a range

 * SD card attached to SPI bus as follows:
 ** MOSI - pin 11
 ** MISO - pin 12
 ** CLK - pin 13
 ** CS - pin 4 (for MKRZero SD: SDCARD_SS_PIN)


Some random links for documentation and examples

- https://create.arduino.cc/projecthub/TheGadgetBoy/ds18b20-digital-temperature-sensor-and-arduino-9cc806
- http://playground.arduino.cc/Code/dateTime
- https://randomnerdtutorials.com/guide-to-sd-card-module-with-arduino/
- https://howtomechatronics.com/tutorials/arduino/control-high-voltage-devices-arduino-relay-tutorial/
- https://github.com/sparkfun/IoT_Power_Relay
- https://www.google.com/shopping/product/15196433662715787558?q=16%C3%972+LCD+Display+%26+Keypad+Shield&biw=1621&bih=756&prds=paur:ClkAsKraXxaA_92k1NMnRzUaoUh-Rs-MS_jptxp1gVrzPWOEAT6y1bCZI6bFEghy1N47alL_3OgeRA8AxHUREFSu8NUqHjRAx3Jlym14dC-9Ot6uoPyKPASBGxIZAFPVH72Fy3ArGnrw2__ggRLNWtNr4VFTgA&sa=X&ved=0ahUKEwjRuf_8vtvgAhVOsp4KHZU4BtMQ8wIIywI

Here is the basic idea for what we are building:
We hook the temperature sensor in, it stores temp data on SD card (some kind of time interval (not real time), Celsius, Farenheight).  Turn on blue LED light when below 75 degrees F. Turn on Green LED when 75-82. Turn on Red LED when above 82.  The only thing I need help on in this version of the product is the SD card coding part of it.  I haven't been able to get the temp data stored on the SD card.

This is the grand idea so you'll have to tell me if these are possibilities:
Along with what I mentioned above, I would also like it to send current temperature to an LCD screen in addition to the temp that we want to set the tank to. So if the current temp is 73 and we want it at 77, we would press a button to turn the heater on that we will have wired to it.  Once it hits that temperature, the heater would turn off.  Please let me know what you think of this part and if it is realistic or if anything about it needs to change.

the materials: 
Arduino Uno
the DS18B20 waterproof temperature monitor. and 5k resistor
WINGONEER 5PCS Micro SD TF card reader module SPI interface with chip level conversion
SanDisk Mobile Class4 MicroSDHC Flash Memory Card- SDSDQM-B35A with Adapter 16GB
IOT Power relay
LCD screen with Keypad

