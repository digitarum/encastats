//
// Created by Long  on 18/11/2023.
//

#ifndef _APP_FUNCTION_H
#define _APP_FUNCTION_H
#include <arduino.h>

class AppFunction {
    public:
        void Init(void);
        void Receive(void);
};

extern AppFunction MyApp;
#endif //_APP_FUNCTION_H
