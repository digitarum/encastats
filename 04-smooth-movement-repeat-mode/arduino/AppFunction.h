//
// Created by Long  on 18/11/2023.
//

#ifndef _APP_FUNCTION_H
#define _APP_FUNCTION_H
#include <arduino.h>

enum ButtonCommands
{ 
    btnForward = 1,    //(1)
    btnBackward,       //(2)
    btnLeft,           //(3)
    btnRight,          //(4)
    btnStop,           //(5)
    btnModeFree,       //(6)
    bntModeLearn,      //(7)
    btnModeRepeat,     //(8)
    Mode4,             //(9)
    btnUnknown = 100
};

class AppFunction {
    public:
        void Init(void);
        uint8_t ReceiveCommandFromController(void);
        void MoveTheCar(uint8_t movement);
        void StopTheCar(void);
};

extern AppFunction MyApp;
#endif //_APP_FUNCTION_H
