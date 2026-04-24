int out1 = 22, out2 = 21, enA = 23;

void setup(){
  pinMode(out1, OUTPUT);
  pinMode(out2, OUTPUT);
  pinMode(enA, OUTPUT);
  Serial.begin(9600);
}

void loop(){
  analogWrite(enA, 200);
  digitalWrite(out1, HIGH);
  digitalWrite(out2, LOW);
}
