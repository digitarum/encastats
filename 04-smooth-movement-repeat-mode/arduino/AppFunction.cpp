//
// Created by Long  on 18/11/2023.
//

#include <hardwareSerial.h>
#include <stdio.h>
#include <string.h>
#include "AppFunction.h"
#include "Driver.h"
#include "Motor.h"

#include <HardwareSerial.h>

#define _is_print 1
#define _Test_print 0

Driver MyDriver;

Motor AppMotor;
uint8_t speed = 60;


/* * * * * * * * * * * * * * * * * */
/* Setup the car app * * * * * * * */
/* * * * * * * * * * * * * * * * * */
void AppFunction::Init(void) {
    bool res_error = true;
    Serial.begin(9600);

    MyDriver.Init();
}

/* * * * * * * * * * * * * * * * * * * * * * * * * */
/* Move the car in a given direction * * * * * * * */
/* * * * * * * * * * * * * * * * * * * * * * * * * */
void AppFunction::MoveTheCar(uint8_t movement){
    switch (movement) {
        case btnForward:
            AppMotor.GoForward(speed);
            break;
        case btnBackward:
            AppMotor.GoBackward(speed);
            break;
        case btnLeft:
            AppMotor.TurnLeft(speed);
            break;
        case btnRight:
            AppMotor.TurnRight(speed);
            break;
        case btnStop:
            AppMotor.Stop();
            break;
        case btnModeFree:
            Serial.println("Mode 1");
            break;
        case bntModeLearn:
            Serial.println("Mode 2");
            break;
        case btnModeRepeat:
            Serial.println("Mode 3");
            break;
        case Mode4:
            Serial.println("Mode 4");
            break;
        case 10:
            Serial.println("Mode 5");
            break;
    }
}

/* * * * * * * * * * * * * * * * * * * * * * * * * */
/* Read a button from the controller * * * * * * * */
/* * * * * * * * * * * * * * * * * * * * * * * * * */
uint8_t AppFunction::ReceiveCommandFromController(void) {
    uint8_t Button_pressed = 100;

    static bool data_received = false;

    if (MyDriver.Get(&Button_pressed)) {
        data_received = true;
        // Serial.println(Button_pressed);
    }

    if (true == data_received) {
        MoveTheCar(Button_pressed);
    }

    return Button_pressed;
}

/* * * * * * * * * * * * * * * */
/* Stop the car  * * * * * * * */
/* * * * * * * * * * * * * * * */
void AppFunction::StopTheCar(void) {
    AppMotor.Stop();
}
