/*
/*
Copyright 2014 Jason Campbell, dochin@campound.org
Licensed under GPL v2 or later
Date: 7/11/14

Purpose: TLC5973 LED Driver Test

Status: works
*/

byte ledPin=8;

int pTime = 1; // in useconds
int tCycle = pTime * 6; // time for one bit. Per datasheet tCycle is between 0.33 useconds and 10 useconds
int numLeds = 5; //number of LEDs

void setup() {
  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, LOW);

  pinMode(A1, OUTPUT);
  pinMode(A2, OUTPUT);
  pinMode(A3, OUTPUT);
  
  digitalWrite(A1, LOW);
  digitalWrite(A2, LOW);
  digitalWrite(A3, HIGH);
}

void loop() {
  
  for(byte i=0; i<255; i++) {
    writeLED(1,0,i,0);
    delay(15);
  }
  for(byte i=0; i<255; i++) {
    writeLED(1,0,0,i);
    delay(5);
  }
  for(byte i=255; i>0; i--) {
    writeLED(1,1,0,0);
    delay(10);
  }
}

void writeLED(byte ledNum, byte redValue, byte greenValue, byte blueValue) {
  for (int i = 0; i< numLeds; i++) {
    writeCommTimer();
    
    if (i = ledNum){
      writeCommand();
      writeData(redValue);
      writeData(greenValue);
      writeData(blueValue);
    }
    waitEOS();
  }
  waitGSLAT();
      
}

void writeZero() {
  PORTB |= B1;
  delayMicroseconds(pTime);
  PORTB &= B111110;
  delayMicroseconds(pTime);
  delayMicroseconds(pTime);
  delayMicroseconds(pTime);
  delayMicroseconds(pTime);
  delayMicroseconds(pTime);
  
}

void writeOne() {
  PORTB |= B1;
  delayMicroseconds(pTime);
  PORTB &= B111110;
  delayMicroseconds(pTime); 
  PORTB |= B1; //rising edge of second pulse has to be within 0.5 * tCycle
  delayMicroseconds(pTime);
  PORTB &= B111110;
  delayMicroseconds(pTime);
  delayMicroseconds(pTime);
  delayMicroseconds(pTime);
   
}

void writeCommTimer() {
  //first two zeroes determine the timing (tcycle) after device is powered up or after a GSLAT
  writeZero();
  writeZero();
}

void writeCommand() {

  //write command in hex is 3AA, binary is 11 1010 1010
  writeOne();
  writeOne();
  
  writeOne();
  writeZero();
  writeOne();
  writeZero();

  writeOne();
  writeZero();
  writeOne();
  writeZero();  
}

//end of sequence (for a single driver chip)
void waitEOS() {
  PORTB &= B111110;
  delayMicroseconds(tCycle*4); // min 3.5 to max 5.5 times tCycle
}

//grayscale latch (for the end of a chain of driver chips)
void waitGSLAT() {
  PORTB &= B111110;
  delayMicroseconds(tCycle*10); // minimum 8 time tCycle
}
  
void writeData(byte data) {
  for (byte i = 0; i<8; i++) {
    if(data & B10000000) {writeOne();}
    else{writeZero();}
    data <<= 1;
  }
  //pad the end of each 8 bit number with four more 
  //bits (use 1010 because that's the end of the
  //write command so it doesn't have to have an entirely
  //separate function) to make a 12 bit number
  writeZero();
  writeZero();
  writeZero();
  writeZero();
}
