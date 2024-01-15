This is an automatic translation, may be incorrect in some places. See sources and examples!

## WARNING, THE LIBRARY IS OUT OF DATE!
### USE THE [EncButton] LIBRARY(https://github.com/GyverLibs/EncButton)
It is much lighter, has more features and uses less CPU time!

```






```

# GyverButton
Library for multifunctional button press processing
- Operation with normally closed and normally open pushbuttons
- Working with connection PULL_UP and PULL_DOWN
- Polling a button with software anti-bounce contacts (adjustable time)
- Working out pressing, holding, releasing, clicking on the button (+ setting timeouts)
- Working out single, double and triple pressing (made separately)
- Working off any number of button presses (the function returns the number of clicks)
- The function of changing the value of a variable with a given step and a given time interval
- Ability to work with "virtual" buttons (all library features are used for matrix and resistive keyboards)

### Compatibility
Compatible with all Arduino platforms (using Arduino functions)

### Documentation
The library has [extended documentation](https://alexgyver.ru/GyverButton/)

## Content
- [Install](#install)
- [Initialization](#init)
- [Usage](#usage)
- [Example](#example)
- [Versions](#versions)
- [Bugs and feedback](#feedback)

<a id="install"></a>
## Installation
- The library can be found by the name **GyverButton** and installed through the menuzher libraries in:
    - Arduino IDE
    - Arduino IDE v2
    - PlatformIO
- [Download Library](https://github.com/GyverLibs/GyverButton/archive/refs/heads/main.zip) .zip archive for manual installation:
    - Unzip and put in *C:\Program Files (x86)\Arduino\libraries* (Windows x64)
    - Unzip and put in *C:\Program Files\Arduino\libraries* (Windows x32)
    - Unpack and put in *Documents/Arduino/libraries/*
    - (Arduino IDE) automatic installation from .zip: *Sketch/Include library/Add .ZIP library…* and specify the downloaded archive
- Read more detailed instructions for installing libraries [here] (https://alexgyver.ru/arduino-first/#%D0%A3%D1%81%D1%82%D0%B0%D0%BD%D0%BE% D0%B2%D0%BA%D0%B0_%D0%B1%D0%B8%D0%B1%D0%BB%D0%B8%D0%BE%D1%82%D0%B5%D0%BA)

<a id="init"></a>
## Initialization
```cpp
GButton btn; // without binding to a pin (virtual button) and without specifying the type (by default HIGH_PULL and NORM_OPEN)
GButton btn(pin); // with pin binding and without type specification (by default HIGH_PULL and NORM_OPEN)
GButton btn(pin, connection type); // with pin binding and connection type indication (HIGH_PULL / LOW_PULL) and without button type indication (NORM_OPEN by default)
GButton btn(pin, connection type, button type); // with pin binding and connection type (HIGH_PULL / LOW_PULL) and button type (NORM_OPEN / NORM_CLOSE)
GButton btn(BTN_NO_PIN, connection type, button type); // without binding to a pin and specifying the connection type (HIGH_PULL / LOW_PULL) and button type (NORM_OPEN / NORM_CLOSE)
```

<a id="usage"></a>
## Usage
```cpp
void setDebounce(uint16_t debounce); // set debounce time (default 80ms)
void setTimeout(uint16_ttimeout); // set hold timeout (default 300ms)
void setClickTimeout(uint16_t timeout); // set timeout between clicks (default 500ms)
void setStepTimeout(uint16_t step_timeout); // set timeout between increments (default400ms)
void setType(uint8_ttype); // setting the button type (HIGH_PULL - pulled up to power, LOW_PULL - to gnd)
void setDirection(uint8_t dir); // set direction (open/closed by default - NORM_OPEN, NORM_CLOSE)
  
void setTickMode(uint8_t tickMode); // (MANUAL / AUTO) manual or automatic polling of the button with the tick() function
// MANUAL - you need to call the tick() function manually
// AUTO - tick() is included in all other functions and polled itself
  
void tick(); // button poll
void tick(boolean state); // polling external value (0 pressed, 1 not pressed) (for matrix, resistive keyboards and joysticks)
  
boolean isPress(); // returns true when the button is clicked. Reset after call
boolean isRelease(); // returns true when the button is released. Reset after call
boolean isClick(); // returns true on click. Reset after call
boolean isHolded(); // returns true if held longer than timeout. Reset after call
boolean isHold(); // returns true when the button is pressed, does not reset
boolean state(); // returns the state of the button
boolean isSingle(); // returns true on single click. Reset after call
boolean isDouble(); // returns true on double click. Reset after call
boolean isTriple(); // returns true on triple click. Reset after call
  
boolean hasClick(); // check for clicks. Reset after call
uint8_t getClicks(); // return the number of clicks
uint8_t getHoldClicks();// return the number of clicks before holding
  
boolean isStep(); // returns true on timer setStepTimeout, see example
void resetStates(); // resets all is-flags and counters
```

<a id="example"></a>
## Example
See **examples** for other examples!
```cpp
// An example of using the GyverButton library, all features in one sketch.

#define BTN_PIN 3 // button connected here (BTN_PIN --- BUTTON --- GND)

#include "GyverButton.h"
GButton butt1(BTN_PIN);

// Initialization options:
// GButton btn; // without binding to a pin (virtual button) and without specifying the type (by default HIGH_PULL and NORM_OPEN)
// GButton btn(pin); // with pin binding and without type specification (by default HIGH_PULL and NORM_OPEN)
// GButton btn(pin, connection type); // with pin binding and connection type indication (HIGH_PULL / LOW_PULL) and without button type indication (NORM_OPEN by default)
// GButton btn(pin, connection type, button type); // with pin binding and connection type (HIGH_PULL / LOW_PULL) and button type (NORM_OPEN / NORM_CLOSE)
// GButton btn(BTN_NO_BTN_PIN, connection type, button type); // without binding to a pin and specifying the connection type (HIGH_PULL / LOW_PULL) and button type (NORM_OPEN / NORM_CLOSE)

int value = 0;

void setup() {
  Serial.begin(9600);

  butt1.setDebounce(50); // debounce setting (default 80ms)
  butt1.setTimeout(300); // set hold timeout (default 500ms)
  butt1.setClickTimeout(600); // set timeout between clicks (default 300ms)

  // HIGH_PULL - button is connected to GND, pin is pulled up to VCC (BTN_PIN --- BUTTON --- GND)
  // LOW_PULL - button is connected to VCC, pin is pulled to GND
  // default is HIGH_PULL
  butt1.setType(HIGH_PULL);

  // NORM_OPEN - normally open button
  // NORM_CLOSE - normally closed button
  // default is NORM_OPEN
  butt1.setDirection(NORM_OPEN);
}

void loop() {
  butt1.tick(); // mandatory processing function. Must be constantly asked

  if (butt1.isClick()) Serial.println("Click"); // check for one click
  if (butt1.isSingle()) Serial.println("Single"); // check for one click
  if (butt1.isDouble()) Serial.println("Double"); // check for double click
  if (butt1.isTriple()) Serial.println("Triple"); // check for triple click

  if (butt1.hasClicks())// check for clicks
    Serial.println(butt1.getClicks()); // get (and display) the number of clicks

  if (butt1.isPress()) Serial.println("Press"); // button click (+ debounce)
  if (butt1.isRelease()) Serial.println("Release"); // button release (+ debounce)
  if (butt1.isHold()) { // if the button is held down
    Serial print("Holding"); // output while held
    Serial.println(butt1.getHoldClicks()); // you can display the number of clicks before holding!
  }
  if (butt1.isHold()) Serial.println("Holding"); // check for hold
  //if (butt1.state()) Serial.println("Hold"); // returns the state of the button

  if (butt1.isStep()) { // if the button was held down (this is for incrementing)
    value++; // increase/decrease value variable with step and interval
    Serial println(value); // for example, output to the port
  }
}
```

<a id="versions"></a>
## Versions
- v2.15: Added the ability to declare a button without linking to a pin
- v3.0: Accelerated and optimized code, redone initialization, added examples
- v3.1: isStep can take the number of clicks made before it (see clicks_step example)
- v3.2: Added getHoldClicks() method - return number of clicks prior to holding
- v3.3: Minor fixes
- v3.4: Added resetStates() method to reset states and counters
- v3.5: increased performance for AVR Arduino
- v3.6: added separate class for working with analog keyboards, see example analogKeyboardG
- v3.7: Dryundel fixes:
    - Any hold timeout
    - Single, Double and Triple now don't interfere with hasClicks and getClicks and work together
    - isStep() also now does not interfere with anything and it works more correctly
- v3.8: Dryundel fixes

<a id="feedback"></a>
## Bugs and feedback
Create **I when finding bugsssue**, or better yet, write to [alex@alexgyver.ru](mailto:alex@alexgyver.ru)
The library is open for revision and your **Pull Request**'s!