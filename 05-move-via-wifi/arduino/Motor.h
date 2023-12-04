//
// Created by Long  on 18/11/2023.
//

#ifndef MOTOR_H
#define MOTOR_H
#include <arduino.h>

class Motor {
    public:
        void Init(void);
        void GoForward(uint8_t speed);
        void GoBackward(uint8_t speed);
        void TurnLeft(uint8_t speed);
        void TurnRight(uint8_t speed);
        void Stop();

    private:
#define PIN_Speed_Motor_A 5
#define PIN_Speed_Motor_B 6
#define PIN_Input_Motor_A 8
#define PIN_Input_Motor_B 7
#define PIN_Motor_STBY 3

        public:
#define speed_Max 100
#define direction_forward true
#define direction_backward false
#define direction_stop 3

#define Duration_enable true
#define Duration_disable false
#define control_enable true
#define control_disable false
};

#endif //MOTOR_H
