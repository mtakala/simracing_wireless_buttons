# simracing_wireless_buttons
Implementation of wireless buttons for a simracing wheel using two HC-05 bluetooth modules and two Teensy microcontrollers.

BOM: Buttons, paddle shifters, wires, soldering iron and solder. Something to attach the buttons to. Battery connector. 
Two leds with suitable series resistors and two resistors to get battery voltage monitoring voltage divider circuit working. 
One 1S Lithium-Polymer battery is required to power up the system at the wheel.

Requires two Teensy microcontrollers connected with two HC-05 (or compatible) bluetooth modules, with Serial connection 
at Teensy pins 0 and 1. Connection status is connected to pin 2. Please adjust the two HC-05 bluetooth modules to pair with 
each other beforehand.

Credit to
  Thomas O Fredericks (tof@t-o-f.info)
  and previous contributions by Eric Lowry, Jim Schimpf and Tom Harkaway
for their work on the bounce library.

Initial (untested) commits on August 4th, 2016. For battery voltage monitoring, resistor values are needed.
