#include <Arduino.h>

const uint8_t lEcho = 33, lTrig = 32, rEcho = 27, rTrig = 14, fEcho = 26, fTrig = 25;

auto calculateDistance(int trigPin, int echoPin){
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  
  // Sets the trigPin on HIGH state for 10 micro seconds
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  
  // Read the echoPin, returns the sound wave travel time in microseconds
  long duration = pulseIn(echoPin, HIGH);
  
  // Calculate the distance
  float distanceCm = duration * 0.034 / 2;
  
  return distanceCm;
}
void setup() {
  pinMode(lEcho, INPUT);
  pinMode(lTrig, OUTPUT);
  pinMode(fEcho, INPUT);
  pinMode(fTrig, OUTPUT);
  pinMode(rEcho, INPUT);
  pinMode(rTrig, OUTPUT);
  Serial.begin(115200);
}

void loop() {
  float lDis = calculateDistance(lTrig, lEcho);
  float fDis = calculateDistance(fTrig, fEcho);
  float rDis = calculateDistance(rTrig, rEcho);
  Serial.println(lDis);
  Serial.println(fDis);
  Serial.println(rDis);
  delay(50);

}
