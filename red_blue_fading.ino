#include <Adafruit_NeoPixel.h>

#define BUTTON_PIN 8
#define NUM_PIXELS 6
#define PIXEL_PIN 2

Adafruit_NeoPixel pixels(NUM_PIXELS, PIXEL_PIN, NEO_GRB + NEO_KHZ800);

int mode = 0;
unsigned long lastButtonPress = 0;
unsigned long lastModeChange = 0;
unsigned long lastTwinkle = 0;
unsigned long twinkleDelay = 1000; // set the delay between twinkle effects in milliseconds
unsigned long lastBreathing[NUM_PIXELS]; // create an array of last breathing times, one for each LED
const unsigned long breathingDelay = 150;
unsigned long breathingDelays[NUM_PIXELS]; // create an array of breathing delays, one for each LED
uint32_t fadeStartColors[NUM_PIXELS];
uint32_t fadeEndColors[NUM_PIXELS];
unsigned long fadeStartTimes[NUM_PIXELS];
unsigned long fadeDuration = 500;

uint32_t mixColors(uint32_t col1, uint32_t col2, float factor) {
  float red = ((col1 >> 16) & 0xFF) * (1 - factor) + ((col2 >> 16) & 0xFF) * factor;
  float green = ((col1 >> 8) & 0xFF) * (1 - factor) + ((col2 >> 8) & 0xFF) * factor;
  float blue = (col1 & 0xFF) * (1 - factor) + (col2 & 0xFF) * factor;
  return pixels.Color(red, green, blue);
}

uint32_t lerpColor(uint32_t color1, uint32_t color2, float t) {
  uint8_t r1 = (color1 >> 16) & 0xFF;
  uint8_t g1 = (color1 >> 8) & 0xFF;
  uint8_t b1 = color1 & 0xFF;

  uint8_t r2 = (color2 >> 16) & 0xFF;
  uint8_t g2 = (color2 >> 8) & 0xFF;
  uint8_t b2 = color2 & 0xFF;

  uint8_t r = r1 + (r2 - r1) * t;
  uint8_t g = g1 + (g2 - g1) * t;
  uint8_t b = b1 + (b2 - b1) * t;

  return ((uint32_t)r << 16) | ((uint32_t)g << 8) | b;
}

uint32_t scaleColor(uint32_t color, float scale) {
  uint8_t r = (color >> 16) & 0xFF;
  uint8_t g = (color >> 8) & 0xFF;
  uint8_t b = color & 0xFF;

  r = r * scale;
  g = g * scale;
  b = b * scale;

  return ((uint32_t)r << 16) | ((uint32_t)g << 8) | b;
}

void setup() {
  pixels.begin();
  pinMode(BUTTON_PIN, INPUT_PULLUP);
   for (int i = 0; i < NUM_PIXELS; i++) {
    lastBreathing[i] = millis() + random(0, 500); // set a random initial breathing time for each LED
  }
}

void loop() {
  if (millis() - lastButtonPress > 50) { // debounce button
    if (digitalRead(BUTTON_PIN) == LOW) {
      if (millis() - lastModeChange > 500) { // check if enough time has passed since the last mode change
        mode = (mode + 1) % 4;
        if (mode == 4) { // loop back to first mode from fourth mode
          mode = 0;
        }
        lastModeChange = millis();
      }
      lastButtonPress = millis();
    }
  }

  switch (mode) {
    case 0:
      // Red mode
      for (int i = 0; i < NUM_PIXELS; i++) {
        if (millis() - lastBreathing[i] > breathingDelay) {
          int brightness = (int)(128 + 128 * sin(2 * PI * millis() / (2000.0 + 500 * i))); // calculate the brightness based on a sine wave and the LED index
          pixels.setPixelColor(i, pixels.Color(brightness, 0, 0)); // set the pixel color to white with the calculated brightness
          lastBreathing[i] = millis() + random(0, 500); // set a new random breathing time for this LED
        }
      }
      pixels.show();
      break;

    case 1:
      // Blue mode
      pixels.fill(pixels.Color(0, 0, 255), 0, NUM_PIXELS);
      pixels.show();
      break;

    case 2:
      // Breathing effect on white
      for (int i = 0; i < NUM_PIXELS; i++) {
        if (millis() - lastBreathing[i] > breathingDelay) {
          int brightness = (int)(128 + 128 * sin(2 * PI * millis() / (2000.0 + 500 * i))); // calculate the brightness based on a sine wave and the LED index
          pixels.setPixelColor(i, pixels.Color(brightness, brightness, brightness)); // set the pixel color to white with the calculated brightness
          lastBreathing[i] = millis() + random(0, 500); // set a new random breathing time for this LED
        }
      }
      pixels.show();
      break;

    case 3:
  // Red and blue mode with random twinkling and breathing
  for (int i = 0; i < NUM_PIXELS; i++) {
    unsigned long now = millis();
    if (now - lastBreathing[i] > breathingDelays[i]) {
      // Determine if the LED should twinkle to the other color
      bool twinkle = random(0, 100) < 5; // 5% chance to twinkle

      if (twinkle) {
        // Get the current color of the LED
        uint32_t currentColor = pixels.getPixelColor(i);

        // Determine the target color based on the current color
        uint32_t targetColor = ((currentColor >> 16) & 0xFF) > 0 ? pixels.Color(0, 0, 255) : pixels.Color(255, 0, 0);

        // Initialize the fade variables
        fadeStartTimes[i] = now;
        fadeStartColors[i] = currentColor;
        fadeEndColors[i] = targetColor;

        // Set a new random breathing time for this LED
        breathingDelays[i] = random(1000, 5000); // random hold time between 1 and 5 seconds
        lastBreathing[i] = now;
      }
    }

    // Apply the fade and breathing effect
    if (fadeStartTimes[i] > 0) {
      float fadeProgress = float(now - fadeStartTimes[i]) / float(fadeDuration);
      if (fadeProgress < 1.0) {
        uint32_t newColor = lerpColor(fadeStartColors[i], fadeEndColors[i], fadeProgress);
        float brightnessScale = 0.5 + 0.5 * sin(2 * PI * now / 2000.0);
        pixels.setPixelColor(i, scaleColor(newColor, brightnessScale));
      } else {
        fadeStartTimes[i] = 0;
        pixels.setPixelColor(i, fadeEndColors[i]);
      }
    }
  }
  pixels.show();
  break;

    default:
      // Invalid mode, do nothing
      break;
  }
}
