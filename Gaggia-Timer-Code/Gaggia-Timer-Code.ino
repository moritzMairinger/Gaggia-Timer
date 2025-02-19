#include "SevSeg.h"

SevSeg sevseg; //Initiate a seven segment controller object
// Rotary Encoder Inputs
#define CLK 34
#define DT 35

int button = 4;
int relais = 2;

int counter = 2500;
int run = false;
int stepSize = 10;

int currentStateCLK;
int lastStateCLK;

void IRAM_ATTR buttonPress(){
  if(!run){
    run = true;
  }
  else{

  }
}

void setup() {
	Serial.begin(9600);

  byte numDigits = 4;
  byte digitPins[] = {33, 26, 25, 22};
  byte segmentPins[] = {32, 23, 19, 17, 16, 27, 21, 18};
  bool resistorsOnSegments = 0;     // variable indicates that 4 resistors were placed on the digit pins.

  sevseg.begin(COMMON_CATHODE, numDigits, digitPins, segmentPins, resistorsOnSegments);
  sevseg.setBrightness(90);

  //Rotary Encoder inputs
  pinMode(CLK,INPUT);
	pinMode(DT,INPUT);

  // Read the initial state of CLK
	lastStateCLK = digitalRead(CLK);

  //Button input and interrupt
  pinMode(button, INPUT_PULLUP);
  attachInterrupt(button, buttonPress, FALLING);

  //Reley
  pinMode(relais, OUTPUT);
}

void loop() {
  currentStateCLK = digitalRead(CLK);

  //If the interrupt activates the timer
  if(run == true){
    countdown();
  }
 
	// If last and current state of CLK are different, then pulse occurred
	// React to only 1 state change to avoid double count
	if (currentStateCLK != lastStateCLK  && currentStateCLK == 1){
		// If the DT state is different than the CLK state then
		// the encoder is rotating CCW so decrement
		if (digitalRead(DT) != currentStateCLK) {
      if(counter > 0){
        counter -= stepSize;
      }
		} else {
			// Encoder is rotating CW so increment
			if(counter < 5000){
        counter += stepSize;
      }
		}
	}

	// Remember last CLK state
	lastStateCLK = currentStateCLK;

  //Set the display to the Counters value
  sevseg.setNumber(counter, 2);
  sevseg.refreshDisplay(); // Must run repeatedly

  //debounce the rotary encoder
  delay(1);
}

void countdown(){
  digitalWrite(relais, HIGH);
  for(int i = 0; i <= counter ; i ++){
    sevseg.setNumber(counter - i, 2);
    
    for(int i2 = 0; i2 < 10; i2++){
      sevseg.refreshDisplay();
      delay(1);
    }
   }

   digitalWrite(relais, LOW);
   //wait shortly to switch the relay off to avoid a problem with the relay emulating a button press
   delay(10);
   run = false;
}