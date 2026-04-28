int out1 = 22, out2 = 21, enA = 23, out3 = 19, out4 = 18, enB = 15, IR = 13;

void setup(){
  pinMode(out1, OUTPUT);
  pinMode(out2, OUTPUT);
  pinMode(out3, OUTPUT);
  pinMode(out4, OUTPUT);
  pinMode(enA, OUTPUT);
  pinMode(enB, OUTPUT);
  analogWrite(enA, 90);
  analogWrite(enB, 100);
  Serial.begin(9600);
}

void loop(){
  Serial.println("HIGH");
  digitalWrite(out1, HIGH);
  digitalWrite(out2, LOW);
  digitalWrite(out3, HIGH);
  digitalWrite(out4, LOW);
}
