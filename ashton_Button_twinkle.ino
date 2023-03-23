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
      pixels.fill(pixels.Color(255, 0, 0), 0, NUM_PIXELS);
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
      // Twinkle mode between red and blue
      if (millis() - lastTwinkle > twinkleDelay) {
        for (int i = 0; i < NUM_PIXELS; i++) {
          if (random(0, 5) == 0) { // randomly choose a pixel to twinkle
            float fadeAmount = random(0.1, 0.9); // randomly choose the amount to fade between red and blue
            int red = fadeAmount * 255;
            int blue = (1 - fadeAmount) * 255;
            pixels.setPixelColor(i, pixels.Color(red, 0, blue));
          } else { // keep non-twinkling pixels red or blue
            if (random(0, 2) == 0) {
              pixels.setPixelColor(i, pixels.Color(255, 0, 0)); // red
            } else {
              pixels.setPixelColor(i, pixels.Color(0, 0, 255)); // blue
            }
          }
        }
        pixels.show();
        lastTwinkle = millis();
      }
      break;

    default:
      // Invalid mode, do nothing
      break;
  }
}