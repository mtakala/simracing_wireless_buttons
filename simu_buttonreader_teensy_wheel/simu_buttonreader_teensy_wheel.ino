/* Base (connected with USB to a PC)
   for wireless bluetooth button/shifter system using
   two Teensy 3.2 microcontrollers and 2 HC-05 bluetooth modules.

   Pinout for this unit on the wheel itself:   
   0 serial
   1 serial
   2 bluetooth status
    
   13  Teensy's own led
   14 
   15
   16
   17
   18
   19
   20
   21/A7 Battery_low_led
   22 Connection_led
   23 Battery voltage adc
*/

#define btStatus 2
#define teensyLed 13
#define batteryLed 21 
#define connectionLed 22
#define batteryADC 23 


// Included the Bounce2 library found here :
// https://github.com/thomasfredericks/Bounce2

// The library has been modified to use digitalReadFast()

// Fast reactions are needed in the buttons. Therefore in the library,
// this is defined:
// By defining "#define BOUNCE_LOCK_OUT" in "Bounce.h" 
// you can activate the alternative debouncing method. This method is 
// a lot more responsive, but does not cancel noise.
#include "Bounce2.h"


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
volatile int buttons_7_13 = 0b10000000;
volatile short int ledStatus = 0;

// periodic status intervals in milliseconds
long const int StatusInterval = 1500; 
long const int BatteryInterval = 1000; 
long const int WaitIndicationInterval = 200;

long int lastStatusUpdate = 0;
long int lastBatteryUpdate = 0;
long int lastWaitIndicationUpdate = 0;

// Adjust to fit your voltage divider and battery.
// This value is good for 33k / 11.85k resistors when using
// internal 1.2V voltage reference on Teensy 3.2.
const int batterylowVoltageThreshold = 3730;

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
}
  
void loop() {
  // Check Bluetooth status
  if (digitalReadFast(btStatus)) {
    digitalWriteFast(connectionLed, HIGH);
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
    digitalWriteFast(connectionLed, connectionLedStatus);
    connectionLedStatus = !connectionLedStatus;
  }
}

void checkBattery() {
  // check battery voltage at defined intervals. 
  if (millis() > (lastBatteryUpdate + BatteryInterval)) {
    lastBatteryUpdate = millis(); 

    // check if battery low:
    if (analogRead(batteryADC) < batterylowVoltageThreshold) {
      batteryLow = true;
      buttons_7_13 |= 0b01000000; // this bit tells battery status to base unit
      
    } else {
      batteryLow = false;
      buttons_7_13 &= ~(0b01000000); // this bit tells battery status to base unit
    }

    if (batteryLow) {
      digitalWriteFast(batteryLed, batteryLedStatus);
      batteryLedStatus = !batteryLedStatus;
    } else digitalWriteFast(batteryLed, LOW);
  }
}

void sanityUpdate() {
  // Periodic sanity update
  // sends known button statuses to the base system at defined intervalls.
  if(millis() > (lastStatusUpdate + StatusInterval)) { 
      lastStatusUpdate = millis();
      Serial1.write(buttons_0_6);
      Serial1.write(buttons_7_13);

    // 13 = red led on teensy 3.2. Might as well flash it.
    // as aliveness status.
    if(ledStatus) {
      ledStatus = 0;
      digitalWriteFast(teensyLed, LOW);
    } else {
      ledStatus = 1;
      digitalWriteFast(teensyLed, HIGH);
    }
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
}

void updateButtons() {
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

