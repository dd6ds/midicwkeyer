// CW-Keyer für Digispark ATtiny85 (16.5MHz)
// Pins: P0=paddle_dit, P2=paddle_dah, P1=Key-Out+LED
// Autor: angepasst für Digispark

#include "DigiKeyboard.h"  // Für USB-Debug (optional)

#define paddleDit  0    // P0
#define paddleDah  2    // P2  
#define keyPin     1    // P1 = Key Out + onboard LED
#define button     4    // P4 = Mode-Toggle

// Timing (WPM)
unsigned long ditTime;      // ms pro Dit
unsigned long dahTime;
unsigned long interElement;
unsigned long interChar;
unsigned long interWord;

// Variablen
byte mode = 0;              // 0=iambicA, 1=iambicB, 2=straight
byte state = 0;             // 0=idle, 1=sending
unsigned long lastTime = 0;
char keyerBuffer[8];
byte bufPos = 0;
bool paddleDitState = false;
bool paddleDahState = false;
bool lastDitState = false;
bool lastDahState = false;

void setup() {
  pinMode(paddleDit, INPUT_PULLUP);
  pinMode(paddleDah, INPUT_PULLUP); 
  pinMode(button, INPUT_PULLUP);
  pinMode(keyPin, OUTPUT);
  
  // 20 WPM Standard
  setWPM(20);
  
  // LED aus
  digitalWrite(keyPin, LOW);
  
  // 1s Startup-Blink
  keyerBlink(3);
}

void loop() {
  // Paddle abfragen
  paddleDitState = !digitalRead(paddleDit);
  paddleDahState = !digitalRead(paddleDah);
  
  // Mode-Toggle Button
  if (!digitalRead(button) && bufPos == 0) {
    mode = (mode + 1) % 3;
    keyerBlink(mode + 1);  // Visual feedback
    delay(200);
  }
  
  // Keyer-State-Machine
  switch(state) {
    case 0: // Idle - Paddle check
      if (paddleDitState && !lastDitState) startDit();
      else if (paddleDahState && !lastDahState) startDah();
      else if (bufPos > 0) sendBuffer();
      break;
      
    case 1: // Sending element
      if (millis() - lastTime > interElement) {
        keyOff();
        state = 2;  // Gap
      }
      break;
      
    case 2: // Inter-element gap
      if (millis() - lastTime > interElement) {
        state = 0;  // Back to idle
      }
      break;
  }
  
  lastDitState = paddleDitState;
  lastDahState = paddleDahState;
}

void startDit() {
  keyOn();
  lastTime = millis();
  state = 1;
  keyerBuffer[bufPos++] = '.';
}

void startDah() {
  keyOn();
  lastTime = millis();
  state = 1;
  keyerBuffer[bufPos++] = '-';
}

void keyOn() {
  digitalWrite(keyPin, HIGH);
}

void keyOff() {
  digitalWrite(keyPin, LOW);
}

void sendBuffer() {
  // Buffer senden (E, T, N, etc. decode optional)
  for (byte i = 0; i < bufPos; i++) {
    // Hier könnte Morse-Decode stehen (PARIS etc.)
    // Für jetzt: direkt als Dit/Dah senden
  }
  bufPos = 0;
}

void setWPM(byte wpm) {
  ditTime = 1200UL / wpm;           // Dit length
  dahTime = ditTime * 3;
  interElement = ditTime;
  interChar = ditTime * 3;
  interWord = ditTime * 7;
}

void keyerBlink(byte count) {
  for (byte i = 0; i < count; i++) {
    digitalWrite(keyPin, HIGH);
    delay(100);
    digitalWrite(keyPin, LOW);
    delay(100);
  }
}

