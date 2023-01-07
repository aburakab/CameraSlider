#include <AccelStepper.h>
#include <SoftwareSerial.h>

#define STEPPER_ENABLE_PIN 8

const int ACCELARATION = 500;  // التسارع
const float MAX_SPEED = 750;

const int yRev = 1700;
const int zRev = 609;

String blValue;

// This is for Nano
AccelStepper yaxis(AccelStepper::DRIVER, 6, 3);
AccelStepper zaxis(AccelStepper::DRIVER, 7, 4);

enum zMovement_enum : int {
  idleZ = 0,
  up = 1,
  down = 2
};
enum yMovement_enum : int {
  idleY = 0,
  rotateLeft = 1,
  rotateRight = 2
};
enum xMovement_enum : int {
  idleX = 0,
  left = 1,
  right = 2
};

struct PacketData {
  xMovement_enum xAxisValue;
  yMovement_enum yAxisValue;
  zMovement_enum zAxisValue;
  int speed;
};

PacketData data;

void setup() {

  pinMode(13, OUTPUT);
  pinMode(STEPPER_ENABLE_PIN, OUTPUT);
  digitalWrite(STEPPER_ENABLE_PIN, HIGH);

  yaxis.setMaxSpeed(MAX_SPEED);
  yaxis.setSpeed(MAX_SPEED / 2);
  yaxis.setAcceleration(ACCELARATION);

  zaxis.setMaxSpeed(MAX_SPEED);
  zaxis.setSpeed(MAX_SPEED / 2);
  zaxis.setAcceleration(ACCELARATION);

  digitalWrite(STEPPER_ENABLE_PIN, LOW);

  Serial.begin(38400);
}

void loop() {

  if (Serial.available() > 0) {
    blValue = Serial.readStringUntil('\n');
    sscanf(blValue.c_str(), "%d,%d,%d,%d",
           &data.xAxisValue,
           &data.yAxisValue,
           &data.zAxisValue,
           &data.speed);

    if (data.zAxisValue == idleZ) {
      zaxis.setSpeed(0);
    } else if (data.zAxisValue == up) {
      zaxis.setSpeed(data.speed);
    } else if (data.zAxisValue == down) {
      zaxis.setSpeed(-data.speed);
    }
  }

  zaxis.runSpeed();
}

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

void move(long x, long y, long z) {
  // xaxis.move(x);
  yaxis.move(y);
  zaxis.move(z);

  do {
    poll_steppers();
  } while (isMoving());
}
bool isMoving() {
  return (/*xaxis.isRunning() ||*/ yaxis.isRunning() || zaxis.isRunning());
}
void poll_steppers() {
  // xaxis.run();
  yaxis.run();
  zaxis.run();
}
