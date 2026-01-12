/*
 * MIDI CW Keyer für Arduino Pro Micro / Leonardo
 * 
 * Hardware: Arduino Pro Micro (ATmega32U4) oder Arduino Leonardo
 * USB: Echtes USB MIDI (Class Compliant - funktioniert auf Windows/Linux/Mac!)
 * 
 * Pinbelegung:
 * - Pin 2: Dit Paddle (INPUT_PULLUP)
 * - Pin 3: Dah Paddle (INPUT_PULLUP)
 * - Pin 4: WPM Potentiometer (optional, A2/analog)
 * - LED_BUILTIN: Status LED
 * 
 * MIDI Nachrichten:
 * - Note 1 (MIDI Note 1): Dit
 * - Note 2 (MIDI Note 2): Dah
 * - Control Change 61: WPM (6-48 WPM)
 * 
 * Autor: Angepasst für Arduino Pro Micro mit USB MIDI
 * Datum: 2026-01-12
 */

#include <MIDIUSB.h>

// ============================================================================
// PIN KONFIGURATION
// ============================================================================
#define PIN_DIT_PADDLE    2       // Dit Paddle (aktiv LOW mit INPUT_PULLUP)
#define PIN_DAH_PADDLE    3       // Dah Paddle (aktiv LOW mit INPUT_PULLUP)
#define PIN_WPM_POT       A2      // Potentiometer für WPM (optional)
#define PIN_LED           LED_BUILTIN  // Status LED

// ============================================================================
// MIDI KONFIGURATION
// ============================================================================
#define MIDI_NOTE_DIT     1       // MIDI Note für Dit
#define MIDI_NOTE_DAH     2       // MIDI Note für Dah
#define MIDI_CHANNEL      0       // MIDI Kanal (0 = Kanal 1)
#define MIDI_VELOCITY     127     // Velocity (Lautstärke)
#define MIDI_CC_WPM       61      // Control Change für WPM

// ============================================================================
// TIMING KONFIGURATION
// ============================================================================
#define DEFAULT_WPM       20      // Standard WPM
#define MIN_WPM           6       // Minimum WPM
#define MAX_WPM           48      // Maximum WPM
#define DEBOUNCE_MS       5       // Debounce Zeit in Millisekunden

// ============================================================================
// GLOBALE VARIABLEN
// ============================================================================
// Paddle Zustände
bool ditState = false;            // Aktueller Dit Paddle Status
bool dahState = false;            // Aktueller Dah Paddle Status
bool lastDitState = false;        // Letzter Dit Status (für Flankenerkennung)
bool lastDahState = false;        // Letzter Dah Status (für Flankenerkennung)

// Timing
unsigned long lastDitDebounce = 0;
unsigned long lastDahDebounce = 0;

// WPM
int currentWPM = DEFAULT_WPM;     // Aktuelle WPM
int lastWPM = DEFAULT_WPM;        // Letzte gesendete WPM
unsigned long lastWPMUpdate = 0;  // Letztes WPM Update

