/* 
 *  Code to put on the Wheel (connected with Bluetooth to a another system)
   for wireless bluetooth button/shifter system using
   two Teensy 3.2 microcontrollers and 2 HC-05 bluetooth modules.

   Authors: Mika Takala & Juha Koljonen 2016

   PINOUT
   -------------------------------------------------------------------------
   0 serial rx
   1 serial tx
   2 bluetooth status
   3-12 button inputs
   13  Teensy's own led
   14-20 unused
   21/A7 Battery_low_led
   22 Connection_led
   23 Battery voltage adc
   24-33 unused

   On Teensy 3.1 & 3.2 the following pins are PWM capable:
   3, 4, 5, 6, 9, 10, 20, 21, 22, 23, 25, 3
   Absolute maximum current is specified as 20 mA on the datasheet.
   Authors of this code recommend at least a 82 ohm series resistor
   for the LED-driving pins when using a typical small 3mm led. 

   The LED:s are PWM driven, and their brightness percentage is set 
   via LED_BRIGHTNESS definition. Range is from 0 - 100 %.
   -------------------------------------------------------------------------

   NOTES ABOUT POWER SAVING MEASURES:
   -------------------------------------------------------------------------
   (1) 
   As the system is intended to be battery powered, the recommendation 
   is to program Teensy as USB TYPE = NO USB and use for example 24MHz 
   or 16 MHz clock. These are set from Tools menu in Arduino/Teensyduino 
   IDE. Setting NO USB will require you to press the manual program button 
   on Teensy to upload the code, but it will save a lot of power.

   (2)
   Additionally you can set all unused inputs as outputs, which
   makes them not floating and they will consume less power that way.
   This has been implemented with a macro, and it has NOT been enabled
   as default.
   PLEASE BE SURE TO NOT HAVE ANYTHING CONNECTED TO THE UNUSED PINS
   LISTED IN InitialiseIO() FUNCTION. THE AUTHOR(S) TAKE NOT RESPONSIBILITY
   IF YOU MANAGE TO FRY/SHORT OUT A PIN OR YOUR TEENSY MICROCONTROLLER.
   -------------------------------------------------------------------------

   NOTE ABOUT BATTERY VOLTAGE ADC MEASUREMENT
   -------------------------------------------------------------------------
   As typical resistors have 5% or even 10% tolerance, you should
   use a multimeter to measure when the battery voltage starts to indicate
   low voltage by blinkin the led, and then adjust
   BATTERY_LOW_VOLTAGE_THRESHOLD accordingly.

   If you are running on 1-cell LiPO battery, you shouldn't discharge
   it more than 80% of its capacity, leaving 20% capacity in the battery.
   This equals to about 3.75 volts. This is recommended, because deeper 
   discharges will start to reduce overall battery life.

   One way to do your ADC/voltage divider calibration is to set
   the variable to quite high value, for example 3800. Start with a known
   full battery, and see when the low battery voltage measurement triggers.
   Then measure the battery voltage using a multimeter. 
   Then you can use maths like this:
   3600     =   YOUR_MEASURED_VOLTAGE
   X        =   3.75

   X = 3600 * 3.75 / YOUR_MEASURED_VOLTAGE

   This will result in a value for BATTERY_LOW_VOLTAGE_THRESHOLD that should
   work, as ADC is linear. Round up to the next even hundred to be
   extra safe.
   -------------------------------------------------------------------------
*/

// used pin definitions, other than buttons
#define btStatus 2
#define teensyLed 13
#define batteryLed 21 
#define connectionLed 22
#define batteryADC 23 


// define led brighness here in percentage
#define LED_BRIGHTNESS 5
// for 82 ohm series resistor and small 3mm red/green through-hole
// led, 5 % seems fine.
#define ledOutputValue 255*LED_BRIGHTNESS/100


// Define if you want to blink Teensy's own led or not.
// Defined value = yes, commented out = no
#define BLINK_TEENSY_LED


// Adjust to fit your voltage divider and battery.
// This value is good starting point for 33k / 11.85k 
// resistors when using internal 1.2V voltage reference on Teensy 3.2.
#define BATTERY_LOW_VOLTAGE_THRESHOLD 3600


// Uncomment this to save a bit of battery power.
// DANGER: Please make double sure that all your unconnected pins
// are correctly set in InitialiseIO() -function.
// Errors here will permanently damage your Teensy microcontroller.

// #define POWERSAVING_WITH_PINS_AS_OUTPUTS


// Included the Bounce2 library found here :
// https://github.com/thomasfredericks/Bounce2
// The library has been modified to use digitalReadFast().
// Fast reactions are needed in the buttons. Therefore in the library,
// this is defined:
//  By defining "#define BOUNCE_LOCK_OUT" in "Bounce.h" 
//  you can activate the alternative debouncing method. This method is 
//  a lot more responsive, but does not cancel noise.
#include "Bounce2.h"


