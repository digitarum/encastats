//
// Created by Long  on 18/11/2023.
//
#ifndef _Driver_H_
#define _Driver_H_

#define Test_Driver 0
#include <arduino.h>
#include <IRremote.h>

class Driver {
  public: 
    void Init(void);
    bool Get(uint8_t *Got);
  
  public:
    unsigned long IR_milliseconds;
  
  private:
    #define RECEIVER_PIN 9

  // Define the word for each button
  #define BUTTON_UP_A 16736925
  #define BUTTON_DOWN_A 16754775
  #define BUTTON_LEFT_A 16720605
  #define BUTTON_RIGHT_A 16761405
  #define BUTTON_OK_A 16712445
  #define BUTTON_1_A 16738455
  #define BUTTON_2_A 16750695
  #define BUTTON_3_A 16756815
  #define BUTTON_4_A 16724175
  #define BUTTON_5_A 16718055
  #define BUTTON_6_A 16743045
  #define BUTTON_7_A 16716015
  #define BUTTON_8_A 16726215
  #define BUTTON_9_A 16734885

  // // Release events: not used
  // #define BUTTON_UP_B 5316027
  // #define BUTTON_DOWN_B 2747854299
  // #define BUTTON_LEFT_B 1386468383
  // #define BUTTON_RIGHT_B 553536955
  // #define BUTTON_OK_B 3622325019
  // #define BUTTON_1_B 3238126971
  // #define BUTTON_2_B 2538093563
  // #define BUTTON_3_B 4039382595
  // #define BUTTON_4_B 2534850111
  // #define BUTTON_5_B 1033561079
  // #define BUTTON_6_B 1635910171
  // #define BUTTON_7_B 2351064443
  // #define BUTTON_8_B 1217346747
  // #define BUTTON_9_B 71952287
};

#endif

