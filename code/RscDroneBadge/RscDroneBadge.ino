/**
 * Code for RSC DRONE BADGE
 * MCU: ATTINY85 @ 8 MHz
 * Using: ATTinyCore
 *
 * TODO: Cleanup. The code is a little bit ugly, but it works somehow.
 */

#include <avr/interrupt.h>
#include <avr/sleep.h>


#define LEDS 4
uint8_t ledPins[LEDS] = { 0, 1, 3, 2 };
uint8_t ledPinsFast[LEDS] = { PB0, PB1, PB3, PB2 };

#define SET_STATE_HIGH(pin_name) (PORTB |= (1 << pin_name))
#define SET_STATE_LOW(pin_name) (PORTB &= ~(1 << pin_name))
#define SET_STATE(pin_name, state) ((state) ? SET_STATE_HIGH(pin_name) : SET_STATE_LOW(pin_name))

#define ON HIGH
#define OFF LOW

#define PIN_BUTTON 4
#define BUTTON_PRESSED LOW

#define INTERVAL_NORMAL 250
#define INTERVAL_FAST 100

/*
 * Animations
 */
byte animationType = 0;                // current animation
unsigned int animationInterval = 250;  // interval between animation frames
byte animationFrame = 0;               // current animation frame
unsigned long lastTime = 0;            // last time in milliseconds the frame changed
byte animationEnabled = true;          // whether animations enabled

/*
 * PWM - brightness settings
 */
#define PWM_MODES 4
byte currentPwmMode = 0;
byte pwmModes[PWM_MODES] = { 0, 10, 100, 250 };
byte pwmIndex = 0;


/*
 * Buttons
 */
bool buttonPressed = false;        // whether mode button pressed last time
unsigned long lastTimeButton = 0;  // last time - used for turning off
bool modeChangeDisabled = false;   // if changing pwm or turning on, mode change disabled when button released

/*
 * Turn off interval
 */
#define TURN_OFF_INTERVAL 1000


bool leds[LEDS] = { 0, 0, 0, 0 };

#define ANIMATIONS 8  // animations count

void animate() {

  animationInterval = INTERVAL_NORMAL;

  switch (animationType) {
    case 0:
      animationOn();
      break;
    case 1:
      animationAllBlink();
      break;
    case 2:
      animationRotateProper();
      break;
    case 3:
      animationInterval = INTERVAL_FAST;
      animationRotateProper();
      break;
    case 4:
      animationRotate();
      break;
    case 5:
      animationInterval = INTERVAL_FAST;
      animationRotate();
      break;
    case 6:
      animationRotateBackwards();
      break;
    case 7:
      animationInterval = INTERVAL_FAST;
      animationRotateBackwards();
      break;
  }
}

void animationOff() {
  for (uint8_t i = 0; i < LEDS; i++) {
    leds[i] = 0;
  }
}

void animationOn() {
  for (uint8_t i = 0; i < LEDS; i++) {
    leds[i] = 1;
  }
}

void animationAllBlink() {
  switch (animationFrame) {
    case 0:
      animationOn();
      animationFrame++;
      break;
    case 1:
      animationOff();
      animationFrame = 0;
      break;
  }
}

void animationRotate() {
  animationFrame = (animationFrame + 1) % LEDS;
  animationOff();
  leds[animationFrame] = 1;
}

void animationRotateBackwards() {
  animationFrame = (animationFrame + 1) % LEDS;
  animationOff();
  leds[LEDS - animationFrame - 1] = 1;
}

void animationRotateProper() {
  animationFrame = (animationFrame + 1) % 2;
  animationOff();
  leds[animationFrame] = 1;
  leds[animationFrame + 2] = 1;
}

void animationRandom() {
  for (uint8_t i = 0; i < LEDS; i++) {
    leds[i] = (random(10) > 7);
  }
}


/* 
 *  The setup function runs once when the battery is inserted (not when "turned off" via button)
 */
void setup() {
  for (uint8_t i = 0; i < LEDS; i++) {
    pinMode(ledPins[i], OUTPUT);
  }
  pinMode(PIN_BUTTON, INPUT_PULLUP);
}


