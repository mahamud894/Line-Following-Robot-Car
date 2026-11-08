/*
 * 2-Wheel Autonomous Line Follower Robot - v2: 8-channel QTR (RC) array + P control
 * Author: Mahamud Hassan
 * Library: "QTRSensors" by Pololu (install via Arduino Library Manager)
 */
#include <QTRSensors.h>

QTRSensors qtr;
const uint8_t SensorCount = 8;
uint16_t sensorValues[SensorCount];

// Motor pins
const int ENA = 5, ENB = 6;
const int IN1 = 7, IN2 = 8, IN3 = 9, IN4 = 10;

// Tuning values - adjust these for your robot
int baseSpeed = 150;
int maxSpeed  = 200;
float Kp = 0.04;

void setup() {
  qtr.setTypeRC();
  qtr.setSensorPins((const uint8_t[]){A0, A1, A2, A3, A4, A5, 2, 3}, SensorCount);

  pinMode(ENA, OUTPUT); pinMode(ENB, OUTPUT);
  pinMode(IN1, OUTPUT); pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT); pinMode(IN4, OUTPUT);
  pinMode(LED_BUILTIN, OUTPUT);

  // Calibration (~10 s): pin 13 LED stays on. Slide the robot by hand left and
  // right across the line so every sensor sees both black line and white floor.
  digitalWrite(LED_BUILTIN, HIGH);
  for (int i = 0; i < 400; i++) {
    qtr.calibrate();
  }
  digitalWrite(LED_BUILTIN, LOW);
}

void loop() {
  // 0 = line far left, 3500 = centered, 7000 = line far right
  uint16_t position = qtr.readLineBlack(sensorValues);
  int error = position - 3500;
  int correction = Kp * error;

  int leftSpeed  = constrain(baseSpeed + correction, 0, maxSpeed);
  int rightSpeed = constrain(baseSpeed - correction, 0, maxSpeed);

  digitalWrite(IN1, HIGH); digitalWrite(IN2, LOW);
  digitalWrite(IN3, HIGH); digitalWrite(IN4, LOW);
  analogWrite(ENA, leftSpeed);
  analogWrite(ENB, rightSpeed);
}
