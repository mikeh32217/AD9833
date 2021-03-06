# AD9833
A C++ library for the AD9833 Signal Generator breakout board for the Arduino UNO

 The AD9833 Signal Generator is a relatively inexpensive low power DDS device that uses the 3 wire SPI interface. The device provides a stable; Sine, Sawtooth or Square wave signal from 0 to 12.5MHz with a resolution of 0.1Hz

It took me a while to get the communication working because the Analog Devices data sheet I had was just plain wrong and I'm the kind of person that only reads the manual when things go wrong. In the datasheet, for the SPI interface the authors claim that the clock should idle in the high state further stating that CPOL=0 and CPHA=1 which is totally wrong. According to the interface when the clock is to idle in the hig state CPOL should be set high and CPHA set low. When I changed the code to match the inteface things started working.

The firmware provided in the download is pretty straight forward and easy to use. After the device and been configured and reset the registers may be changed at will and no further reset is necessary. In the example provided I demonstrate some of the functionality of this class, additionally he frequency, phase and mode may be changed on the fly. 
