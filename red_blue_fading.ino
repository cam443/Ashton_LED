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
unsigned long fadeStartTimes[NUM_PIXELS] = {0};



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
    unsigned long currentTime = millis();

    // Fade duration (milliseconds)
    int fadeDuration = 500;

    // Breathing effect duration (milliseconds)
    int breathingDuration = 2000;

    // Apply the breathing effect on top of the current color
    uint32_t currentColor = pixels.getPixelColor(i);
    uint8_t r = (currentColor >> 16) & 0xFF;
    uint8_t g = (currentColor >> 8) & 0xFF;
    uint8_t b = currentColor & 0xFF;
    float brightnessMultiplier = 0.5 + 0.5 * sin(2 * PI * currentTime / (float)breathingDuration);
    uint8_t newR = r * brightnessMultiplier;
    uint8_t newG = g * brightnessMultiplier;
    uint8_t newB = b * brightnessMultiplier;
    pixels.setPixelColor(i, newR, newG, newB);

    // Perform the fade transition
    unsigned long elapsedFadeTime = currentTime - fadeStartTimes[i];
    if (elapsedFadeTime < fadeDuration) {
      uint32_t startColor = fadeStartColors[i];
      uint32_t endColor = fadeEndColors[i];

      float progress = (float)elapsedFadeTime / fadeDuration;
      uint8_t newR = (((startColor >> 16) & 0xFF) * (1 - progress)) + (((endColor >> 16) & 0xFF) * progress);
      uint8_t newG = (((startColor >> 8) & 0xFF) * (1 - progress)) + (((endColor >> 8) & 0xFF) * progress);
      uint8_t newB = ((startColor & 0xFF) * (1 - progress)) + ((endColor & 0xFF) * progress);
      pixels.setPixelColor(i, newR, newG, newB);
    } else if (currentTime - lastBreathing[i] > breathingDelays[i]) {
      // Determine if the LED should twinkle to the other color
      bool twinkle = random(0, 100) < 5; // 5% chance to twinkle

      if (twinkle) {
        // Get the current color of the LED
        uint32_t currentColor = pixels.getPixelColor(i);

        // Determine the target color based on the current color
        uint32_t targetColor = ((currentColor >> 16) & 0xFF) > 0 ? pixels.Color(0, 0, 255) : pixels.Color(255, 0, 0);

        // Update the start and end colors for the fade transition
        fadeStartColors[i] = currentColor;
        fadeEndColors[i] = targetColor;

        // Set a new random breathing time for this LED
        int holdTime = random(1000, 5000); // random hold time between 1 and 5 seconds
        breathingDelays[i] = holdTime;
        lastBreathing[i] = currentTime;
        fadeStartTimes[i] = currentTime;
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
