#include <Arduino.h>

const uint8_t lEcho = 33, lTrig = 32, rEcho = 27, rTrig = 14, fEcho = 26, fTrig = 25, out1 = 22, out2 = 21, enA = 23, out3 = 19, out4 = 18, enB = 15;
int enAspeed, enBspeed;
int linearPWM = 160; // Increased slightly for more torque
int turnPWM = 180;

int freq = 10000;
int res = 8;
int finalDelay = 400;


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

void moveForward(int speedM) {
  enBspeed = speedM;
  enAspeed = speedM;

  ledcWrite(enA, enAspeed);
  ledcWrite(enB, enBspeed);

  digitalWrite(out1, HIGH); digitalWrite(out2, LOW);
  digitalWrite(out3, HIGH); digitalWrite(out4, LOW);
}
void turnLeft(int speedM, String mode) {
  enBspeed = speedM;
  enAspeed = 0.9 * speedM;

  ledcWrite(enA, enAspeed);
  ledcWrite(enB, enBspeed);

  if(mode == "LF"){
    digitalWrite(out1, LOW); digitalWrite(out2, HIGH);
    digitalWrite(out3, HIGH); digitalWrite(out4, LOW);
    delay(finalDelay);
  }else if(mode == "Manual"){
    digitalWrite(out1, LOW); digitalWrite(out2, LOW);
    digitalWrite(out3, HIGH); digitalWrite(out4, LOW);  
  }
}
void turnRight(int speedM, String mode) {
  enBspeed = 0.9 * speedM;
  enAspeed = speedM;
  
  ledcWrite(enA, enAspeed);
  ledcWrite(enB, enBspeed);

  if(mode == "LF"){
    digitalWrite(out1, HIGH); digitalWrite(out2, LOW);
    digitalWrite(out3, LOW); digitalWrite(out4, HIGH);
    delay(finalDelay);
  }else if(mode == "Manual"){
    digitalWrite(out1, HIGH); digitalWrite(out2, LOW);
    digitalWrite(out3, LOW); digitalWrite(out4, LOW);  
  }
}

void moveBackward(int speedM) {
  enBspeed = 0.9 * speedM;
  enAspeed = speedM;

  ledcWrite(enA, enAspeed);
  ledcWrite(enB, enBspeed);

  digitalWrite(out1, LOW); digitalWrite(out2, HIGH);
  digitalWrite(out3, LOW); digitalWrite(out4, HIGH);
}

void stopMotors() {
  ledcWrite(enA, 0);
  ledcWrite(enB, 0);
  
  digitalWrite(out1, LOW); digitalWrite(out2, LOW);
  digitalWrite(out3, LOW); digitalWrite(out4, LOW);
}

void setup() {
  pinMode(lEcho, INPUT);
  pinMode(lTrig, OUTPUT);
  pinMode(fEcho, INPUT);
  pinMode(fTrig, OUTPUT);
  pinMode(rEcho, INPUT);
  pinMode(rTrig, OUTPUT);

  pinMode(out1, OUTPUT);
  pinMode(out2, OUTPUT);
  pinMode(out3, OUTPUT);
  pinMode(out4, OUTPUT);

  ledcAttach(enA, freq, res);
  ledcAttach(enB, freq, res);

  ledcWrite(enA, linearPWM);
  ledcWrite(enB, linearPWM);

  Serial.begin(115200);
}

void loop() {
  float lDis = calculateDistance(lTrig, lEcho);
  delay(20); 
  float fDis = calculateDistance(fTrig, fEcho);
  delay(20);
  float rDis = calculateDistance(rTrig, rEcho);

  Serial.printf("L: %.2f | F: %.2f | R: %.2f\n", lDis, fDis, rDis);

  // 1. CRITICAL: Front Obstacle Handling
  if (fDis < 15) { 
    if (rDis > lDis) {
      turnRight(turnPWM, "LF");
      Serial.println("Front Blocked -> Turning Right");
    } else {
      turnLeft(turnPWM, "LF");
      Serial.println("Front Blocked -> Turning Left");
    }
  } else if (lDis < 10) {
    // Too close to left wall -> nudge right while moving
    turnRight(turnPWM, "Manual"); 
    Serial.println("Too close to Left -> Nudging Right");
    delay(50); // Short burst to clear the wall
  } 
  else if (rDis < 10) {
    // Too close to right wall -> nudge left while moving
    turnLeft(turnPWM, "Manual");
    Serial.println("Too close to Right -> Nudging Left");
    delay(50); // Short burst to clear the wall
  }

  // 3. PATH CLEAR: Move forward
  else {
    moveForward(linearPWM);
    Serial.println("Path Clear -> Forward");
  }

  delay(30); 
}