/* 
 * The loop function runs over and over again forever
 */
void loop() {

  /*
     * BUTTON MODE switches animations, but after is released (because of PWM and turning off)
     */
  if (digitalRead(PIN_BUTTON) == BUTTON_PRESSED) {
    if (!buttonPressed) {
      buttonPressed = true;
      lastTimeButton = millis();
      delay(100);  // little bit of debouncing here and there
    }
    checkWhetherToSleep();  // checking whether to sleep
  } else {

    // if mode released, change mode
    if (buttonPressed) {
      if (!modeChangeDisabled) {  // changing modes is disabled after PWM setting and waking up
        lastTime = 0;
        animationType = (animationType + 1) % ANIMATIONS;
        if (animationType == 0) {
          currentPwmMode = (currentPwmMode + 1) % PWM_MODES;
        }
        animationFrame = 0;
      }
      modeChangeDisabled = false;
    }
    buttonPressed = false;
    lastTimeButton = 0;
  }

  /*
     * Doing animation and PWM!
     */
  if (animationEnabled) {
    doAnimation();
    updateLeds();
  }
}

/*
 * Whether is time to do animation
 */
void doAnimation() {
  if (millis() - lastTime < animationInterval) {  // check whether is the time to do it
    return;
  }

  lastTime = millis();
  animate();
}

/*
 * Checks whether MODE has been pressed for too long, if so, go to sleep - then wake up
 */
void checkWhetherToSleep() {
  if (lastTimeButton == 0) return;

  if (millis() - lastTimeButton > TURN_OFF_INTERVAL) {
    animationOff();
    animationEnabled = false;
    updateLeds();
    delay(200);  // for person to have a little time to put their finger off

    // NOW GOING TO SLEEP
    sleep();
    // NOW WAKING UP

    lastTimeButton = 0;
    animationEnabled = true;
    modeChangeDisabled = true;
  }
}


/*
 * This is called when the interrupt occurs - button pressed, but we don't need to do anything in it
 */

ISR(PCINT0_vect) {
}


/* 
 *  Doing sleep
 *  
 *  The sleep is interrupted by even shortest pressing MODE,
 *  but then there is a check, wheter the button is pressed for longer time to prevent unwanted waking-ups
 *  
 */
void sleep() {
  GIMSK |= _BV(PCIE);                   // Enable Pin Change Interrupts
  PCMSK |= _BV(PCINT4);                 // Use PB4 as interrupt pin
  ADCSRA &= ~_BV(ADEN);                 // ADC off
  set_sleep_mode(SLEEP_MODE_PWR_DOWN);  // replaces above statement
  sleep_enable();                       // Sets the Sleep Enable bit in the MCUCR Register (SE BIT)
  sei();                                // Enable interrupts
  sleep_cpu();                          // sleep

  // NOW WAKING UP
  cli();                  // Disable interrupts
  PCMSK &= ~_BV(PCINT4);  // Turn off PB4 as interrupt pin
  sleep_disable();        // Clear SE bit
  ADCSRA |= _BV(ADEN);    // ADC on
  sei();                  // Enable interrupts

  // CHECK WHETHER THE BUTTON IS PRESSED FOR 1s, if not, sleep again
  bool slp = false;
  for (byte i = 0; i < 4; i++) {
    delay(250);
    if (digitalRead(PIN_BUTTON) != BUTTON_PRESSED) {
      slp = true;
      break;
    }
  }
  if (slp) {
    sleep();
  }
}


void updateLeds() {
  if (pwmIndex > pwmModes[currentPwmMode]) {
    pwmIndex = 0;
  } else {
    pwmIndex++;
  }
  if (pwmIndex == 0) {
    for (uint8_t i = 0; i < LEDS; i++) {
      SET_STATE(ledPinsFast[i], leds[i]);
      //digitalWrite(ledPins[i], leds[i]);
    }

  } else {
    for (uint8_t i = 0; i < LEDS; i++) {
      SET_STATE_LOW(ledPinsFast[i]);
      //digitalWrite(ledPins[i], false);
    }
  }
}
