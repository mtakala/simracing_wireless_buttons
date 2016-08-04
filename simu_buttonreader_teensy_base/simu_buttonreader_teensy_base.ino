/*
0 serial
1 serial
2 bluetooth status
*/


void setup() {
  pinMode(2, INPUT);
  Joystick.X(512);            // "value" is from 0 to 1023
  Joystick.Y(512);            //   512 is resting position
  Joystick.Z(512);
  Joystick.Zrotate(512);
  Joystick.sliderLeft(512);
  Joystick.sliderRight(512);
  for(int i = 1; i<33; i++) {
    Joystick.button(i, 0);
  }
  while(digitalRead(2) == 0);
  // bluetooth päällä
  Serial1.begin(57600);
  Serial1.flush();

}

void loop() {
  // put your main code here, to run repeatedly:
  if(Serial1.available()) {
    char buttons = Serial1.read();

    // check high bit
    if(!bitRead(buttons, 7)) {
      //do stuff for buttons 0-6 (joystick 1-7)
      if(bitRead(buttons, 0)) Joystick.button(1,1);
      if(bitRead(buttons, 1)) Joystick.button(2,1);
      if(bitRead(buttons, 2)) Joystick.button(3,1);
      if(bitRead(buttons, 3)) Joystick.button(4,1);
      if(bitRead(buttons, 4)) Joystick.button(5,1);
      if(bitRead(buttons, 5)) Joystick.button(6,1);
      if(bitRead(buttons, 6)) Joystick.button(7,1);
      
    } else {
      // do stuff for buttons 7-x (joystick 8-10)
      if(bitRead(buttons, 0)) Joystick.button(8,1);
      if(bitRead(buttons, 1)) Joystick.button(9,1);
      if(bitRead(buttons, 2)) Joystick.button(10,1);
    }
  }
}
