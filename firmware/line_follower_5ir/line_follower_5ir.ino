/*
 * 2-Wheel Autonomous Line Follower Robot - v1: 5-channel digital IR array
 * Author: Mahamud Hassan
 * Arduino Uno R3 + 5-channel digital IR array + L298N + 2x N20 motors
 * Black line = HIGH, white floor = LOW (swap HIGH/LOW if your module is inverted)
 */

// 5 IR sensors, left -> right
const int SENSOR_FAR_LEFT   = 2;
const int SENSOR_NEAR_LEFT  = 3;
const int SENSOR_CENTER     = 4;
const int SENSOR_NEAR_RIGHT = 5;
const int SENSOR_FAR_RIGHT  = 6;

// L298N motor driver
const int ENA = 9;   // left motor speed (PWM)
const int IN1 = 7;
const int IN2 = 8;
const int IN3 = 11;
const int IN4 = 12;
const int ENB = 10;  // right motor speed (PWM)

// Speeds (0-255)
const int BASE_SPEED      = 70;
const int TURN_SPEED_HIGH = 85;
const int TURN_SPEED_LOW  = 25;
int lastDirection = 0;  // 0 = center, 1 = left, 2 = right (line memory)

void setup() {
  pinMode(SENSOR_FAR_LEFT, INPUT);
  pinMode(SENSOR_NEAR_LEFT, INPUT);
  pinMode(SENSOR_CENTER, INPUT);
  pinMode(SENSOR_NEAR_RIGHT, INPUT);
  pinMode(SENSOR_FAR_RIGHT, INPUT);

  pinMode(ENA, OUTPUT); pinMode(ENB, OUTPUT);
  pinMode(IN1, OUTPUT); pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT); pinMode(IN4, OUTPUT);

  stopMotors();
  delay(3000);  // time to place the robot on the track
}

void loop() {
  int s1 = digitalRead(SENSOR_FAR_LEFT);
  int s2 = digitalRead(SENSOR_NEAR_LEFT);
  int s3 = digitalRead(SENSOR_CENTER);
  int s4 = digitalRead(SENSOR_NEAR_RIGHT);
  int s5 = digitalRead(SENSOR_FAR_RIGHT);

  if (s3 == HIGH && s2 == LOW && s4 == LOW) {            // on the line: straight
    moveForward(BASE_SPEED, BASE_SPEED);
    lastDirection = 0;
  }
  else if (s2 == HIGH && s1 == LOW) {                    // gentle left
    moveForward(TURN_SPEED_LOW, BASE_SPEED);
    lastDirection = 1;
  }
  else if (s1 == HIGH) {                                 // sharp left
    moveForward(0, TURN_SPEED_HIGH);
    lastDirection = 1;
  }
  else if (s4 == HIGH && s5 == LOW) {                    // gentle right
    moveForward(BASE_SPEED, TURN_SPEED_LOW);
    lastDirection = 2;
  }
  else if (s5 == HIGH) {                                 // sharp right
    moveForward(TURN_SPEED_HIGH, 0);
    lastDirection = 2;
  }
  else if (s1 == LOW && s2 == LOW && s3 == LOW && s4 == LOW && s5 == LOW) {
    // line lost: search toward the side where it was last seen
    if (lastDirection == 1)      moveForward(0, TURN_SPEED_HIGH);
    else if (lastDirection == 2) moveForward(TURN_SPEED_HIGH, 0);
    else                         stopMotors();
  }
}

void moveForward(int leftSpeed, int rightSpeed) {
  digitalWrite(IN1, HIGH); digitalWrite(IN2, LOW);
  digitalWrite(IN3, HIGH); digitalWrite(IN4, LOW);
  analogWrite(ENA, leftSpeed);
  analogWrite(ENB, rightSpeed);
}

void stopMotors() {
  analogWrite(ENA, 0); analogWrite(ENB, 0);
  digitalWrite(IN1, LOW); digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW); digitalWrite(IN4, LOW);
}
