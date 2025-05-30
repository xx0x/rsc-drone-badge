/**
 * Code for RSC DRONE BADGE (Mini Variant with 4 LEDs)
 * MCU: ATTINY25 @ 8 MHz
 * Using: Arduino + ATTinyCore
 * Author: Vaclav M (github.com/xx0x)
 * Licence: MIT
 */

#include <avr/interrupt.h>
#include <avr/sleep.h>

// LED configuration
#define LEDS 4
const uint8_t ledPinsFast[LEDS] = {PB0, PB1, PB3, PB2};

#define SET_STATE_HIGH(pin_name) (PORTB |= (1 << pin_name))
#define SET_STATE_LOW(pin_name) (PORTB &= ~(1 << pin_name))
#define SET_STATE(pin_name, state) ((state) ? SET_STATE_HIGH(pin_name) : SET_STATE_LOW(pin_name))

#define PIN_BUTTON 4
#define BUTTON_PRESSED LOW

#define INTERVAL_NORMAL 250
#define INTERVAL_FAST 100
#define TURN_OFF_INTERVAL 1000

// Animations
uint8_t animationType = 0;          // current animation
uint8_t animationFrame = 0;         // current animation frame
uint8_t animationEnabled = 1;       // whether animations enabled
uint8_t animationInterval = INTERVAL_NORMAL; // interval between animation frames

// Button state tracking
uint8_t buttonPressed = 0;       
uint8_t modeChangeDisabled = 0;

// Timing variables
unsigned long lastTime = 0;
unsigned long lastTimeButton = 0;

// LED states
uint8_t leds[LEDS];

// Animation defines
#define ANIMATIONS 11
#define ANIMATION_RANDOM 10
uint8_t randomAnimation = 1;
uint8_t randomFramesCounter = 0;

// Forward declarations
void animate(void);
void updateLeds(void);
void sleep(void);
void animationOff(void);

void setup() {
  // Set LED pins as outputs
  DDRB |= ((1 << PB0) | (1 << PB1) | (1 << PB2) | (1 << PB3));
  
  // Set button pin as input with pullup
  pinMode(PIN_BUTTON, INPUT_PULLUP);
}

void loop() {
  // Debounce to prevent double taps
  if (millis() - lastTimeButton > 200) {
    if (digitalRead(PIN_BUTTON) == BUTTON_PRESSED) {
      if (!buttonPressed) {
        buttonPressed = 1;
        lastTimeButton = millis();
      }
      // Check whether to sleep
      if (lastTimeButton && (millis() - lastTimeButton > TURN_OFF_INTERVAL)) {
        animationOff();
        animationEnabled = 0;
        updateLeds();
        delay(200);
        sleep();
        lastTimeButton = 0;
        animationEnabled = 1;
        modeChangeDisabled = 1;
      }
    } else {
      lastTimeButton = 0;
      
      // If mode released, change mode
      if (buttonPressed) {
        if (!modeChangeDisabled) {
          lastTime = 0;
          animationType = (animationType + 1) % ANIMATIONS;
          animationFrame = 0;
        }
        modeChangeDisabled = 0;
        lastTimeButton = millis();
      }
      buttonPressed = 0;
    }
  }

  // Handle animation and update LEDs
  if (animationEnabled) {
    // Check if it's time for a new animation frame
    if (millis() - lastTime >= animationInterval) {
      lastTime = millis();
      animate();
    }
    updateLeds();
  }
}

// Empty interrupt handler
ISR(PCINT0_vect) {}

void sleep() {
  GIMSK |= _BV(PCIE);                  // Enable Pin Change Interrupts
  PCMSK |= _BV(PCINT4);                // Use PB4 as interrupt pin
  ADCSRA &= ~_BV(ADEN);                // ADC off
  set_sleep_mode(SLEEP_MODE_PWR_DOWN);
  sleep_enable();
  sei();
  sleep_cpu();

  // Wake up
  cli();
  PCMSK &= ~_BV(PCINT4);
  sleep_disable();
  ADCSRA |= _BV(ADEN);
  sei();

  // Check if button is still pressed for 1s
  for (uint8_t i = 0; i < 4; i++) {
    delay(250);
    if (digitalRead(PIN_BUTTON) != BUTTON_PRESSED) {
      sleep();
      break;
    }
  }
}

void animate() {
  animationInterval = INTERVAL_NORMAL;
  uint8_t animationTypeNow = animationType;
  
  // Handle random animation selection
  if (animationTypeNow == ANIMATION_RANDOM) {
    if (++randomFramesCounter > 8) {
      uint8_t x;
      do {
        x = (rand() % 7) + 1;
      } while (x == randomAnimation);
      randomAnimation = x;
      randomFramesCounter = 0;
      animationFrame = 0;
    }
    animationTypeNow = randomAnimation;
  }

  // Reset all LEDs first
  animationOff();

  // Set animation speed
  if (animationTypeNow == 3 || animationTypeNow == 5 || 
      animationTypeNow == 7 || animationTypeNow == 9) {
    animationInterval = INTERVAL_FAST;
    animationTypeNow--; // Use the same pattern as the slower version
  }

  // Handle different animation patterns
  switch (animationTypeNow) {
    case 0: // All on
      for (uint8_t i = 0; i < LEDS; i++) leds[i] = 1;
      break;
      
    case 1: // All blink
      animationFrame ^= 1; // Toggle between 0 and 1
      if (animationFrame) {
        for (uint8_t i = 0; i < LEDS; i++) leds[i] = 1;
      }
      break;
      
    case 2: // Left-right
      animationFrame = (animationFrame + 1) % 2;
      leds[animationFrame] = 1;
      leds[animationFrame + 2] = 1;
      break;
      
    case 4: // Up-down
      animationFrame = (animationFrame + 1) % 2;
      if (animationFrame == 0) {
        leds[0] = leds[3] = 1;
      } else {
        leds[1] = leds[2] = 1;
      }
      break;
      
    case 6: // Rotate
      animationFrame = (animationFrame + 1) % LEDS;
      leds[(animationFrame == 0) ? 0 : (animationFrame == 1) ? 2 : (animationFrame == 2) ? 3 : 1] = 1;
      break;
      
    case 8: // Rotate backwards
      animationFrame = (animationFrame + 1) % LEDS;
      leds[(animationFrame == 0) ? 1 : (animationFrame == 1) ? 3 : (animationFrame == 2) ? 2 : 0] = 1;
      break;
  }
}

void animationOff() {
  for (uint8_t i = 0; i < LEDS; i++) {
    leds[i] = 0;
  }
}

void updateLeds() {
  for (uint8_t i = 0; i < LEDS; i++) {
    SET_STATE(ledPinsFast[i], leds[i]);
  }
}
