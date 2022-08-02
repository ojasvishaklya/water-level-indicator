#include "pitches.h"

#define TRIG 8  // rx
#define ECHO 9  // tx

#define B0 7
#define L0 6
#define L1 5
#define L2 4
#define L3 3
#define L4 2


int PROBE = A0;

int average;
int count;


// Buzzer code
// notes in the melody:
int melody[] = {
  NOTE_C4, NOTE_G3, NOTE_G3, NOTE_A3, NOTE_G3, 0, NOTE_B3, NOTE_C4
};

// note durations: 4 = quarter note, 8 = eighth note, etc.:
int noteDurations[] = {
  4, 8, 8, 4, 4, 4, 4, 4
};

void runBuzzer() {

  for (int thisNote = 0; thisNote < 8; thisNote++) {
    int noteDuration = 1000 / noteDurations[thisNote];

    tone(B0, melody[thisNote], noteDuration);

    int pauseBetweenNotes = noteDuration * 1.30;
    delay(pauseBetweenNotes);
    noTone(B0);
  }
}
// Buzzer code ENDS


void setup() {
  Serial.begin(9600);     // Serial monitoring
  pinMode(TRIG, OUTPUT);  // Initializing Trigger Output and Echo Input
  pinMode(ECHO, INPUT_PULLUP);
  pinMode(L0, OUTPUT);
  pinMode(L1, OUTPUT);
  pinMode(L2, OUTPUT);
  pinMode(L3, OUTPUT);
  pinMode(L4, OUTPUT);
  pinMode(B0, OUTPUT);
  average = 0;
  count = 0;
  // digitalWrite(A0, HIGH);
  // digitalWrite(A1, HIGH);
  // digitalWrite(A2, HIGH);
  // digitalWrite(A3, HIGH);
  // digitalWrite(A4, HIGH);
}

int percentageCalculator(int distance) {
  int sensor_height = 40;
  int level = sensor_height - distance;
  int max_level = 40;
  int percentage = (level * 100) / max_level;
  if (percentage > 100) {
    return 100;
  }
  if (percentage < 0) {
    return 0;
  }
  return percentage;
}

int getDistanceInInches(int distance) {
  return ((distance)*0.393701) / 58;
}

void powerLeds(int level) {
  digitalWrite(L0, LOW);
  digitalWrite(L1, LOW);
  digitalWrite(L2, LOW);
  digitalWrite(L3, LOW);
  digitalWrite(L4, LOW);
  if (level == 0) {

  } else if (level == 1) {
    digitalWrite(L0, HIGH);

  } else if (level == 2) {
    digitalWrite(L0, HIGH);
    digitalWrite(L1, HIGH);

  } else if (level == 3) {
    digitalWrite(L0, HIGH);
    digitalWrite(L1, HIGH);
    digitalWrite(L2, HIGH);

  } else if (level == 4) {
    digitalWrite(L0, HIGH);
    digitalWrite(L1, HIGH);
    digitalWrite(L2, HIGH);
    digitalWrite(L3, HIGH);

  } else if (level == 5) {
    digitalWrite(L0, HIGH);
    digitalWrite(L1, HIGH);
    digitalWrite(L2, HIGH);
    digitalWrite(L3, HIGH);
    digitalWrite(L4, HIGH);
  }
}

void printer(int percentage) {
  Serial.print("Current level: ");
  Serial.print(percentage);
  Serial.println(" %");
}

void ledLogic(int percentage) {
  if (percentage <= 10) {
    printer(percentage);
    powerLeds(0);
    Serial.println("- - - - -");
  } else if (percentage > 10 && percentage <= 20) {
    printer(percentage);
    powerLeds(1);
    Serial.println("X - - - -");
  } else if (percentage > 20 && percentage <= 40) {
    printer(percentage);
    powerLeds(2);
    Serial.println("X X - - -");
  } else if (percentage > 40 && percentage <= 60) {
    printer(percentage);
    powerLeds(3);
    Serial.println("X X X - -");
  } else if (percentage > 60 && percentage <= 80) {
    printer(percentage);
    powerLeds(4);
    Serial.println("X X X X -");
  } else {
    printer(percentage);
    powerLeds(5);
    Serial.println("X X X X X");
  }
}

bool checkIfValid(int percentage) {

  // to initialise average with first reading
  if (average == 0 && count == 0) {
    average = percentage;
    count = count + 1;
    return true;
  }

  // to check if new ewading is under +- 10% variation
  if (percentage <= average * 1.1 && percentage >= average * 0.9) {
    average = ((average * count + percentage) / (count + 1));
    count = count + 1;

    if (count > 10) {
      count = 1;
      average = average;
    }
    return true;
  }
  return false;
}

void loop() {

  digitalWrite(TRIG, LOW);  // Set the trigger pin to low for 2uS
  delayMicroseconds(2);

  digitalWrite(TRIG, HIGH);  // Send a 10uS high to trigger ranging
  delayMicroseconds(20);

  digitalWrite(TRIG, LOW);                    // Send pin low again
  int distance = pulseIn(ECHO, HIGH, 26000);  // Read in times pulse
  distance = getDistanceInInches(distance);
  int percentage = percentageCalculator(distance);

  // if (checkIfValid(percentage)) {
  //   ledLogic(average);
  // }
  int probeReading = analogRead(PROBE);
  Serial.print("probeReading: ");
  Serial.println(probeReading);
  if (probeReading > 0) {
    Serial.println("buzzing");
    runBuzzer();
    digitalWrite(B0, HIGH);
  } else {
    Serial.println("not buzzing");
    digitalWrite(B0, LOW);
  }
  Serial.print("distance:");
  Serial.println(distance);
  Serial.print("percentage: ");
  Serial.println(percentage);
  ledLogic(percentage);

  delay(3000);
}