# simracing_wireless_buttons
Implementation of wireless buttons for a simracing wheel using two HC-05 bluetooth modules and two Teensy microcontrollers.

BOM: Buttons, paddle shifters, wires, soldering iron and solder. Something to attach the buttons to. Battery connector. Two leds with suitable series resistors and two resistors to get battery voltage monitoring voltage divider circuit working. One 1S Lithium-Polymer battery is required to power up the system at the wheel.

Requires two Teensy microcontrollers connected with two HC-05 (or compatible) bluetooth modules, with Serial connection 
at Teensy pins 0 and 1. Connection status is connected to pin 2. 

Please adjust the two HC-05 bluetooth modules to pair with each other beforehand. There are various types of HC-05 modules around. To access AT mode to setup, some require manually soldering a wire at pin 34, some just require system bootup with one easily accessible header connector. Instructions on pairing bluetooth modules can be found for example here:
http://phillipecantin.blogspot.fi/2014/08/hc-05-bluetooth-link-with-zero-code.html

Credit to
  Thomas O Fredericks (tof@t-o-f.info)
  and previous contributions by Eric Lowry, Jim Schimpf and Tom Harkaway
for their work on the bounce library.

Initial (untested) commits on August 4th, 2016.

Initial testing and improvements by Juha Koljonen on August 4th, 2016.

Full-up end-to-end testing performed on August 5th, 2016, and at this stage the system works. Battery notification is also being relayed to a led on the base system.

Measured delay / lag is in between 10-15ms.
