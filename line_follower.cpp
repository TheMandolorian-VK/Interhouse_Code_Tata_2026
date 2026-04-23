#include <tuple>

bool running = true;

class Vehicle{
  private:
    int _leftIR, _rightIR, _motorL1, _motorL2, _motorR1, _motorR2, _enA, _enB;
  public:
    Vehicle(int leftIR, int rightIR, int motorL1, int motorR1, int motorL2, int motorR2, int enA, int enB){
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

  void moveForward(){
    digitalWrite(_motorL1, HIGH); digitalWrite(_motorL2, LOW);
    digitalWrite(_motorR1, HIGH); digitalWrite(_motorR2, LOW); 
  }
  void turnLeft(){
    digitalWrite(_motorL1, LOW); digitalWrite(_motorL2, LOW);
    digitalWrite(_motorR1, LOW); digitalWrite(_motorR2, HIGH); 
  }
  void turnRight(){
    digitalWrite(_motorL1, LOW); digitalWrite(_motorL2, HIGH);
    digitalWrite(_motorR1, LOW); digitalWrite(_motorR2, LOW); 
  }
  void stop(){
    digitalWrite(_motorL1, LOW); digitalWrite(_motorL2, LOW);
    digitalWrite(_motorR1, LOW); digitalWrite(_motorR2, LOW);
  }
  std::pair<int, int> returnSensorStates(){
    int sensorLeftState = digitalRead(_leftIR);
    int sensorRightState = digitalRead(_rightIR);
    return {sensorLeftState, sensorRightState};
  }
};

Vehicle LineFollower(12, 13, 22, 19, 21, 18, 23, 5);

void setup(){
  //enter in pin numbers tomorrow after checking connections
}

void loop(){
  while(running){
    auto [sLstate, sRstate] = LineFollower.returnSensorStates();
    if(sLState == LOW && sRstate == LOW){
      LineFollower.moveForward();
    }else if(sLState == LOW && sRstate == HIGH){
      LineFollower.turnRight();
    }else if(sLState == HIGH && sRstate == LOW){
      LineFollower.turnLeft();
    }
  
  }
}
