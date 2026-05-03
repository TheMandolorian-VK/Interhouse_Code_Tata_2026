#include <Arduino.h>

int out1 = 22, out2 = 21, enA = 23, out3 = 19, out4 = 18, enB = 15, lIR = 13, rIR = 12;

int freq = 10000;
int res = 8;

int enAspeed, enBspeed;

int linearPWM = 160;
int turnPWM = 140;


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
  enAspeed = speedM;

  ledcWrite(enA, enAspeed);
  ledcWrite(enB, enBspeed);

  if(mode == "LF"){
    digitalWrite(out1, LOW); digitalWrite(out2, HIGH);
    digitalWrite(out3, HIGH); digitalWrite(out4, LOW);
  }else if(mode == "Manual"){
    digitalWrite(out1, LOW); digitalWrite(out2, LOW);
    digitalWrite(out3, HIGH); digitalWrite(out4, LOW);  
  }
}
void turnRight(int speedM, String mode) {
  enBspeed = speedM;
  enAspeed = speedM;
  
  ledcWrite(enA, enAspeed);
  ledcWrite(enB, enBspeed);

  if(mode == "LF"){
    digitalWrite(out1, HIGH); digitalWrite(out2, LOW);
    digitalWrite(out3, LOW); digitalWrite(out4, HIGH);
  }else if(mode == "Manual"){
    digitalWrite(out1, HIGH); digitalWrite(out2, LOW);
    digitalWrite(out3, LOW); digitalWrite(out4, LOW);  
  }
}

void moveBackward(int speedM) {
  enBspeed = speedM;
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

void setup(){
  pinMode(lIR, INPUT);
  pinMode(rIR, INPUT);
  pinMode(out1, OUTPUT);
  pinMode(out2, OUTPUT);
  pinMode(out3, OUTPUT);
  pinMode(out4, OUTPUT);

  ledcAttach(enA, freq, res);
  ledcAttach(enB, freq, res);

  ledcWrite(enA, enAspeed);
  ledcWrite(enB, enBspeed);
  Serial.begin(115200);
}

/*
 * Normally, 0, 0 is for both on white line
 * 
 */

int lastTurn = 0;

void loop(){
  int sLread = digitalRead(lIR);
  int sRread = digitalRead(rIR);

  if(sLread == LOW && sRread == LOW){
    moveForward(linearPWM);
  }
  else if(sLread == HIGH && sRread == LOW){
    lastTurn = 1;
    turnLeft(turnPWM, "LF");
  }
  else if(sLread == LOW && sRread == HIGH){
    lastTurn = 2;
    turnRight(turnPWM, "LF");
  }
  else if(sLread == HIGH && sRread == HIGH){
    if(lastTurn == 1){
      turnLeft(turnPWM, "LF");
      delay(10);
    }
    else if(lastTurn == 2){
      turnRight(turnPWM, "LF");
      delay(10);
    }
    else{
     moveBackward(linearPWM);  
    }
  }
}
