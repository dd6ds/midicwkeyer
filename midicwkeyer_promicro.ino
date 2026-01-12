/*
 * MIDI CW Keyer für DigiSpark ATtiny85
 * 
 * Hardware: DigiSpark ATtiny85 (16.5MHz, V-USB)
 * USB: V-USB MIDI (HID-MIDI - funktioniert auf Linux, braucht Bridge auf Windows!)
 * 
 * Pinbelegung:
 * - Pin 0 (P0): Dit Paddle (INPUT_PULLUP)
 * - Pin 2 (P2): Dah Paddle (INPUT_PULLUP)  
 * - Pin 1 (P1): Status LED (onboard)
 * - Pin 5 (P5): WPM Potentiometer (optional, A0/analog)
 * 
 * Pins 3 & 4 sind für USB reserviert - NICHT verwenden!
 * 
 * MIDI Nachrichten:
 * - Note 1: Dit
 * - Note 2: Dah
 * - Control Change 61: WPM (6-48 WPM)
 * 
 * WICHTIG: 
 * - Funktioniert auf Linux direkt
 * - Auf Windows: Braucht HID-MIDI Bridge (loopMIDI + Bridge-Software)
 * - Oder: Arduino Pro Micro kaufen für native Windows-Unterstützung
 * 
 * Autor: Angepasst für DigiSpark ATtiny85 mit DigiMIDI
 * Datum: 2026-01-12
 */

#include <DigiMIDI.h>

// ============================================================================
// PIN KONFIGURATION (ATtiny85 hat nur P0, P1, P2, P5 verfügbar!)
// ============================================================================
#define PIN_DIT_PADDLE    0       // P0 = Dit Paddle (aktiv LOW mit INPUT_PULLUP)
#define PIN_DAH_PADDLE    2       // P2 = Dah Paddle (aktiv LOW mit INPUT_PULLUP)
#define PIN_LED           1       // P1 = Onboard LED
#define PIN_WPM_POT       5       // P5 = Poti für WPM (optional, analog A0)
// P3 & P4 = USB (nicht verwenden!)

// ============================================================================
// MIDI KONFIGURATION
// ============================================================================
#define MIDI_NOTE_DIT     1       // MIDI Note für Dit
#define MIDI_NOTE_DAH     2       // MIDI Note für Dah
#define MIDI_CHANNEL      0       // MIDI Kanal 1 (0-based = Kanal 1)
#define MIDI_VELOCITY     127     // Velocity (Lautstärke)
#define MIDI_CC_WPM       61      // Control Change für WPM

// ============================================================================
// TIMING KONFIGURATION
// ============================================================================
#define DEFAULT_WPM       20      // Standard WPM
#define MIN_WPM           6       // Minimum WPM
#define MAX_WPM           48      // Maximum WPM
#define DEBOUNCE_MS       5       // Debounce Zeit

// ============================================================================
// GLOBALE VARIABLEN
// ============================================================================
// Paddle Zustände
bool ditState = false;
bool dahState = false;
bool lastDitState = false;
bool lastDahState = false;

// Timing für Debouncing
unsigned long lastDitDebounce = 0;
unsigned long lastDahDebounce = 0;

// WPM (optional)
int currentWPM = DEFAULT_WPM;
int lastWPM = DEFAULT_WPM;
unsigned long lastWPMUpdate = 0;

// ============================================================================
// SETUP
// ============================================================================
void setup() {
  // Pin-Konfiguration
  pinMode(PIN_DIT_PADDLE, INPUT_PULLUP);
  pinMode(PIN_DAH_PADDLE, INPUT_PULLUP);
  pinMode(PIN_LED, OUTPUT);
  
  // LED Startup-Sequenz (3x blinken zur Bestätigung)
  for (int i = 0; i < 3; i++) {
    digitalWrite(PIN_LED, HIGH);
    DigiMIDI.delay(100);  // ⚠️ DigiMIDI.delay(), nicht delay()!
    digitalWrite(PIN_LED, LOW);
    DigiMIDI.delay(100);
  }
  
  // Initiales WPM senden
  sendWPM(currentWPM);
  
  // LED aus
  digitalWrite(PIN_LED, LOW);
}

