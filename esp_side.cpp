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
      leds[0] = CRGB::Blue; FastLED.show(); delay(250);
      leds[0] = CRGB::Black; FastLED.show(); delay(500);
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
}