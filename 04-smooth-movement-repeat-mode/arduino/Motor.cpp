//
// Created by Long  on 18/11/2023.
//
#include "Motor.h"

void Motor::Init(void) {
  // prepare the pins for output
  pinMode(PIN_Speed_Motor_A, OUTPUT);
  pinMode(PIN_Speed_Motor_B, OUTPUT);
  pinMode(PIN_Input_Motor_A, OUTPUT);
  pinMode(PIN_Input_Motor_B, OUTPUT);
  pinMode(PIN_Motor_STBY, OUTPUT);
}

void Motor::GoForward(uint8_t speed) {
  Serial.println("motor forward");
  digitalWrite(PIN_Motor_STBY, HIGH);

  // right side
  digitalWrite(PIN_Input_Motor_A, HIGH);
  analogWrite(PIN_Speed_Motor_A, speed);
  // left side
  digitalWrite(PIN_Input_Motor_B, HIGH);
  analogWrite(PIN_Speed_Motor_B, speed);
}

void Motor::GoBackward(uint8_t speed) {
  Serial.println("motor backward");
  digitalWrite(PIN_Motor_STBY, HIGH);

  // right side
  digitalWrite(PIN_Input_Motor_A, LOW);
  analogWrite(PIN_Speed_Motor_A, speed);
  // left side
  digitalWrite(PIN_Input_Motor_B, LOW);
  analogWrite(PIN_Speed_Motor_B, speed);
}

void Motor::TurnLeft(uint8_t speed) {
  digitalWrite(PIN_Motor_STBY, HIGH);

  // right side
  digitalWrite(PIN_Input_Motor_A, LOW);
  analogWrite(PIN_Speed_Motor_A, speed);
  // left side
  digitalWrite(PIN_Input_Motor_B, HIGH);
  analogWrite(PIN_Speed_Motor_B, speed);
}

void Motor::TurnRight(uint8_t speed) {
  digitalWrite(PIN_Motor_STBY, HIGH);

  // right side
  digitalWrite(PIN_Input_Motor_A, HIGH);
  analogWrite(PIN_Speed_Motor_A, speed);
  // left side
  digitalWrite(PIN_Input_Motor_B, LOW);
  analogWrite(PIN_Speed_Motor_B, speed);
}


void Motor::Stop() {
  digitalWrite(PIN_Motor_STBY, LOW);

  // right side
  digitalWrite(PIN_Input_Motor_A, LOW);
  analogWrite(PIN_Speed_Motor_A, 0);
  // left side
  digitalWrite(PIN_Input_Motor_B, LOW);
  analogWrite(PIN_Speed_Motor_B, 0);
}


