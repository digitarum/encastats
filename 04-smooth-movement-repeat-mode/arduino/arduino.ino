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

void loopOld() {
  // read the serial stuff
  
  if (Serial.available()) {
    String receivedString = Serial.readStringUntil('\n');
    if (receivedString.length() > 0) {
      Serial.println("Received: " + receivedString);
    }
  }
  
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

  if (!countingDown) {
    lastButtonPressed = MyApp.ReceiveCommandFromController(); 

    if (isModeChange(lastButtonPressed)) {
      MyApp.StopTheCar();
      carMode = lastButtonPressed;
      modeLoopedAtLeastOnce = false;
      return;
    } else if (isValidMovement(lastButtonPressed)) {
      startTime = millis();
      countingDown = true;
      beginningOfMovement.Reset();
    }
  }

  switch (lastButtonPressed) {
      case btnForward: 
      case btnBackward:
        if (beginningOfMovement.HasElapsed(delayForLongMovement)) {
          // stop the car
          countingDown = false;
          lastButtonPressed = btnUnknown;
          MyApp.StopTheCar();
        }
        break;
      case btnLeft:
      case btnRight:
        if (beginningOfMovement.HasElapsed(delayForShortMovement)) {
          // stop the car
          countingDown = false;
          lastButtonPressed = btnUnknown;
          MyApp.StopTheCar();
        }
        break;
      default:
        countingDown = false;
        break;
  }  
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
      startTime = millis();
      countingDown = true;
      beginningOfMovement.Reset();
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

  // stop after a given timeout for each type of movement
  switch (lastButtonPressed) {
      case btnForward: 
      case btnBackward:
        if (beginningOfMovement.HasElapsed(delayForLongMovement)) {
          // stop the car
          countingDown = false;
          lastButtonPressed = btnUnknown;
          MyApp.StopTheCar();
        }
        break;
      case btnLeft:
      case btnRight:
        if (beginningOfMovement.HasElapsed(delayForShortMovement)) {
          // stop the car
          countingDown = false;
          lastButtonPressed = btnUnknown;
          MyApp.StopTheCar();
        }
        break;
      default:
        countingDown = false;
        break;
  } 
}

/* * * * * * * * * * * * * * * * * * * */
/* Loop for Mode 1: Repeat movement  * */
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

char receivedChar;
boolean newData = false;

// debug: Trying to read data from Serial port
void loop() {
  recvOneChar();
  showNewData();
}

void recvOneChar() {
    if (Serial.available() > 0) {
      String receivedString = Serial.readStringUntil('\n');
      if (receivedString.length() > 0) {
        Serial.println("Received: " + receivedString);
        // Process the received data as needed
      }
    }
}

void showNewData() {
    if (newData == true) {
        Serial.print("This just in ... ");
        Serial.println(receivedChar);
        newData = false;
    }
}