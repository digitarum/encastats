//
// Created by Long  on 18/11/2023.
//
#include "Driver.h"

// create the object to receive infrared
IRrecv receiver(RECEIVER_PIN);

// Initialize the receiver, enable the input of IR data
void Driver::Init(void) {
  receiver.enableIRIn();
}

// Read an unsigned integer from the receiver
bool Driver::Get(uint8_t *Got) {
  decode_results input;

  if (receiver.decode(&input)) {
    // I got a button press. Save the time: 
    IR_milliseconds = millis();
    
    // which button was pressed? 
    switch (input.value) {
      case BUTTON_UP_A:
        *Got = 1;
        break;
      case BUTTON_DOWN_A:
        *Got = 2;
        break;
      case BUTTON_LEFT_A:
        *Got = 3;
        break;
      case BUTTON_RIGHT_A:
        *Got = 4;
        break;
      case BUTTON_OK_A:
        *Got = 5;
        break;
      case BUTTON_1_A:
        *Got = 6;
        break;
      case BUTTON_2_A:
        *Got = 7;
        break;
      case BUTTON_3_A:
        *Got = 8;
        break;
      case BUTTON_4_A:
        *Got = 9;
        break;
      case BUTTON_5_A:
        *Got = 10;
        break;
      case BUTTON_6_A:
      // case BUTTON_6_B:
        *Got = 11;
        break;
      default:
        // todo: send via Wifi to the telegram bot
        // Serial.println("Unknown code received");
        // Serial.println(input.value);
        receiver.resume();
        return false;
    }

    receiver.resume();
    return true;
  } else {
    // Serial.println("Error decoding remote results");
    return false;
  }
}
