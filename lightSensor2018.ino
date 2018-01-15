#include <elapsedMillis.h>

int sensorPin = A0;     // 
int relayPin = 7;      // Connected Relay
int sensorValue = 0;   // Connected Sensor 
int alarmTriggered = 600;
int controlValue = 0;

elapsedMillis timer;

void setup() {
  pinMode(relayPin, OUTPUT);
}
void loop() {
  ifVilkor();
}

//-------------Functions-------------//
void ifVilkor(){
  sensorValue = analogRead(sensorPin);

  if (sensorValue > alarmTriggered) {
    lightOn();
    resetTimer();
    controlValue = 1;
  }
  if ((timer > 60000) && ( controlValue == 1 )) {
    lightOff();
    controlValue = 0;
  }
  }
void resetTimer() {
  timer = 0;
}
void lightOn() {
  digitalWrite(relayPin, HIGH);
}
void lightOff() {
  digitalWrite(relayPin, LOW);
}
