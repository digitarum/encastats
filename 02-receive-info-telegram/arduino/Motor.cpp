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

void Motor::SetMotorControl(
            boolean direction_A,
            uint8_t speed_A,
            boolean direction_B,
            uint8_t speed_B,
            boolean controlED
) {
    if (controlED != control_enable) {
        // turn off motor
        digitalWrite(PIN_Motor_STBY, LOW);
        return;
    }

    digitalWrite(PIN_Motor_STBY, HIGH);

    // check the Right side
    switch(direction_A) {
        case direction_forward:
            digitalWrite(PIN_Input_Motor_A, HIGH);
            analogWrite(PIN_Speed_Motor_A, speed_A);
            break;
        case direction_backward:
            digitalWrite(PIN_Input_Motor_A, LOW);
            analogWrite(PIN_Speed_Motor_A, speed_A);
            break;
        case direction_stop:
            digitalWrite(PIN_Input_Motor_A, LOW);
            analogWrite(PIN_Speed_Motor_A, 0);
            break;
        default:
            digitalWrite(PIN_Input_Motor_A, LOW);
            analogWrite(PIN_Speed_Motor_A, 0);
    }

    // check the Left side
    switch(direction_B) {
        case direction_forward:
            digitalWrite(PIN_Input_Motor_B, HIGH);
            analogWrite(PIN_Speed_Motor_B, speed_B);
        break;
        case direction_backward:
            digitalWrite(PIN_Input_Motor_B, LOW);
            analogWrite(PIN_Speed_Motor_B, speed_B);
        break;
        case direction_stop:
            digitalWrite(PIN_Input_Motor_B, LOW);
            analogWrite(PIN_Speed_Motor_B, 0);
        break;
        default:
            digitalWrite(PIN_Input_Motor_B, LOW);
            analogWrite(PIN_Speed_Motor_B, 0);
    }

}
