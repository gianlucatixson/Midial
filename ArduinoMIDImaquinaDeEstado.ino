#include "MIDIUSB.h"
#include "PitchToNote.h"

#define NUM_BUTTONS  7

//pulsadores pitch++ y pitch--
#define BTN_PITCH_MAS 5
#define BTN_PITCH_MENOS 7

//contador
int escala = 0;

//pines LDRs
const uint8_t LDR1 = 16;
const uint8_t LDR2 = 15;
const uint8_t LDR3 = 14;
const uint8_t LDR4 = 11;
const uint8_t LDR5 = 10;
const uint8_t LDR6 = 9;
const uint8_t LDR7 = 8;
const int intensityPot = 0;

//configuracion midi
const uint8_t buttons[NUM_BUTTONS] = {LDR1, LDR2, LDR3, LDR4, LDR5, LDR6, LDR7};


//escalas (estan en mayor)
const byte notePitches_C[NUM_BUTTONS] = {pitchC3, pitchD3, pitchE3, pitchF3, pitchG3, pitchA3, pitchB3};
const byte notePitches_D[NUM_BUTTONS] = {pitchD3, pitchE3, pitchG3b, pitchG3, pitchA3, pitchB3, pitchD4b};
const byte notePitches_E[NUM_BUTTONS] = {pitchE3, pitchG3b, pitchA3b, pitchA3, pitchB3, pitchD4b, pitchE4b};
const byte notePitches_F[NUM_BUTTONS] = {pitchF3, pitchG3, pitchA3, pitchB3b, pitchC4, pitchD4, pitchE4};
const byte notePitches_G[NUM_BUTTONS] = {pitchG3, pitchA3, pitchB3, pitchC3, pitchD4, pitchE4, pitchG4b};
const byte notePitches_A[NUM_BUTTONS] = {pitchA3, pitchB3, pitchD4b, pitchD4, pitchE4, pitchG4b, pitchA4b};
const byte notePitches_B[NUM_BUTTONS] = {pitchB3, pitchD4b, pitchE4b, pitchE4, pitchG4b, pitchA4b, pitchB4b};
//byte notePitches[NUM_BUTTONS] = {pitchC3, pitchD4b, pitchE4b, pitchE4, pitchG4b, pitchA4b, pitchB4b};

uint8_t notesTime[NUM_BUTTONS];
uint8_t pressedButtons = 0x00;
uint8_t previousButtons = 0x00;
uint8_t intensity;

enum scales{
    ESCALA_C_MAYOR,
    ESCALA_D_MAYOR,
    ESCALA_E_MAYOR,
    ESCALA_F_MAYOR,
    ESCALA_G_MAYOR,
    ESCALA_A_MAYOR,
    ESCALA_B_MAYOR
    };

void controlChange(byte channel, byte control, byte value) {

  midiEventPacket_t event = {0x0B, 0xB0 | channel, control, value};

  MidiUSB.sendMIDI(event);
}

void contador()
{
  int estPulsador_1 = digitalRead(BTN_PITCH_MAS);
  if (estPulsador_1 == 1){
    escala = escala + 1;
    Serial.println(escala);
    if (escala>7)
     escala = 0;
  }

  int estPulsador_0 = digitalRead(BTN_PITCH_MENOS);
  if (estPulsador_0 == 1){
    escala = escala - 1;
    Serial.println(escala);
    if(escala<0)
    escala = 7;
  }
  }

void programa(){
  
    switch (escala) {
    case ESCALA_C_MAYOR:
     playNotes (notePitches_C);
     break;
    case ESCALA_D_MAYOR:
     playNotes (notePitches_D);
     break;
    case ESCALA_E_MAYOR:
     playNotes (notePitches_E);
     break;
    case ESCALA_F_MAYOR:
     playNotes (notePitches_F);
     break;
    case ESCALA_G_MAYOR:
     playNotes (notePitches_G);
     break;
    case ESCALA_A_MAYOR:
     playNotes (notePitches_A);
      break;
    case ESCALA_B_MAYOR:
     playNotes (notePitches_B);
     break;
  }

}


void readButtons()
{
  for (int i = 0; i < NUM_BUTTONS; i++)
  {
    if (digitalRead(buttons[i]) == LOW)
    {
      bitWrite(pressedButtons, i, 1);
      delay(50);
    }
    else
      bitWrite(pressedButtons, i, 0);
  }
}

void readIntensity()
{
  int val = analogRead(intensityPot);
  intensity = (uint8_t) (map(val, 0, 1023, 0, 127));
}

void playNotes (const byte notePitches[])
{
  for (int i = 0; i < NUM_BUTTONS; i++)
  {
    if (bitRead(pressedButtons, i) != bitRead(previousButtons, i))
    {
      if (bitRead(pressedButtons, i))
      {
        bitWrite(previousButtons, i , 1);
        noteOn(0, notePitches[i], intensity);
        MidiUSB.flush();
      }
      
      else
      {
        bitWrite(previousButtons, i , 0);
        noteOff(0, notePitches[i], 0);
        MidiUSB.flush();
      }
    }
  }
}



void noteOn(byte channel, byte pitch, byte velocity) {
  midiEventPacket_t noteOn = {0x09, 0x90 | channel, pitch, velocity};
  MidiUSB.sendMIDI(noteOn);
}

void noteOff(byte channel, byte pitch, byte velocity) {
  midiEventPacket_t noteOff = {0x08, 0x80 | channel, pitch, velocity};
  MidiUSB.sendMIDI(noteOff);
}

void setup() {
  Serial.begin(9600);
  for (int i = 0; i < NUM_BUTTONS; i++)
    pinMode(buttons[i], INPUT_PULLUP);

}

void loop() {
 Serial.println(escala);
  contador();
  programa();
  readButtons();
  readIntensity();
  //playNotes();
  
}
