#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// Temp Code
int period = 3000;
unsigned long time_now = 0;

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

// Set up the Bluetooth serial connection
LiquidCrystal_I2C lcd(0x27, 16, 2);

void setup() {
  Serial.begin(38400);
  lcd.begin();
  lcd.clear();
  lcd.backlight();
  lcd.println("Starting.");
}

void lcdPrint(String data) {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(data);
}

int x = 0;
int y = 0;
int z = 0;
int speed = 500;
String dataString = "0,0,0\n";

void loop() {

  if (millis() >= time_now + period) {
    time_now += period;

    int x = random(0, 3);  // 0:idle, 1:left, 2:right
    int y = random(0, 3);  // 0:idle, 1:rotateLeft, 2:rotateRight
    int z = random(0, 3);  // 0:idle, 1:up, 2:down
    speed = random(100, 501);

    dataString = String(x) + "," + 
                 String(y) + "," + 
                 String(z) + "," + 
                 String(speed);

    lcdPrint(dataString);

    Serial.print(dataString + "\n");
  }

  delay(10);
}