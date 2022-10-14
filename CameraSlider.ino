const int HOME_Y_VALUE = 620;
const int HOME_Z_VALUE = 45;

const int HOME_Y_SENSOR_PIN = A1;
const int HOME_Z_SENSOR_PIN = A0;

const int STEPPERS_COUNT = 3;

const int ACCELARATION = 500;  // التسارع
const float MAX_SPEED = 200;

#define STEPPER_ENABLE_PIN 8  // LOW -> turns on the drivers

// =============================================================================

#include <AccelStepper.h>

// =============================================================================

// Declare three AccelStepper objects to manage the output timing.
AccelStepper xaxis(AccelStepper::DRIVER, 2, 5);
AccelStepper yaxis(AccelStepper::DRIVER, 3, 6);
AccelStepper zaxis(AccelStepper::DRIVER, 4, 7);

bool _homingZ = true;
bool _homingY = true;

void setup() {

  pinMode(HOME_Z_SENSOR_PIN, OUTPUT);

  // initialize drivers in disabled state
  digitalWrite(STEPPER_ENABLE_PIN, HIGH);
  pinMode(STEPPER_ENABLE_PIN, OUTPUT);

  xaxis.setMaxSpeed(4000.0);
  xaxis.setAcceleration(8000.0);

  yaxis.setMaxSpeed(4000.0);
  yaxis.setAcceleration(8000.0);

  zaxis.setMaxSpeed(4000.0);
  zaxis.setAcceleration(8000.0);

  // set up the serial port for debugging output
  Serial.begin(115200);

  // enable the drivers, the motors will remain constantly energized
  digitalWrite(STEPPER_ENABLE_PIN, LOW);
}

void poll_steppers() {
  xaxis.run();
  yaxis.run();
  zaxis.run();
}

// Moving Logic ================================================================

/// Return true if any one of the drivers are still moving.
bool isMoving() {
  return (xaxis.isRunning() || yaxis.isRunning() || zaxis.isRunning());
}

/// Move a relative displacement at the current speed, blocking until the move is done.
void move(long x, long y, long z) {
  xaxis.move(x);
  yaxis.move(y);
  zaxis.move(z);

  do {
    poll_steppers();
  } while (isMoving());
}

// Homing Logic ================================================================

long _magnetFoundOn = 0;
bool firstMagnetFound = false;
void homingY() {
  _magnetFoundOn = millis();
  while (_homingY) {
    int ySensorValue = analogRead(HOME_Y_SENSOR_PIN);
    if (ySensorValue >= HOME_Y_VALUE && !firstMagnetFound) {
      long now = millis();
      long diff = now - _magnetFoundOn;
      _magnetFoundOn = now;
      if (diff > 3000) {
        firstMagnetFound = true;
        Serial.println("Yes => diff > 3000");
      }
    }
    yaxis.setSpeed(4000);
    yaxis.runSpeed();

    if (firstMagnetFound) {
      move(0, 7000, 0);
      _homingY = false;
      break;
    }
  }
}

void homingZ() {
  while (_homingZ) {
    double runningTime = millis();
    int zSensorValue = analogRead(HOME_Z_SENSOR_PIN);
    delayMicroseconds(1000);
    Serial.println(zSensorValue);
    if (zSensorValue == HOME_Z_VALUE) {
      if (runningTime > 100) {
        move(0, 0, 550);
        zaxis.stop();
      }
      _homingZ = false;
      break;
    }
    zaxis.setSpeed(3000);
    zaxis.runSpeed();
  }
}

// =============================================================================

// read Serial until until_c char found, returns true when found else false
// non-blocking, until_c is returned as last char in String, updates input String with chars read
bool readStringUntil(String& input, char until_c) {
  while (Serial.available()) {
    char c = Serial.read();
    input += c;
    if (c == until_c) {
      return true;
    }
  }
  return false;
}

// =============================================================================

String _input = "";  // for incoming serial data

void loop() {

  homingZ();
  homingY();

  // read until find newline
  if (readStringUntil(_input, '\n') && !_homingZ && !_homingY) {

    _input.trim();
    _input.replace("\n", "");
    
    Serial.print(F(" got a line of input '"));
    Serial.print(_input);
    Serial.println("'");

    bool isItNegative = false;
    if(_input.startsWith("-")) {
      isItNegative = true;
      _input.replace("-", "");
    }
    
    int inputInt = _input.toInt();
    if(inputInt > 0) {
      if(isItNegative) {
        move(-inputInt, -inputInt, -inputInt);  
      }
      else {
        move(inputInt, inputInt, inputInt);  
      }
    }
    else if(_input.equals("hz")) {
      move(0, 0, 500);
      _homingZ = true;
    }
    else if(_input.equals("hy")) {
      move(0, 1000, 0);
      _homingY = true;
      _magnetFoundOn = 0;
      firstMagnetFound = false;
    }
    else {
      Serial.println("Ops!");
    }
    _input = "";
  }

  // move(0, 0, 2000);
  // delay(500);

  // move(0, 0, 0);
  // delay(500);
}
