#include <Arduino.h>
#include <FastLED.h>

#define LED_PIN     48
#define NUM_LEDS    1
CRGB leds[NUM_LEDS];

const char* secretKey = "moji@air"; 

// 1. Define the possible states (Removed LOCKED)
enum DongleState {
  IDLE,
  AUTHENTICATING
};

DongleState currentState = IDLE;

// Variables to track time without pausing the processor
unsigned long lastFlashTime = 0;
bool isBlueOn = false;

void setup() {
  Serial.begin(115200);
  FastLED.addLeds<WS2812, LED_PIN, GRB>(leds, NUM_LEDS);
  FastLED.setBrightness(50);
  FastLED.clear();
  FastLED.show();
}

void loop() {
  // --- PART A: LISTENING TO THE PC ---
  if (Serial.available() > 0) {
    char incomingByte = Serial.read();

    if (incomingByte == 'R' && currentState == IDLE) {
      currentState = AUTHENTICATING;
      Serial.print(secretKey);
    } 
    else if (incomingByte == 'S' && currentState == AUTHENTICATING) { 
      // Success: Flash Green 3 times
      for(int i = 0; i < 3; i++) {
        leds[0] = CRGB::Green; FastLED.show(); delay(300);
        leds[0] = CRGB::Black; FastLED.show(); delay(300);
      }
      currentState = IDLE; // Reset so the dongle can be used again
    }
    else if (incomingByte == 'F' && currentState == AUTHENTICATING) { 
      // Fail: Flash Red 3 times instead of locking down
      for(int i = 0; i < 3; i++) {
        leds[0] = CRGB::Red; FastLED.show(); delay(300);
        leds[0] = CRGB::Black; FastLED.show(); delay(300);
      }
      currentState = IDLE; // Immediately reset to listen for the next attempt
    }
  }

  // --- PART B: DRIVING THE HARDWARE (Non-Blocking) ---
  if (currentState == AUTHENTICATING) {
    // Check if 500ms have passed since the last toggle
    if (millis() - lastFlashTime >= 500) {
      lastFlashTime = millis();    // Reset the stopwatch
      isBlueOn = !isBlueOn;        // Flip the state
      
      if (isBlueOn) {
        leds[0] = CRGB::Blue;
      } else {
        leds[0] = CRGB::Black;
      }
      FastLED.show();
    }
  } 
}