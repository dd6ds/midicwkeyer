// CW-Keyer für Digispark ATtiny85 mit ECHTEM MIDI!
// Pins: P0=paddle_dit, P2=paddle_dah
// Autor: Angepasst für USB MIDI

#include <DigiMIDI.h>  // ✅ RICHTIGE Bibliothek für MIDI!

#define paddleDit  0    // P0
#define paddleDah  2    // P2  
#define LED        1    // P1 = onboard LED

// MIDI Noten (Note 1 = Dit, Note 2 = Dah)
#define MIDI_NOTE_DIT 1
#define MIDI_NOTE_DAH 2
#define MIDI_CHANNEL 1
#define MIDI_VELOCITY 127

// Variablen
bool paddleDitState = false;
bool paddleDahState = false;
bool lastDitState = false;
bool lastDahState = false;

void setup() {
  pinMode(paddleDit, INPUT_PULLUP);
  pinMode(paddleDah, INPUT_PULLUP); 
  pinMode(LED, OUTPUT);
  
  // LED blinkt 3x zur Bestätigung
  for (int i = 0; i < 3; i++) {
    digitalWrite(LED, HIGH);
    DigiMIDI.delay(100);
    digitalWrite(LED, LOW);
    DigiMIDI.delay(100);
  }
}

void loop() {
  // ✅ WICHTIG: MIDI update() regelmäßig aufrufen! (NICHT refresh()!)
  DigiMIDI.update();
  
  // Paddle abfragen
  paddleDitState = !digitalRead(paddleDit);
  paddleDahState = !digitalRead(paddleDah);
  
  // Dit Paddle gedrückt?
  if (paddleDitState && !lastDitState) {
    // MIDI Note On senden
    DigiMIDI.sendNoteOn(MIDI_NOTE_DIT, MIDI_VELOCITY, MIDI_CHANNEL);
    digitalWrite(LED, HIGH);
  }
  else if (!paddleDitState && lastDitState) {
    // MIDI Note Off senden
    DigiMIDI.sendNoteOff(MIDI_NOTE_DIT, MIDI_VELOCITY, MIDI_CHANNEL);
    digitalWrite(LED, LOW);
  }
  
  // Dah Paddle gedrückt?
  if (paddleDahState && !lastDahState) {
    DigiMIDI.sendNoteOn(MIDI_NOTE_DAH, MIDI_VELOCITY, MIDI_CHANNEL);
    digitalWrite(LED, HIGH);
  }
  else if (!paddleDahState && lastDahState) {
    DigiMIDI.sendNoteOff(MIDI_NOTE_DAH, MIDI_VELOCITY, MIDI_CHANNEL);
    digitalWrite(LED, LOW);
  }
  
  lastDitState = paddleDitState;
  lastDahState = paddleDahState;
  
  // ✅ WICHTIG: DigiMIDI.delay() verwenden für V-USB!
  DigiMIDI.delay(1);  // 1ms ist OK
}

