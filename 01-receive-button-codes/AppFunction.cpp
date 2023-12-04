//
// Created by Long  on 18/11/2023.
//

#include <hardwareSerial.h>
#include <stdio.h>
#include <string.h>
#include "AppFunction.h"
#include "Driver.h"
#include "Motor.h"

// #include "ArduinoJson-v6.11.1.h" //ArduinoJson

#define _is_print 1
#define _Test_print 0

Driver MyDriver;

// never used anymore
// enum CarModes {
//     Mode_1,
//     Mode_2,
//     Mode_3,
//     Mode_4,
//     Mode_5,
//     Mode_6,
// };

// struct Application {
//     CarModes Car_Mode;
// };

enum ButtonCommands
{ 
    Forward = 1,   //(1)
    Backward,      //(2)
    Left,          //(3)
    Right,         //(4)
    Stop,           //(9)
    LeftForward,   //(5)
    LeftBackward,  //(6)
    RightForward,  //(7)
    RightBackward, //(8)
};

// Application ElegooCar;
Motor AppMotor;
uint8_t speed = 60;

void AppFunction::Init(void) {
    bool res_error = true;
    Serial.begin(9600);
    MyDriver.Init();
}

void AppFunction::Receive(void) {
    uint8_t Button_pressed;
    static bool data_received = false;

    if (MyDriver.Get(&Button_pressed)) {
        data_received = true;
    }

    if (true == data_received) {
        switch (Button_pressed) {
            case Forward:
                Serial.println("Up");

                AppMotor.SetMotorControl(
                    direction_forward,
                    speed,
                    direction_forward,
                    speed,
                    control_enable
                );
                break;
            case Backward:
                Serial.println("Down");

                AppMotor.SetMotorControl(
                    direction_backward,
                    speed,
                    direction_backward,
                    speed,
                    control_enable
                );

                break;
            case Left:
                Serial.println("Left");

                AppMotor.SetMotorControl(
                    direction_backward,
                    speed,
                    direction_forward,
                    speed,
                    control_enable
                );

                break;
            case Right:
                Serial.println("Right");

                AppMotor.SetMotorControl(
                    direction_forward,
                    speed,
                    direction_backward,
                    speed,
                    control_enable
                );

                break;
            case Stop:
                Serial.println("Ok");

                AppMotor.SetMotorControl(
                    direction_stop,
                    0,
                    direction_stop,
                    0,
                    control_enable
                );

                break;
            case 6:
                Serial.println("Mode 1");
                break;
            case 7:
                Serial.println("Mode 2");
                break;
            case 8:
                Serial.println("Mode 3");
                break;
            case 9:
                Serial.println("Mode 4");
                break;
            case 10:
                Serial.println("Mode 5");
                break;
        }
    }
}




