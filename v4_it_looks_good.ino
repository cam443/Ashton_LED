#include <Adafruit_NeoPixel.h>

// Pin configurations
#define NEOPIXEL_PIN 2
#define BUTTON_PIN 8

// LED settings
#define NUM_PIXELS 6
#define MODE_COUNT 4
#define COLOR_CHANGE_THRESHOLD 50

// Timing settings
const uint32_t DEBOUNCE_TIME = 150;
const uint32_t UPDATE_INTERVAL = 3;
const uint8_t FADE_STEP = 1;

// Customization settings
const uint8_t MIN_BRIGHTNESS = 50;
const uint8_t MAX_BRIGHTNESS = 255;
const uint8_t MIN_BRIGHTNESS_CASE_3 = 0;  // Separate minimum brightness setting for case 3

Adafruit_NeoPixel strip = Adafruit_NeoPixel(NUM_PIXELS, NEOPIXEL_PIN, NEO_GRB + NEO_KHZ800);

uint8_t mode = 0;
uint32_t lastButtonPressTime = 0;
uint32_t lastUpdate = 0;
bool colorChanged[NUM_PIXELS];

uint8_t currentBrightness[NUM_PIXELS];
uint8_t targetBrightness[NUM_PIXELS];
uint8_t currentColor[NUM_PIXELS];
uint8_t targetColor[NUM_PIXELS];
bool targetColorIsRed[NUM_PIXELS];

void setup() {
  strip.begin();
  strip.show();
  pinMode(BUTTON_PIN, INPUT_PULLUP);

  // Initialize LED settings
  for (uint16_t i = 0; i < NUM_PIXELS; i++) {
    currentBrightness[i] = random(MIN_BRIGHTNESS, MAX_BRIGHTNESS);
    targetBrightness[i] = random(MIN_BRIGHTNESS, MAX_BRIGHTNESS);
    targetColorIsRed[i] = random(2);
    currentColor[i] = targetColorIsRed[i] ? MAX_BRIGHTNESS : 0;
    colorChanged[i] = false;
  }
}

void loop() {
  handleButton();
  updateLEDs();
}

// Handles button input and changes LED mode
void handleButton() {
  if (digitalRead(BUTTON_PIN) == LOW) {
    uint32_t currentTime = millis();
    if (currentTime - lastButtonPressTime > DEBOUNCE_TIME) {
      mode = (mode + 1) % MODE_COUNT;
      lastButtonPressTime = currentTime;
    }
  }
}

// Updates LED brightness and colors based on the current mode
void updateLEDs() {
  uint32_t currentTime = millis();
  if (currentTime - lastUpdate > UPDATE_INTERVAL) {
    for (uint16_t i = 0; i < strip.numPixels(); i++) {
      // Update brightness
      if (abs(currentBrightness[i] - targetBrightness[i]) < FADE_STEP) {
        targetBrightness[i] = mode == 3 ? random(MIN_BRIGHTNESS_CASE_3, MAX_BRIGHTNESS) : random(MIN_BRIGHTNESS, MAX_BRIGHTNESS);
      } else {
        currentBrightness[i] += (targetBrightness[i] > currentBrightness[i]) ? FADE_STEP : -FADE_STEP;
      }

      // Update colors
      uint32_t color;
      if (mode == 3) {
        // Color changing mode
        if (currentBrightness[i] <= COLOR_CHANGE_THRESHOLD && !colorChanged[i]) {
          targetColorIsRed[i] = !targetColorIsRed[i];
          currentColor[i] = targetColorIsRed[i] ? MAX_BRIGHTNESS : 0;
          colorChanged[i] = true;
        } else if (currentBrightness[i] > COLOR_CHANGE_THRESHOLD) {
          colorChanged[i] = false;
        }

        uint8_t r = currentColor[i];
        uint8_t b = MAX_BRIGHTNESS - currentColor[i];
        color = strip.Color(r * currentBrightness[i] / MAX_BRIGHTNESS, 0, b * currentBrightness[i] / MAX_BRIGHTNESS);
      } else {
        // Predefined color modes
        switch (mode) {
          case 0:
            // White mode
            color = strip.Color(currentBrightness[i], currentBrightness[i], currentBrightness[i]);
            break;
          case 1:
            // Red mode
            color = strip.Color(currentBrightness[i], 0, 0);
            break;
          case 2:
            // Blue mode
            color = strip.Color(0, 0, currentBrightness[i]);
            break;
        }
      }

      // Set pixel color
      strip.setPixelColor(i, color);
    }

    // Display updated colors
    strip.show();
    lastUpdate = currentTime;
  }
}