// ============================================================================
// SETUP
// ============================================================================
void setup() {
  // Pin-Konfiguration
  pinMode(PIN_DIT_PADDLE, INPUT_PULLUP);
  pinMode(PIN_DAH_PADDLE, INPUT_PULLUP);
  pinMode(PIN_LED, OUTPUT);
  
  // LED Startup Sequenz (3x blinken)
  for (int i = 0; i < 3; i++) {
    digitalWrite(PIN_LED, HIGH);
    delay(100);
    digitalWrite(PIN_LED, LOW);
    delay(100);
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
  // WPM von Potentiometer lesen (optional)
  readWPM();
  
  // Paddle Status lesen (mit Debouncing)
  readPaddles();
  
  // Dit Paddle - Flanke erkennen
  if (ditState && !lastDitState) {
    // Dit Paddle gedrückt - Note On senden
    sendNoteOn(MIDI_NOTE_DIT);
    digitalWrite(PIN_LED, HIGH);
  }
  else if (!ditState && lastDitState) {
    // Dit Paddle losgelassen - Note Off senden
    sendNoteOff(MIDI_NOTE_DIT);
    digitalWrite(PIN_LED, LOW);
  }
  
  // Dah Paddle - Flanke erkennen
  if (dahState && !lastDahState) {
    // Dah Paddle gedrückt - Note On senden
    sendNoteOn(MIDI_NOTE_DAH);
    digitalWrite(PIN_LED, HIGH);
  }
  else if (!dahState && lastDahState) {
    // Dah Paddle losgelassen - Note Off senden
    sendNoteOff(MIDI_NOTE_DAH);
    digitalWrite(PIN_LED, LOW);
  }
  
  // Status speichern für nächste Iteration
  lastDitState = ditState;
  lastDahState = dahState;
  
  // Kurze Pause (nicht blockierend)
  delay(1);
}

// ============================================================================
// PADDLE LESEN MIT DEBOUNCING
// ============================================================================
void readPaddles() {
  unsigned long now = millis();
  
  // Dit Paddle lesen
  bool ditReading = !digitalRead(PIN_DIT_PADDLE);  // Invertiert wegen INPUT_PULLUP
  if (ditReading != ditState) {
    if (now - lastDitDebounce > DEBOUNCE_MS) {
      ditState = ditReading;
      lastDitDebounce = now;
    }
  }
  
  // Dah Paddle lesen
  bool dahReading = !digitalRead(PIN_DAH_PADDLE);  // Invertiert wegen INPUT_PULLUP
  if (dahReading != dahState) {
    if (now - lastDahDebounce > DEBOUNCE_MS) {
      dahState = dahReading;
      lastDahDebounce = now;
    }
  }
}

// ============================================================================
// WPM VON POTENTIOMETER LESEN (OPTIONAL)
// ============================================================================
void readWPM() {
  // Nur alle 100ms lesen um MIDI nicht zu überlasten
  unsigned long now = millis();
  if (now - lastWPMUpdate < 100) {
    return;
  }
  lastWPMUpdate = now;
  
  // Analog lesen (0-1023)
  int potValue = analogRead(PIN_WPM_POT);
  
  // Auf WPM Range mappen (MIN_WPM bis MAX_WPM)
  currentWPM = map(potValue, 0, 1023, MIN_WPM, MAX_WPM);
  
  // Nur senden wenn sich WPM geändert hat
  if (currentWPM != lastWPM) {
    sendWPM(currentWPM);
    lastWPM = currentWPM;
  }
}

// ============================================================================
// MIDI NOTE ON SENDEN
// ============================================================================
void sendNoteOn(byte note) {
  midiEventPacket_t noteOn = {
    0x09,                           // Note On Event (Cable 0, Code Index 9)
    0x90 | MIDI_CHANNEL,           // Note On Kanal
    note,                          // Note Nummer
    MIDI_VELOCITY                  // Velocity
  };
  MidiUSB.sendMIDI(noteOn);
  MidiUSB.flush();
}

// ============================================================================
// MIDI NOTE OFF SENDEN
// ============================================================================
void sendNoteOff(byte note) {
  midiEventPacket_t noteOff = {
    0x08,                           // Note Off Event (Cable 0, Code Index 8)
    0x80 | MIDI_CHANNEL,           // Note Off Kanal
    note,                          // Note Nummer
    0                              // Velocity (0 für Note Off)
  };
  MidiUSB.sendMIDI(noteOff);
  MidiUSB.flush();
}

// ============================================================================
// WPM ALS CONTROL CHANGE SENDEN
// ============================================================================
void sendWPM(int wpm) {
  // WPM auf MIDI Wertebereich begrenzen (0-127)
  byte midiValue = constrain(wpm, 0, 127);
  
  midiEventPacket_t controlChange = {
    0x0B,                           // Control Change Event (Cable 0, Code Index 11)
    0xB0 | MIDI_CHANNEL,           // Control Change Kanal
    MIDI_CC_WPM,                   // Control Change Nummer (61)
    midiValue                      // Wert (WPM)
  };
  MidiUSB.sendMIDI(controlChange);
  MidiUSB.flush();
}

// ============================================================================
// HILFSFUNKTION: LED BLINKEN (für Debug)
// ============================================================================
void blinkLED(int count, int delayMs) {
  for (int i = 0; i < count; i++) {
    digitalWrite(PIN_LED, HIGH);
    delay(delayMs);
    digitalWrite(PIN_LED, LOW);
    delay(delayMs);
  }
}
