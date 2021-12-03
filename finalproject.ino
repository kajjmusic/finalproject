
//int nextPresetButton = 31;
//int prevPresetButton = 32;
//int loadPresetButton = 37;
//int savePresetButton = 38;
#include <EEPROM.h>

int presetSettings[4][20] = {
  {33, 66, 100, 0, 0, 0, 0 },
  {33, 66, 100, 0, 0, 0, 0,}
};
char *presetName[] = { "Small Room", "Large Hall", "Cavern", "User1", "User2", "User3", "User4" };
bool buttonState[4] = {LOW, LOW, LOW, LOW};
bool lastButtonState[4] = {LOW, LOW, LOW, LOW};
int buttonPins[4] = {31, 32, 37, 38};
boolean buttonOn[4] = {false, false, false, false};
int mappedReverbVal = 0;
int mappedReduxVal = 0;
int mappedDelayVal = 0;
int mappedChorusVal = 0;
int lastReduxVal = 0;
int lastReverbVal = 0;
int lastChorusVal = 0;
int lastDelayVal = 0;
int currentPreset = 0;
int reduxLedPin = 30;
int reverbLedPin = 29;
int chorusLedPin = 28;
int delayLedPin = 27;
int reduxDisplayVal = 0;
int reverbDisplayVal = 0;
int chorusDisplayVal = 0;
int delayDisplayVal = 0;




void setup() {
  for (int i = 0; i < 4; i++) {
    pinMode(buttonPins[i], INPUT);
  }

  pinMode(reduxLedPin, OUTPUT);
  pinMode(reverbLedPin, OUTPUT);
  pinMode(chorusLedPin, OUTPUT);
  pinMode(delayLedPin, OUTPUT);


  Serial.begin(9600);
}

void loop() {
  mapValues();
  savePreset();
  loadPreset();
  changePreset();
  checkButtons();
  controlLeds();
  if (currentPreset > 2) {
    if (lastReduxVal != mappedReduxVal) {
      usbMIDI.sendControlChange(1, mappedReduxVal, 1);
      lastReduxVal = mappedReduxVal;
    }
    if (lastReverbVal != mappedReverbVal) {
      usbMIDI.sendControlChange(2, mappedReverbVal, 2);
      lastReverbVal = mappedReverbVal;
    }
    if (lastChorusVal != mappedChorusVal) {
      usbMIDI.sendControlChange(3, mappedChorusVal, 3);
      lastChorusVal = mappedChorusVal;
    }
    if (lastDelayVal != mappedDelayVal) {
      usbMIDI.sendControlChange(4, mappedDelayVal, 4);
      lastDelayVal = mappedDelayVal;
    }
  } else {
    usbMIDI.sendControlChange(2, presetSettings[0][currentPreset], 2);
    usbMIDI.sendControlChange(3, presetSettings[1][currentPreset], 3);
  }
}

void checkButtons() {
  for (int i = 0; i < 4; i++) {
    lastButtonState[i] = buttonState[i];
    buttonState[i] = digitalRead(buttonPins[i]);
    if (lastButtonState[i] == HIGH and buttonState[i] == LOW) {
      buttonOn[i] = true;
      delay(5);
    } else {
      buttonOn[i] = false;
    }
  }
}

void controlLeds() {
  analogWrite(reduxLedPin, reduxDisplayVal);
  analogWrite(reverbLedPin, reverbDisplayVal);
  analogWrite(chorusLedPin, chorusDisplayVal);
  analogWrite(delayLedPin, delayDisplayVal);
}

void changePreset() {
  if (buttonOn[0] == true) {
    currentPreset += 1;
    if (currentPreset == 7) {
      currentPreset = 0;
    }
    Serial.print("Current Preset: ");
    Serial.println(presetName[currentPreset]);
    delay(50);
  }
  if (buttonOn[1] == true) {
    currentPreset -= 1;
    if (currentPreset == -1) {
      currentPreset = 6;
    }
    Serial.print("Current Preset: ");
    Serial.println(presetName[currentPreset]);
    delay(50);
  }
}

void savePreset() {
  if (buttonOn[3] == true) {
    if (currentPreset > 2) {
      for (int i = 0; i < 2; i++) {
        if (i == 0) {
          EEPROM.write(i * 7 + currentPreset, mappedReverbVal);
        }
        if (i == 1) {
          EEPROM.write(i * 7 + currentPreset, mappedChorusVal);
        }
      }
      Serial.println("Preset saved!");
      Serial.print("Reverb: ");
      Serial.println(mappedReverbVal);
      Serial.print("Chorus: ");
      Serial.println(mappedChorusVal);
    } else {
      Serial.println("You cannot overwrite a default preset!");
    }
  }
}

void loadPreset() {
  if (buttonOn[2] == true) {
    if (currentPreset > 2) {
      for (int i = 0; i < 2; i++) {
        presetSettings[i][currentPreset] = EEPROM.read(i * 7 + currentPreset);
      }
      Serial.println("Preset loaded: ");
      Serial.print("Reverb: ");
      Serial.println(presetSettings[0][currentPreset]);
      Serial.print("Chorus: ");
      Serial.println(presetSettings[1][currentPreset]);
    } else {
      Serial.println("You cannot overwrite a default preset!");
    }
  }
}

void mapValues() {
  mappedReduxVal = constrain(map(analogRead(A15), 555, 920, 0, 127), 0, 127);
  mappedReverbVal = constrain(map(analogRead(A16), 0, 1023, 0, 127), 0, 127);
  mappedChorusVal = constrain(map(analogRead(A17), 0, 1023, 0, 127), 0, 127);
  mappedDelayVal = constrain(map(analogRead(A14), 555, 920, 0, 127), 0, 127);

  reduxDisplayVal = constrain(map(analogRead(A15), 555, 920, 255, 0), 0, 255);
  reverbDisplayVal = constrain(map(analogRead(A16), 0, 1023, 0, 255), 0, 255);
  chorusDisplayVal = constrain(map(analogRead(A17), 0, 1023, 0, 255), 0, 255);
  delayDisplayVal = constrain(map(analogRead(A14), 555, 920, 255, 0), 0, 255);
}
