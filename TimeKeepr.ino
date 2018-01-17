#include <Bounce2.h>
#include <Adafruit_GFX.h>
#include "Adafruit_LEDBackpack.h"
#include <SPI.h>
#include <avr/sleep.h>
#include "PinChangeInterrupt.h"
#include "LowPower.h"
#include <EEPROMex.h>

Adafruit_7segment matrix = Adafruit_7segment();

Bounce P1U_db = Bounce(); 
Bounce P1D_db = Bounce(); 
Bounce P2U_db = Bounce(); 
Bounce P2D_db = Bounce(); 
const int P1U = 5;
const int P1D = 6;
const int P2U = 4;
const int P2D = A2;
int shortPress = 1500;
boolean isserver = true;
boolean sleeping = false;
int P1Score, P2Score, P1UState, P1DState, P2UState, P2DState;
boolean P1UChanged,P1DChanged,P2UChanged,P2DChanged;
int min_dim = 60;
int min_off = 90;
int level_dim = 4;
int level_bright = 12;
int BrightnessMemory = 8;
int lastChange;
int memBase = 200;

void setup() {
  matrix.begin(0x70);    
    
  pinMode(P1U,INPUT_PULLUP);  
  P1U_db.attach(P1U);
  P1U_db.interval(50);  
  attachPinChangeInterrupt(digitalPinToPinChangeInterrupt(P1U), wake, CHANGE);

  pinMode(P1D,INPUT_PULLUP);
  P1D_db.attach(P1D);
  P1D_db.interval(50);
  attachPinChangeInterrupt(digitalPinToPinChangeInterrupt(P1D), wake, CHANGE);
  
  pinMode(P2U,INPUT_PULLUP);
  P2U_db.attach(P2U);
  P2U_db.interval(50);
  attachPinChangeInterrupt(digitalPinToPinChangeInterrupt(P2U), wake, CHANGE);

  
  pinMode(P2D,INPUT_PULLUP);
  P2D_db.attach(P2D);
  P2D_db.interval(50);
  attachPinChangeInterrupt(digitalPinToPinChangeInterrupt(P2U), wake, CHANGE);
      
  level_bright = EEPROM.readInt(memBase+BrightnessMemory);
  if (level_bright < 0)
    level_bright = 12;    
  matrix.setBrightness(level_bright);  
  
  lastChange = millis();    
}

void writeErrorCode(int errorcode) {  
  matrix.clear();
  matrix.writeDisplay();
  matrix.print(errorcode,DEC);
  matrix.writeDisplay();
  while(1);
}

void loop() {   
  if (lastChange > 0 && millis() - lastChange > (min_off * 60000)) {
    matrix.clear();
    matrix.writeDisplay();     
    LowPower.powerDown(SLEEP_FOREVER, ADC_OFF, BOD_OFF);
    sleeping = true;   
  } 
  else if (lastChange > 0 && millis() - lastChange > (min_dim * 60000)) {
    matrix.setBrightness(level_dim);  
  } 
  else {
    matrix.setBrightness(level_bright);      
  }
  
  P1UChanged = P1U_db.update();
  P1DChanged = P1D_db.update();
  P2UChanged = P2U_db.update();
  P2DChanged = P2D_db.update();

  if (P1UChanged || P1DChanged || P2UChanged || P2DChanged)
    lastChange = millis();

  int duration = millis()-lastChange;
  int P1UState = P1U_db.read();
  int P1DState = P1D_db.read();
  int P2UState = P2U_db.read();
  int P2DState = P2D_db.read();
/*
  if (P1U_db.rose() && P1UChanged && duration < shortPress) 
    //changeScore('1', 'U');
  else if (P1D_db.rose() && P1DChanged && duration < shortPress && P1Score > 0) 
    //changeScore('1', 'D');
  else if (P2U_db.rose() && P2UChanged && duration < shortPress) 
    //changeScore('2', 'U');
  else if (P2D_db.rose() && P2DChanged && duration < shortPress && P2Score > 0) 
    //changeScore('2', 'D');
  else if (P2DState == LOW && duration > shortPress) 
    //resetScores();
  else if (P1DState == LOW && duration > shortPress) 
    cycleBrightness();
*/

  matrix.clear();

  if (!sleeping)
    matrix.writeDisplay();
}

void cycleBrightness() {
  level_bright++;
  
  if (level_bright > 15)
    level_bright = 1;
    
  matrix.setBrightness(level_bright);
  EEPROM.writeInt(memBase+BrightnessMemory, level_bright);
  
  matrix.clear();
  matrix.println(level_bright);
  matrix.writeDisplay();
  
  delay(500);
  matrix.clear();
  matrix.writeDisplay();
}

void wake() {
  sleeping = false;
  lastChange = millis();
}
