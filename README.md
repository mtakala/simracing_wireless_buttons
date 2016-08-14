# Simracing Wireless Buttons
Implementation of wireless buttons for a simracing wheel using two HC-05 bluetooth modules and two Teensy microcontrollers.

BOM: Buttons, paddle shifters, wires, soldering iron and solder. Something to attach the buttons to. Battery connector. Two leds with suitable series resistors and two resistors to get battery voltage monitoring voltage divider circuit working. One 1S Lithium-Polymer battery is required to power up the system at the wheel.

Requires two Teensy microcontrollers connected with two HC-05 (or compatible) bluetooth modules, with Serial connection 
at Teensy pins 0 and 1. Connection status is connected to pin 2. 

Please adjust the two HC-05 bluetooth modules to pair with each other beforehand. There are various types of HC-05 modules around. To access AT mode to setup, some require manually soldering a wire at pin 34, some just require system bootup with one easily accessible header connector pulled high before supplying power. Instructions on pairing HC-05 bluetooth modules can be found for example here:
http://phillipecantin.blogspot.fi/2014/08/hc-05-bluetooth-link-with-zero-code.html

Please take note to avoid HC-06 modules which cannot be programmed with AT commands and thus cannot connect to each other.

Credit to
  Thomas O Fredericks (tof@t-o-f.info)
  and previous contributions by Eric Lowry, Jim Schimpf and Tom Harkaway
for their work on the bounce library.

Initial (untested) commits on August 4th, 2016.

Initial testing and improvements by Juha Koljonen on August 4th, 2016.

Measured delay / lag is in between 10-15ms.

Full-up end-to-end testing performed on August 5th, 2016, and at this stage the system works. Battery notification is also being relayed to a led on the base system.

Additional features added since then.




# NOTES ABOUT POWER SAVING MEASURES FOR THE WHEEL:
-------------------------------------------------------------------------
(1) 
As the system is intended to be battery powered, the recommendation is to program Teensy as USB TYPE = NO USB and use for example 24MHz 
or 16 MHz clock. These are set from Tools menu in Arduino/Teensyduino IDE. Setting NO USB will require you to press the manual program button on Teensy to upload the code, but it will save a lot of power.
(2)
Additionally you can set all unused inputs as outputs, which makes them not floating and they will consume less power that way.
This has been implemented with a macro, and it has NOT been enabled as default.
PLEASE BE SURE TO NOT HAVE ANYTHING CONNECTED TO THE UNUSED PINS LISTED IN InitialiseIO() FUNCTION. THE AUTHOR(S) TAKE NOT RESPONSIBILITY IF YOU MANAGE TO FRY/SHORT OUT A PIN OR YOUR TEENSY MICROCONTROLLER.
-------------------------------------------------------------------------
# NOTE ABOUT BATTERY VOLTAGE ADC MEASUREMENT
-------------------------------------------------------------------------
As typical resistors have 5% or even 10% tolerance, you should use a multimeter to measure when the battery voltage starts to indicate
low voltage by blinkin the led, and then adjust BATTERY_LOW_VOLTAGE_THRESHOLD accordingly. If you are running on 1-cell LiPO battery, you shouldn't discharge it more than 80% of its capacity, leaving 20% capacity in the battery. This equals to about 3.75 volts. This is recommended, because deeper discharges will start to reduce overall battery life. One way to do your ADC/voltage divider calibration is to set the variable to quite high value, for example 3600. Start with a known full battery, and see when the low battery voltage measurement triggers. Then measure the battery voltage using a multimeter. 

Then you can use maths like this:
  3600 = YOUR_MEASURED_VOLTAGE
  
  X = 3.75
  
  X = 3600 * 3.75 / YOUR_MEASURED_VOLTAGE
  

This will result in a value for BATTERY_LOW_VOLTAGE_THRESHOLD that should work, as ADC is linear. Round up to the next even hundred to be extra safe.
-------------------------------------------------------------------------
