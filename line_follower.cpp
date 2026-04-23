bool running = true;

const uint8_t leftIR = 12, rightIR = 13, motorL1 = 22, motorL2 = 21, motorR1 = 19, motorR2= 18, enA = 23, enB = 5;
class Vehicle {
private:
  int _leftIR, _rightIR, _motorL1, _motorL2, _motorR1, _motorR2, _enA, _enB;
public:
  Vehicle(int leftIR, int rightIR, int motorL1, int motorR1, int motorL2, int motorR2, int enA, int enB) {
    _leftIR = leftIR;
    _rightIR = rightIR;
    _motorL1 = motorL1;
    _motorL2 = motorL2;
    _motorR1 = motorR1;
    _motorR2 = motorR2;
    _enA = enA;
    _enB = enB;

    pinMode(_leftIR, INPUT);
    pinMode(_rightIR, INPUT);
    pinMode(_motorL1, OUTPUT);
    pinMode(_motorL2, OUTPUT);
    pinMode(_motorR1, OUTPUT);
    pinMode(_motorR2, OUTPUT);
    pinMode(_enA, OUTPUT);
    pinMode(_enB, OUTPUT);

    analogWrite(_enA, 150);
    analogWrite(_enB, 150);
  }

  void moveForward() {
    digitalWrite(_motorL1, HIGH);
    digitalWrite(_motorL2, LOW);
    digitalWrite(_motorR1, HIGH);
    digitalWrite(_motorR2, LOW);
  }
  void turnLeft() {
    digitalWrite(_motorL1, LOW);
    digitalWrite(_motorL2, LOW);
    digitalWrite(_motorR1, LOW);
    digitalWrite(_motorR2, HIGH);
  }
  void turnRight() {
    digitalWrite(_motorL1, LOW);
    digitalWrite(_motorL2, HIGH);
    digitalWrite(_motorR1, LOW);
    digitalWrite(_motorR2, LOW);
  }
  void stop() {
    digitalWrite(_motorL1, LOW);
    digitalWrite(_motorL2, LOW);
    digitalWrite(_motorR1, LOW);
    digitalWrite(_motorR2, LOW);
  }
  bool returnLSensorState() {
    int sensorLeftState = digitalRead(_leftIR);
    return sensorLeftState;
  }
  bool returnRSensorState() {
    int sensorRightState = digitalRead(_rightIR);
    return sensorRightState;
  }
};

Vehicle LineFollower(leftIR, rightIR, motorL1, motorL2, motorR1, motorR2, enA, enB);

void setup() {
  Serial.begin(9600);
}

void loop() {
  while (running) {
    bool sLstate = LineFollower.returnLSensorState();
    bool sRstate = LineFollower.returnRSensorState();
    if (sLstate == LOW && sRstate == LOW) {
      LineFollower.moveForward();
    } else if (sLstate == LOW && sRstate == HIGH) {
      LineFollower.turnRight();
    } else if (sLstate == HIGH && sRstate == LOW) {
      LineFollower.turnLeft();
    } else if (sLstate == HIGH && sRstate == HIGH)
      LineFollower.stop();  
  }
}
