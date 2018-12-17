# neo60esp_clock
Circular clock using 60 neopixel ring and an ESP8266

Make a circle out of the 4 segments of the neopixel ring - 
connect +5V and GND together all the way round, and all but one DIN to DOUT

Connect the 5V and GND to NodeMCU or whatever ESP8266 you are using
Connect DIN to pin GPIO2 on Node MCU, labelled as D4 sometimes.

Hook up USB power supply.  
Look for wireless AP with SSID: NeoPixel_Clock

Connect to the AP with your phone, it should redirect to a page which will allow you to enter
the details of your own wireless network.

Enjoy 
Richard Shipman
17th December 2018