// Bouncer instances connected to buttons.
// If you have more or less buttons,
// adjust this to your liking.
Bounce debouncer3 = Bounce(); 
Bounce debouncer4 = Bounce(); 
Bounce debouncer5 = Bounce(); 
Bounce debouncer6 = Bounce(); 
Bounce debouncer7 = Bounce(); 
Bounce debouncer8 = Bounce(); 
Bounce debouncer9 = Bounce(); 
Bounce debouncer10 = Bounce(); 
Bounce debouncer11 = Bounce(); 
Bounce debouncer12 = Bounce(); 

// Two bytes to store button status
volatile int buttons_0_6 = 0b00000000;
volatile int buttons_7_13 = 0b10000000; // MSB indicates which is it.
volatile short int ledStatus = 0;

// periodic status intervals in milliseconds
long const int StatusInterval = 1500; 
long const int BatteryInterval = 1000; 
long const int WaitIndicationInterval = 200;

unsigned long int lastStatusUpdate = 0;
unsigned long int lastBatteryUpdate = 0;
unsigned long int lastWaitIndicationUpdate = 0;


boolean batteryLow = false;
byte batteryLedStatus = 0;

boolean btConnected = false;
byte connectionLedStatus = 0;
 
void setup()
{
  InitialiseIO();
  
  // Show that we are booting up

  digitalWriteFast(connectionLed, HIGH);
  digitalWriteFast(batteryLed, HIGH);
  delay(1000);
  digitalWriteFast(connectionLed, LOW);
  digitalWriteFast(batteryLed, LOW);
  
  // Define/set debounce intervalls here to suit your buttons
  // Remember to adjust the amount of these if you have
  // more or less buttons.
  debouncer3.attach(3);  debouncer3.interval(5);
  debouncer4.attach(4);  debouncer4.interval(5);
  debouncer5.attach(5);  debouncer5.interval(5);
  debouncer6.attach(6);  debouncer6.interval(5);
  debouncer7.attach(7);  debouncer7.interval(5);
  debouncer8.attach(8);  debouncer8.interval(5);
  debouncer9.attach(9);  debouncer9.interval(5);
  debouncer10.attach(10);  debouncer10.interval(5);
  debouncer11.attach(11);  debouncer11.interval(5);
  debouncer12.attach(12);  debouncer12.interval(5);

  buttons_0_6 = 0b00000000;
  buttons_7_13 = 0b10000000;

  lastStatusUpdate = millis();
  lastBatteryUpdate = millis();
  lastWaitIndicationUpdate = millis();
  
  // Connect to Bt module
  Serial1.begin(57600); // use same baud rate as you have set your BT modules
  Serial1.flush();

  // turn off teensy led
  digitalWriteFast(teensyLed, LOW);
}
  
void loop() {
  // Check Bluetooth status
  if (digitalReadFast(btStatus)) {
    analogWrite(connectionLed, ledOutputValue);
    btConnected = true;
  } else btConnected = false;
  
  // Go through all buttons if connected. Button press is detected on falling edge.
  if (btConnected) {
    updateButtons();
    sanityUpdate();
  } else {
    runWaitBtIndication();
  }
  
  checkBattery();
}

void runWaitBtIndication() {
  if (millis() > (lastWaitIndicationUpdate + WaitIndicationInterval)) {
    lastWaitIndicationUpdate = millis();
    if(connectionLedStatus) {
      analogWrite(connectionLed, ledOutputValue);
    } else{
      analogWrite(connectionLed, 0);
    }
    connectionLedStatus = !connectionLedStatus;
  }
}

void checkBattery() {
  // check battery voltage at defined intervals. 
  if (millis() > (lastBatteryUpdate + BatteryInterval)) {
    lastBatteryUpdate = millis(); 

    // check if battery low:
    if (analogRead(batteryADC) < BATTERY_LOW_VOLTAGE_THRESHOLD) {
      batteryLow = true;
      buttons_7_13 |= 0b01000000; // this bit tells battery status to base unit
      
    } else {
      batteryLow = false;
      buttons_7_13 &= ~(0b01000000); // this bit tells battery status to base unit
    }

    if (batteryLow) {
      if(batteryLedStatus) {
        analogWrite(batteryLed, ledOutputValue);
      } else {
        analogWrite(batteryLed, 0);
      }
      batteryLedStatus = !batteryLedStatus;
    } 
  }
}

void sanityUpdate() {
  // Periodic sanity update
  // sends known button statuses to the base system at defined intervalls.
  if(millis() > (lastStatusUpdate + StatusInterval)) { 
      lastStatusUpdate = millis();
      Serial1.write(buttons_0_6);
      Serial1.write(buttons_7_13);

    #ifdef BLINK_TEENSY_LED
    // 13 = red led on teensy 3.2. Might as well flash it.
    // as aliveness status.
    // Pin 13 is not PWM capable, so just regular digitalWriteFasts here.
    if(ledStatus) {
      ledStatus = 0;
      digitalWriteFast(teensyLed, LOW);
    } else {
      ledStatus = 1;
      digitalWriteFast(teensyLed, HIGH);
    }
    #endif
  }
}


