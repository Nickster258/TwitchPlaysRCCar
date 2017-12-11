class Motor {
  int modePin1;
  int modePin2;
  int enablePin;
  boolean defaultDirection; // Clockwise = true; Counterclockwise = false;

  public:
    Motor(int m1, int m2, int en, boolean deDir) {
      modePin1 = m1;
      modePin2 = m2;
      enablePin = en;
      boolean defaultDirection = deDir;
      pinMode(modePin1, OUTPUT);
      pinMode(modePin2, OUTPUT);
      pinMode(enablePin, OUTPUT);
    }
    void run(int speed) {
      if (speed > 0) {
        digitalWrite(modePin1, defaultDirection);
        digitalWrite(modePin2, !defaultDirection);
        analogWrite(enablePin, speed);
      } else {
        digitalWrite(modePin2, defaultDirection);
        digitalWrite(modePin1, !defaultDirection);
        analogWrite(enablePin, -speed);
      }
    }
    void brake(int speed) {
      digitalWrite(modePin1, LOW);
      digitalWrite(modePin2, LOW);
      analogWrite(enablePin, speed);
    }
};
