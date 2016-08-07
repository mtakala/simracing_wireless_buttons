/* Base (connected with USB to a PC)
   for wireless bluetooth button/shifter system using
   two Teensy 3.2 microcontrollers and 2 HC-05 bluetooth modules.
   
   Pinout for this base unit:
	0 serial rx
	1 serial tx
	2 bluetooth status
  11 connection status led
  13 power led
*/

// some status variables
boolean teensyLedStatus = false;
boolean btConnected = false;
boolean lowBattery = false;
byte connectionLedStatus = 0;

#define btStatus 2
#define teensyLed 13
#define connectionLed 11

long const int WaitIndicationInterval = 200;
long int lastWaitIndicationUpdate = 0;


long const int WaitBatteryInterval = 1000;
long int lastBatteryIndicationUpdate = 0;

void setup() {

  // I/O directions
  pinMode(btStatus, INPUT);
  pinMode(teensyLed, OUTPUT);
  pinMode(connectionLed, OUTPUT);

  // wake-up blinking
  digitalWriteFast(connectionLed, HIGH);
  digitalWriteFast(teensyLed, HIGH);
  delay(1000);
  digitalWriteFast(connectionLed, LOW);
  digitalWriteFast(teensyLed, LOW);
  delay(1000);
  digitalWriteFast(teensyLed, HIGH);
  
  // set led status
  teensyLedStatus=true;
  
  // set all joystick things to known positions
  Joystick.useManualSend(true); // enable this to get simultaneous button presses if needed
  Joystick.X(512);            // "value" is from 0 to 1023
  Joystick.Y(512);            //   512 is resting position
  Joystick.Z(512);
  Joystick.Zrotate(512);
  Joystick.sliderLeft(512);
  Joystick.sliderRight(512);
  
  // all buttons off
  for(int i = 1; i<33; i++) {
    Joystick.button(i, 0);
  }
  Joystick.send_now();

  
  // start serial even without bluetooth connection
  Serial1.begin(57600); // use same baud rate as you have set your BT modules
  Serial1.flush();
  Serial1.flush(); // flush twice to make sure
  
  // reset these to known value
  lastWaitIndicationUpdate = millis();
  lastBatteryIndicationUpdate = millis();
}

void loop() {
  // Check Bluetooth status
  if (digitalReadFast(btStatus)) {
    digitalWriteFast(connectionLed, HIGH);
    btConnected = true;
  } else {
    btConnected = false;
  }

  if(btConnected) {
    if(Serial1.available()) {
      char buttons = Serial1.read();

      // check high bit
      if(!bitRead(buttons, 7)) {
        //do stuff for buttons 0-6 (joystick 1-7)
        if(bitRead(buttons, 0)) {Joystick.button(1,1);} else { Joystick.button(1,0);}
        if(bitRead(buttons, 1)) {Joystick.button(2,1);} else { Joystick.button(2,0);}
        if(bitRead(buttons, 2)) {Joystick.button(3,1);} else { Joystick.button(3,0);}
        if(bitRead(buttons, 3)) {Joystick.button(4,1);} else { Joystick.button(4,0);}
        if(bitRead(buttons, 4)) {Joystick.button(5,1);} else { Joystick.button(5,0);}
        if(bitRead(buttons, 5)) {Joystick.button(6,1);} else { Joystick.button(6,0);}
        if(bitRead(buttons, 6)) {Joystick.button(7,1);} else { Joystick.button(7,0);}
	      Joystick.send_now();
      
      } else {
        // do stuff for buttons 7-x (joystick 8-10)
	      // (my own system has 8 buttons + 2 shifter paddles totaling 10)
        if(bitRead(buttons, 0)) {Joystick.button(8,1);} else { Joystick.button(8,0);}
        if(bitRead(buttons, 1)) {Joystick.button(9,1);} else { Joystick.button(9,0);}
        if(bitRead(buttons, 2)) {Joystick.button(10,1);} else { Joystick.button(10,0);}

        // there are some spare bits. This uses one of them for battery status reporting.
        // get battery status
        if(bitRead(buttons, 6)) { // bit 6 && 7 are both ones when battery low
          lowBattery = true;
        } else {
          lowBattery = false;
        }
	      Joystick.send_now();
      }
    }
    // periodic battery status blinking
    runBatteryIndication();
  } else {
    // periodic bluetooth connection status
    runWaitBtIndication();
  }
}

void runWaitBtIndication() {
  if (millis() > (lastWaitIndicationUpdate + WaitIndicationInterval)) {
    lastWaitIndicationUpdate = millis();
    digitalWriteFast(connectionLed, connectionLedStatus);
    connectionLedStatus = !connectionLedStatus;
  }
}

void runBatteryIndication() {
  // blink status led at known interval when battery is low.
  if(lowBattery) {
    if (millis() > (lastBatteryIndicationUpdate + WaitBatteryInterval)) {
      lastBatteryIndicationUpdate = millis();
      digitalWriteFast(teensyLed, teensyLedStatus);
      teensyLedStatus = !teensyLedStatus;
    }
  }
}

