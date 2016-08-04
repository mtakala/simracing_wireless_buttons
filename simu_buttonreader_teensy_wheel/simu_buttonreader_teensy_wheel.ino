// teensy 3.2 sim wheel button reader
// usb = Serial
// rx0/tx1 = Serial1

/*
0 serial
1 serial
2 bluetooth status
3 Button0
4 Button1
5 Button2
6 Button3
7 Button4
8 Button5
9 Button6
10  Button7
11  Button8
12  Button9
13  Teensyn oma ledi, vilkuttaa periodic lähetyksen tahtiin
14 
15
16
17
18
19
20
21/A7 Battery voltage adc
22 Connection_led
23 Battery_low_led
*/


// Include the Bounce2 library found here :
// https://github.com/thomasfredericks/Bounce2

// kirjasto modattu käyttämään digitalReadFast()

// Ratin napeissa (etenkin vaihtonapeissa) tärkeää
// responsiivisuus. Siksi tehdään tää:
// define this in the bounce2.h:
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

// nappuloiden variablet
volatile int buttons_0_6 = 0b00000000;
volatile int buttons_7_13 = 0b10000000;
volatile short int ledStatus = 0;

// periodic status intervallit millisekunteina
long const int StatusInterval = 2000; // pari sekuntia
long const int BatteryInterval = 60000; // minuutti

long int lastStatusUpdate;
long int lastBatteryUpdate;

const int batterylowVoltageThreshold = 700;
 
void setup()
{
  InitialiseIO();
  // intervallit säädettävissä, paddleille pienempi ja
  // nappuloille vähän isompi. Vaatii hands-on testausta.
  
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
  // serial1 on rx/tx pinnit
  Serial1.begin(9600);

  //Serial.begin(9600);
  
  lastStatusUpdate=millis();
  lastBatteryUpdate=millis();
  
  ledStatus = 0;

  analogReadRes(12);
  analogReadAveraging(5);
  analogReference(INTERNAL);

  
  // setup 1000ms timer
//  noInterrupts(); // disable all interrupts
//  TCCR1A = 0;
//  TCCR1B = 0;
//  TCNT1 = 0;
//  OCR1A = 31250; // compare match register 8MHz/256 = 1Hz
//  TCCR1B |= (1 << WGM12); // CTC mode
//  TCCR1B |= (1 << CS12); // 256 prescaler
//  TIMSK1 |= (1 << OCIE1A); // enable timer compare interrupt
//  interrupts(); // enable all interrupts
}



//ISR(Timer1_COMPA_vect) // timer compare interrupt service routine
//{
//  periodic_status = 1;
//}



void loop() {
  // debug loopin pituuden mittaukseen
  //long loopstart = micros();

  // bluetooth status
  if(digitalReadFast(2)) {
    digitalWriteFast(22, HIGH);
  } else {
    digitalWriteFast(22, LOW);
  }


  
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


  // debug loopin pituuden mittaustulos usb seriaaliin
  //long loopend = micros();
  //Serial.print((String)(loopend-loopstart));
  //Serial.print("\n");




  // periodic sanity
  
  if(lastStatusUpdate + StatusInterval > millis()) { 
      lastStatusUpdate = millis();
      Serial1.write(buttons_0_6);
      Serial1.write(buttons_7_13);
      //Serial.print("Periodic!\n");
      //delay(500);
      //delay(500);
    
    // 13 = punainen ledi teensyssä, saa vilkuttaa
    // aliveness status indikaattori
    if(ledStatus) {
      ledStatus = 0;
      digitalWriteFast(13, LOW);
    
    } else {
      ledStatus = 1;
      digitalWriteFast(13, HIGH);
    }
   }

  if(lastBatteryUpdate + BatteryInterval > millis()) {
    lastBatteryUpdate = millis(); 
    // adc ja ledin nitkutus tänne
    int batterylow=0;

    // check if battery low:

    long batteryVoltage = analogRead(A7);
    if (batteryVoltage < batterylowVoltageThreshold) {
      batterylow=1;
    }
 
    if(batterylow) {
      digitalWriteFast(23, HIGH);
    } else {
      digitalWriteFast(23, LOW);
    }
  }

}


void InitialiseIO(){
  pinMode(2, INPUT);
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
  pinMode(13, OUTPUT);
  pinMode(21, INPUT);
  pinMode(22, OUTPUT);
  pinMode(23, OUTPUT);
  digitalWriteFast(22, LOW);
  digitalWriteFast(23, HIGH); // oletus että patteri ei oo alussa tyhjä
}

