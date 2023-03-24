#include <Adafruit_NeoPixel.h>

#define NEOPIXEL_PIN 2
#define BUTTON_PIN 8
#define NUM_PIXELS 6
#define MODE_COUNT 4

Adafruit_NeoPixel strip = Adafruit_NeoPixel(NUM_PIXELS, NEOPIXEL_PIN, NEO_GRB + NEO_KHZ800);

uint8_t mode = 0;
uint32_t lastButtonPressTime = 0;
uint32_t lastUpdate = 0;
const uint32_t debounceTime = 150;
const uint32_t updateInterval = 3;
const uint8_t fadeStep = 1;
const uint8_t holdTime = 250;
const uint8_t colorChangeThreshold = 50;
bool colorChanged[NUM_PIXELS];

uint8_t currentBrightness[NUM_PIXELS];
uint8_t targetBrightness[NUM_PIXELS];
uint8_t holdCounter[NUM_PIXELS];
bool targetColorIsRed[NUM_PIXELS];
uint8_t currentColor[NUM_PIXELS];
uint8_t targetColor[NUM_PIXELS];

void setup() {
  strip.begin();
  strip.show();
  pinMode(BUTTON_PIN, INPUT_PULLUP);

  for (uint16_t i = 0; i < NUM_PIXELS; i++) {
    currentBrightness[i] = random(50, 255);
    targetBrightness[i] = random(50, 255);
    holdCounter[i] = 0;
    targetColorIsRed[i] = random(2);
    currentColor[i] = targetColorIsRed[i] ? 255 : 0;
    colorChanged[i] = false;
  }
}

void loop() {
  handleButton();
  updateLEDs();
}

void handleButton() {
  if (digitalRead(BUTTON_PIN) == LOW) {
    uint32_t currentTime = millis();
    if (currentTime - lastButtonPressTime > debounceTime) {
      mode = (mode + 1) % MODE_COUNT;
      lastButtonPressTime = currentTime;
    }
  }
}

void updateLEDs() {
  uint32_t currentTime = millis();
  if (currentTime - lastUpdate > updateInterval) {
    for (uint16_t i = 0; i < strip.numPixels(); i++) {
      if (mode == 3) {
    if (abs(currentBrightness[i] - targetBrightness[i]) < fadeStep) {
        targetBrightness[i] = random(0, 255);
    } else {
        currentBrightness[i] += (targetBrightness[i] > currentBrightness[i]) ? fadeStep : -fadeStep;
    }

    if (currentBrightness[i] <= colorChangeThreshold && !colorChanged[i]) {
        targetColorIsRed[i] = !targetColorIsRed[i];
        currentColor[i] = targetColorIsRed[i] ? 255 : 0;
        colorChanged[i] = true;
    } else if (currentBrightness[i] > colorChangeThreshold) {
        colorChanged[i] = false;
    }

    uint8_t r = currentColor[i];
    uint8_t b = 255 - currentColor[i];
    uint32_t color = strip.Color(r, 0, b);

    uint8_t adjustedR = (r * currentBrightness[i]) / 255;
    uint8_t adjustedB = (b * currentBrightness[i]) / 255;
    color = strip.Color(adjustedR, 0, adjustedB);

    strip.setPixelColor(i, color);
} else {
        if (abs(currentBrightness[i] - targetBrightness[i]) < fadeStep) {
          targetBrightness[i] = random(50, 255);
        } else {
          currentBrightness[i] += (targetBrightness[i] > currentBrightness[i]) ? fadeStep : -fadeStep;
        }

        uint32_t color;
        switch (mode) {
          case 0: color = strip.Color(currentBrightness[i], currentBrightness[i], currentBrightness[i]); break;
          case 1: color = strip.Color(currentBrightness[i], 0, 0); break;
          case 2: color = strip.Color(0, 0, currentBrightness[i]); break;
        }
        strip.setPixelColor(i, color);
      }
    }
    strip.show();
    lastUpdate = currentTime;
  }
}
