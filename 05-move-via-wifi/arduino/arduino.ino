#include <avr/wdt.h>
#include "Driver.h"
#include "AppFunction.h"
#include "Timeout.h"

/* * * * * * * * * * * * * * * * * */
/* Variables for all Modes * * * * */
/* * * * * * * * * * * * * * * * * */
bool modeLoopedAtLeastOnce = false;

/* * * * * * * * * * * * * * * * * */
/* Variables for Free Mode * * * * */
/* * * * * * * * * * * * * * * * * */
unsigned long startTime = 0;
const unsigned int delayForShortMovement = 500;  // 500 ms for spinning left or right
const unsigned int delayForLongMovement = 750;   // 700 ms for forward and backward
bool countingDown = false;
uint8_t lastButtonPressed;
Timeout beginningOfMovement;

/* * * * * * * * * * * * * * * * * */
/* Variables for Learn Modes * * * */
/* * * * * * * * * * * * * * * * * */
const int maxHistorySize = 50;
uint8_t buttonHistory[maxHistorySize];
uint8_t historyIndex = 0;
uint8_t buttonsMemorized;

/* * * * * * * * * * * * * * * * * */
/* Variables for Repeat Mode * * * */
/* * * * * * * * * * * * * * * * * */
bool isRepeating;
uint8_t repeatingButtonIndex;

/* * * * * * * * * * * * * * * * * */
/* List of modes available * * * * */
/* * * * * * * * * * * * * * * * * */
enum Modes
{ 
    modeFree = 6,   // (Button 1) Freedom of movement
    modeLearn,      // (Button 2) Freedom of movement - but the movements are stored in an array
    modeRepeat,     // (Button 3) The movements are repeated from the array
};

uint8_t carMode = modeFree;    // we begin in FREE MODE

/* * * * * * * * * * * * * * * * * */
/* Setup the Arduino app * * * * * */
/* * * * * * * * * * * * * * * * * */
void setup() {
  Serial.begin(9600);

  MyApp.Init();
  Serial.println(" > > > > > > > > > > > > > > > > > > > > >");
  Serial.println(" > > > > > > > > > > > > > > > > > > > > >");
  Serial.println(" > > > > ELEGOO CAR  > > > > > > >");
  Serial.println(" > > > > > > > > > > > > > > > > > > > > >");
  Serial.println(" > > > > > > > > > > > > > > > > > > > > >");

  Serial.println("The car will start in Free Mode.");
}

/* * * * * * * * * * * * * * * * * */
/* * * * * Main Loop * * * * * * * */
/* * * * * * * * * * * * * * * * * */

void loop() {  
  switch (carMode) {
    case modeFree:
      loopModeFree();
      break;
    case modeLearn:
      loopModeLearn();
      break;
    case modeRepeat:
      loopModeRepeat();
      break;
  }  
}


/* * * * * * * * * * * * * * * * * * */
/* Loop for Mode 1: Free movement  * */
/* * * * * * * * * * * * * * * * * * */
void loopModeFree() {
  unsigned long currentTime = millis();

  // read commands as long as we are not waiting for a movement to finish
  if (!countingDown) {
    lastButtonPressed = MyApp.ReceiveCommandFromController(); 
    processLastButtonPressed(); 
  }

  // check delay time to see if a movement needs to be stopped
  stopWhenDelayOver();
}

/* * * * * * * * * * * * * * * * * * * */
/* Loop for Mode 1: Learn movements  * */
/* * * * * * * * * * * * * * * * * * * */
void loopModeLearn() {
  if (!modeLoopedAtLeastOnce) {
    // This is the first loop. Reset the history
    modeLoopedAtLeastOnce = true;
    buttonsMemorized = 0;
    historyIndex = 0;
  }

  unsigned long currentTime = millis();

  // We are not waiting for a movement
  if (!countingDown) {
    lastButtonPressed = MyApp.ReceiveCommandFromController();

    if (isModeChange(lastButtonPressed)) {
      MyApp.StopTheCar();
      carMode = lastButtonPressed;
      modeLoopedAtLeastOnce = false;
      return;
    } else if (isValidMovement(lastButtonPressed)) {
      startCountdown();
      buttonsMemorized++;

      // store the movement in the button history, if there is enough space
      if (historyIndex < maxHistorySize) {
        buttonHistory[historyIndex] = lastButtonPressed;
        historyIndex++;
      } else {
        Serial.println("You used all the memory for buttons. Go to Repeat mode now.");
      }
    }
  }

  stopWhenDelayOver();
}