// ============================================================================
// MAIN LOOP
// ============================================================================
void loop() {
  // ⚠️ KRITISCH: DigiMIDI.refresh() muss regelmäßig aufgerufen werden!
  // Dies ist essentiell für V-USB Kommunikation!
  DigiMIDI.refresh();
  
  // WPM von Potentiometer lesen (optional, wenn Pin 5 angeschlossen)
  readWPM();
  
  // Paddle Status lesen mit Debouncing
  readPaddles();
  
  // Dit Paddle - Flanken erkennen (gedrückt/losgelassen)
  if (ditState && !lastDitState) {
    // Dit Paddle gedrückt → MIDI Note On
    sendNoteOn(MIDI_NOTE_DIT);
    digitalWrite(PIN_LED, HIGH);
  }
  else if (!ditState && lastDitState) {
    // Dit Paddle losgelassen → MIDI Note Off
    sendNoteOff(MIDI_NOTE_DIT);
    digitalWrite(PIN_LED, LOW);
  }
  
  // Dah Paddle - Flanken erkennen
  if (dahState && !lastDahState) {
    // Dah Paddle gedrückt → MIDI Note On
    sendNoteOn(MIDI_NOTE_DAH);
    digitalWrite(PIN_LED, HIGH);
  }
  else if (!dahState && lastDahState) {
    // Dah Paddle losgelassen → MIDI Note Off
    sendNoteOff(MIDI_NOTE_DAH);
    digitalWrite(PIN_LED, LOW);
  }
  
  // Status für nächste Iteration speichern
  lastDitState = ditState;
  lastDahState = dahState;
  
  // ⚠️ WICHTIG: DigiMIDI.delay() verwenden, nicht delay()!
  // DigiMIDI.delay() ruft intern refresh() auf
  DigiMIDI.delay(1);
}

// ============================================================================
// PADDLE LESEN MIT DEBOUNCING
// ============================================================================
void readPaddles() {
  unsigned long now = millis();
  
  // Dit Paddle lesen (invertiert wegen INPUT_PULLUP)
  bool ditReading = !digitalRead(PIN_DIT_PADDLE);
  if (ditReading != ditState) {
    if (now - lastDitDebounce > DEBOUNCE_MS) {
      ditState = ditReading;
      lastDitDebounce = now;
    }
  }
  
  // Dah Paddle lesen (invertiert wegen INPUT_PULLUP)
  bool dahReading = !digitalRead(PIN_DAH_PADDLE);
  if (dahReading != dahState) {
    if (now - lastDahDebounce > DEBOUNCE_MS) {
      dahState = dahReading;
      lastDahDebounce = now;
    }
  }
}

// ============================================================================
// WPM VON POTENTIOMETER LESEN (OPTIONAL)
// Nur wenn Poti an Pin 5 (A0) angeschlossen ist
// ============================================================================
void readWPM() {
  // Nur alle 200ms lesen um Performance zu schonen
  unsigned long now = millis();
  if (now - lastWPMUpdate < 200) {
    return;
  }
  lastWPMUpdate = now;
  
  // Analog lesen (0-1023) von Pin 5 (= A0)
  int potValue = analogRead(0);  // A0 = Pin 5
  
  // Auf WPM Range mappen
  currentWPM = map(potValue, 0, 1023, MIN_WPM, MAX_WPM);
  
  // Nur senden wenn WPM sich geändert hat
  if (currentWPM != lastWPM) {
    sendWPM(currentWPM);
    lastWPM = currentWPM;
  }
}

// ============================================================================
// MIDI NOTE ON SENDEN
// ============================================================================
void sendNoteOn(byte note) {
  DigiMIDI.sendNoteOn(note, MIDI_VELOCITY, MIDI_CHANNEL);
}

// ============================================================================
// MIDI NOTE OFF SENDEN
// ============================================================================
void sendNoteOff(byte note) {
  DigiMIDI.sendNoteOff(note, MIDI_CHANNEL);
}

// ============================================================================
// WPM ALS CONTROL CHANGE SENDEN
// ============================================================================
void sendWPM(int wpm) {
  // WPM auf MIDI Wertebereich begrenzen (0-127)
  byte midiValue = constrain(wpm, 0, 127);
  DigiMIDI.sendControlChange(MIDI_CC_WPM, midiValue, MIDI_CHANNEL);
}