void updateButtons() {
  // updates each button status and sees if it changed.
  // add more or less code according to suit your buttons.
  // 
  debouncer3.update();
  if(debouncer3.fell()) {
    buttons_0_6 |= 0b00000001;
    Serial1.write(buttons_0_6);
  } else if(debouncer3.rose()) {
    buttons_0_6 &= ~(0b00000001);
    Serial1.write(buttons_0_6);
  }
  
  debouncer4.update();
  if(debouncer4.fell()) {
    buttons_0_6 |= 0b00000010;
    Serial1.write(buttons_0_6);
  } else if(debouncer4.rose()) {
    buttons_0_6 &= ~(0b00000010);
    Serial1.write(buttons_0_6);
  }
  
  debouncer5.update();
  if(debouncer5.fell()) {
    buttons_0_6 |= 0b00000100;
    Serial1.write(buttons_0_6);
  } else if(debouncer5.rose()) {
    buttons_0_6 &= ~(0b00000100);
    Serial1.write(buttons_0_6);
  }

  debouncer6.update();
  if(debouncer6.fell()) {
    buttons_0_6 |= 0b00001000;
    Serial1.write(buttons_0_6);
  } else if(debouncer6.rose()) {
    buttons_0_6 &= ~(0b00001000);
    Serial1.write(buttons_0_6);
  }

  debouncer7.update();
  if(debouncer7.fell()) {
    buttons_0_6 |= 0b00010000;
    Serial1.write(buttons_0_6);
  } else if(debouncer7.rose()) {
    buttons_0_6 &= ~(0b00010000);
    Serial1.write(buttons_0_6);
  }

  debouncer8.update();
  if(debouncer8.fell()) {
    buttons_0_6 |= 0b00100000;
    Serial1.write(buttons_0_6);
  } else if(debouncer8.rose()) {
    buttons_0_6 &= ~(0b00100000);
    Serial1.write(buttons_0_6);
  }

  debouncer9.update();
  if(debouncer9.fell()) {
    buttons_0_6 |= 0b01000000;
    Serial1.write(buttons_0_6);
  } else if(debouncer9.rose()) {
    buttons_0_6 &= ~(0b01000000);
    Serial1.write(buttons_0_6);
  }

  debouncer10.update();
  if(debouncer10.fell()) {
    buttons_7_13 |= 0b00000001;
    Serial1.write(buttons_7_13);
  } else if(debouncer10.rose()) {
    buttons_7_13 &= ~(0b00000001);
    Serial1.write(buttons_7_13);
  }

  debouncer11.update();
  if(debouncer11.fell()) {
    buttons_7_13 |= 0b00000010;
    Serial1.write(buttons_7_13);
  } else if(debouncer11.rose()) {
    buttons_7_13 &= ~(0b00000010);
    Serial1.write(buttons_7_13);
  }

  debouncer12.update();
  if(debouncer12.fell()) {
    buttons_7_13 |= 0b00000100;
    Serial1.write(buttons_7_13);
  } else if(debouncer12.rose()) {
    buttons_7_13 &= ~(0b00000100);
    Serial1.write(buttons_7_13);
  }
}



void InitialiseIO(){
  pinMode(btStatus, INPUT);
  pinMode(3, INPUT_PULLUP);
  pinMode(4, INPUT_PULLUP);
  pinMode(5, INPUT_PULLUP);
  pinMode(6, INPUT_PULLUP);
  pinMode(7, INPUT_PULLUP);
  pinMode(8, INPUT_PULLUP);
  pinMode(9, INPUT_PULLUP);
  pinMode(10, INPUT_PULLUP);
  pinMode(11, INPUT_PULLUP);
  pinMode(12, INPUT_PULLUP);
  pinMode(teensyLed, OUTPUT);
  pinMode(batteryLed, OUTPUT);
  pinMode(connectionLed, OUTPUT);
  pinMode(batteryADC, INPUT);
  digitalWriteFast(teensyLed, LOW);
  digitalWriteFast(connectionLed, LOW);
  digitalWriteFast(batteryLed, LOW); // By default the battery is not empty at startup.

  analogReadRes(12);
  analogReadAveraging(5);
  analogReference(INTERNAL);

  // By default, all pins are inputs.
  // Unused inputs in an electrically noisy environment will consume many 
  // mA of power. This is because their voltage floats to a value which makes the input state
  // switch, and all switching of digital circuitry consumes power.
  // Please be double sure that you have not connected anything to these pins,
  // as errors here might fry you Teensy.

  #ifdef POWERSAVING_WITH_PINS_AS_OUTPUTS
  // list your unused digital pins here
  pinMode(14, OUTPUT);
  pinMode(15, OUTPUT);
  pinMode(16, OUTPUT);
  pinMode(17, OUTPUT);
  pinMode(18, OUTPUT);
  pinMode(19, OUTPUT);
  pinMode(20, OUTPUT);
  pinMode(24, OUTPUT);
  pinMode(25, OUTPUT);
  pinMode(26, OUTPUT);
  pinMode(27, OUTPUT);
  pinMode(28, OUTPUT);
  pinMode(29, OUTPUT);
  pinMode(30, OUTPUT);
  pinMode(31, OUTPUT);
  pinMode(32, OUTPUT);
  pinMode(33, OUTPUT);
  #endif
}

