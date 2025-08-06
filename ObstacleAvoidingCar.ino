#include <Servo.h>
#include <SoftwareSerial.h>

// Pin definitions
#define TRIG_PIN 6
#define ECHO_PIN 7
#define LEFT_MOTOR_F 9
#define LEFT_MOTOR_B 10
#define RIGHT_MOTOR_F 11
#define RIGHT_MOTOR_B 12
#define SERVO_PIN 5
#define SAFE_DISTANCE 15

// Bluetooth module
SoftwareSerial BT(2, 3); // RX, TX

Servo servoMotor;
bool isAutonomous = false; // Default mode is manual

// Function to measure distance
long measureDistance() {
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  long duration = pulseIn(ECHO_PIN, HIGH);
  long distance = duration * 0.034 / 2; // Convert duration to distance in cm
  return distance;
}

// Car movement functions
void stopCar() {
  digitalWrite(LEFT_MOTOR_F, LOW);
  digitalWrite(LEFT_MOTOR_B, LOW);
  digitalWrite(RIGHT_MOTOR_F, LOW);
  digitalWrite(RIGHT_MOTOR_B, LOW);
}

void moveForward() {
  digitalWrite(LEFT_MOTOR_F, HIGH);
  digitalWrite(LEFT_MOTOR_B, LOW);
  digitalWrite(RIGHT_MOTOR_F, HIGH);
  digitalWrite(RIGHT_MOTOR_B, LOW);
}

void moveBackward() {
  digitalWrite(LEFT_MOTOR_F, LOW);
  digitalWrite(LEFT_MOTOR_B, HIGH);
  digitalWrite(RIGHT_MOTOR_F, LOW);
  digitalWrite(RIGHT_MOTOR_B, HIGH);
}

void turnLeft() {
  digitalWrite(LEFT_MOTOR_F, LOW);
  digitalWrite(LEFT_MOTOR_B, HIGH);
  digitalWrite(RIGHT_MOTOR_F, HIGH);
  digitalWrite(RIGHT_MOTOR_B, LOW);
}

void turnRight() {
  digitalWrite(LEFT_MOTOR_F, HIGH);
  digitalWrite(LEFT_MOTOR_B, LOW);
  digitalWrite(RIGHT_MOTOR_F, LOW);
  digitalWrite(RIGHT_MOTOR_B, HIGH);
}

void findSafeDirection() {
  long leftDistance, rightDistance;

  // Check left direction
  servoMotor.write(120);
  delay(500);
  leftDistance = measureDistance();

  // Check right direction
  servoMotor.write(60);
  delay(500);
  rightDistance = measureDistance();

  // Return servo to default position
  servoMotor.write(90);
  delay(500);

  // Decide the safest direction
  if (leftDistance > rightDistance && leftDistance > SAFE_DISTANCE) {
    turnLeft();
    delay(1000);
  } else if (rightDistance > SAFE_DISTANCE) {
    turnRight();
    delay(1000);
  } else {
    stopCar();
  }
}

void autonomousMode() {
  long distance = measureDistance();

  if (distance < SAFE_DISTANCE) {
    stopCar();
    delay(500);
    moveBackward();
    delay(1000);
    stopCar();
    delay(500);
    findSafeDirection();
  } else {
    moveForward();
  }
}

void manualMode(char command) {
  switch (command) {
    case 'F':
      moveForward();
      break;
    case 'B':
      moveBackward();
      break;
    case 'L':
      turnLeft();
      break;
    case 'R':
      turnRight();
      break;
    case 'S':
      stopCar();
      break;
  }
}

void setup() {
  // Initialize pins
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  pinMode(LEFT_MOTOR_F, OUTPUT);
  pinMode(LEFT_MOTOR_B, OUTPUT);
  pinMode(RIGHT_MOTOR_F, OUTPUT);
  pinMode(RIGHT_MOTOR_B, OUTPUT);

  // Initialize Bluetooth
  BT.begin(9600);
  Serial.begin(9600);

  // Initialize servo
  servoMotor.attach(SERVO_PIN);
  servoMotor.write(90); // Default position
}

void loop() {
  if (BT.available()) {
    char command = BT.read();

    if (command == 'A') {
      isAutonomous = true; // Switch to autonomous mode
    } else {
      isAutonomous = false; // Switch to manual mode
      manualMode(command); // Execute manual command
    }
  }

  if (isAutonomous) {
    autonomousMode(); // Run autonomous behavior
  }
}