/* * * * * * * * * * * * * * * * * * * */
/* Loop for Mode 2: Repeat movement  * */
/* * * * * * * * * * * * * * * * * * * */
void loopModeRepeat() {
  uint8_t mov;
  unsigned int delay;

  // Allow reading a new button command
  lastButtonPressed = MyApp.ReceiveCommandFromController(); 

  // if during the Repeat, the mode changes:
  if (isModeChange(lastButtonPressed)) {
    MyApp.StopTheCar();
    carMode = lastButtonPressed;
    isRepeating = false;
    modeLoopedAtLeastOnce = false;

    return;
  }

  // the first loop of the repetition
  if (!isRepeating && !modeLoopedAtLeastOnce) {
      isRepeating = true;
      repeatingButtonIndex = 0;
      mov = buttonHistory[repeatingButtonIndex];
      beginningOfMovement.Reset();
      MyApp.MoveTheCar(mov);

      return;
  }
  
  // regular repeat loop
  mov = buttonHistory[repeatingButtonIndex];

  if (isRepeating) {
    // based on the button, check its timeout
    switch(mov) {
      case btnForward:
      case btnBackward:
        delay = delayForLongMovement;
        break;
      case btnLeft:
      case btnRight:
        delay = delayForShortMovement;
        break;
      default:
        Serial.print("movement not detected: ");
        Serial.println(mov);
    }

    // still have to wait a little bit more
    if (!beginningOfMovement.HasElapsed(delay)) {
      return;
    }

    // no more delay for now, have I finished? 
    if (repeatingButtonIndex >= buttonsMemorized - 1) {
      isRepeating = false;
      repeatingButtonIndex = 0;
      mov = buttonHistory[repeatingButtonIndex];
      modeLoopedAtLeastOnce = true;
      MyApp.StopTheCar();

      return;
    }

    // I have not finished, more buttons to repeat
    repeatingButtonIndex++;
    mov = buttonHistory[repeatingButtonIndex];
    MyApp.MoveTheCar(mov);
    beginningOfMovement.Reset();
      
    // historyIndex = 0; This would enable printing only once
    // modeLoopedAtLeastOnce = true;
  }
}


/* * * * * * * * * * * * * * * * * * * * * */
/* read the last button pressed        * * */
/* * * * * * * * * * * * * * * * * * * * * */
void processLastButtonPressed() {
  if (isModeChange(lastButtonPressed)) {
    MyApp.StopTheCar();
    carMode = lastButtonPressed;
    modeLoopedAtLeastOnce = false;
    return;
  } else if (isValidMovement(lastButtonPressed)) {
    startCountdown();
  } else if (moveFromSerial()) {
    startCountdown();
  }
}


/* * * * * * * * * * * * * * * * * * * * * */
/* stop the robot after delay is over  * * */
/* * * * * * * * * * * * * * * * * * * * * */
void stopWhenDelayOver() {
  switch (lastButtonPressed) {
    case btnForward: 
    case btnBackward:
      if (beginningOfMovement.HasElapsed(delayForLongMovement)) {
        // stop the car
        stopCountdown();
      }
      break;
    case btnLeft:
    case btnRight:
      if (beginningOfMovement.HasElapsed(delayForShortMovement)) {
        // stop the car
        stopCountdown();
      }
      break;
    default:
      countingDown = false;
      break;
  }  
}

/* * * * * * * * * * * * * * * * * * * * * */
/* read movement from Serial port      * * */
/* * * * * * * * * * * * * * * * * * * * * */
bool moveFromSerial() {
  String movement = recvLine();
  movement.trim();
  movement.replace("\n", "");  // Remove newline character if present

  if (movement.length() == 0) {
    return false;
  }

  if (movement == "forward") {
    lastButtonPressed = btnForward;
  } else if (movement == "backward") {
    lastButtonPressed = btnBackward;
  } else if (movement == "left") {
    lastButtonPressed = btnLeft; 
  } else if (movement == "right") {
    lastButtonPressed = btnRight; 
  } else {
    Serial.print("No movement matches: ");
    Serial.println(movement);
    
    return false;
  }

  // Serial.println("Moving vehicle from serial");
  MyApp.MoveTheCar(lastButtonPressed);
  return true;
}


/* * * * * * * * * * * * * * * * * * * * * */
/* Check whether the movement is valid * * */
/* * * * * * * * * * * * * * * * * * * * * */
bool isValidMovement(uint8_t buttonPressed) {
  switch (buttonPressed) {
    case btnForward:
    case btnBackward:
    case btnLeft:
    case btnRight:
    case btnStop:
      return true;
  }

  return false;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/* Check whether the button pressed is a change mode button  * * */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
bool isModeChange(uint8_t buttonPressed) {
  switch (buttonPressed) {
    case btnModeFree:
    case bntModeLearn:
    case btnModeRepeat:
      return true;
  }

  return false;
}

/* * * * * * * * * * * * * * * * * * * * * * */
/* Receive a line from the Serial port * * * */
/* * * * * * * * * * * * * * * * * * * * * * */
String recvLine() {
  String receivedString;
    if (Serial.available() > 0) {
      receivedString = Serial.readStringUntil('\n');
    }
  return receivedString;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * */
/* Start the countdown for movement to stop  * * * */
/* * * * * * * * * * * * * * * * * * * * * * * * * */
void startCountdown(void) {
  startTime = millis();
  countingDown = true;
  beginningOfMovement.Reset();
}

/* * * * * * * * * * * * * * * * * * * * * * */
/* Stop the countdown and the vehicle  * * * */
/* * * * * * * * * * * * * * * * * * * * * * */
void stopCountdown(void) {
  countingDown = false;
  lastButtonPressed = btnUnknown;
  MyApp.StopTheCar();
}