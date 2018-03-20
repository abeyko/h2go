/*
 * H2Go 

 * Soft transluscent rubber raindrop with embedded electronics.
 * 
 * H2Go is fully portable and reminds users to drink 8 glasses of
 * water a day.
 * 
 * LED brightness changes from dim to fully bright
 * and then reverses from fully bright to dim. Does repeatedly until
 * user presses down on raindrop to indicate they have drank a 
 * glass of water. 
 * 
 * Created by Angeliki Beyko on March 08, 2018
 * 
 * Sources used: 
 * http://diarmuid.ie/blog/pwm-exponential-led-fading-on-arduino-or-other-platforms/
 * https://github.com/FastLED/FastLED/wiki/Basic-usage
 * https://github.com/FastLED/FastLED/wiki/Pixel-reference
 * https://www.rapidtables.com/convert/color/rgb-to-hsv.html
 * 
 */

// import library
#include <FastLED.h>

// define constants
#define NUM_LEDS 12
#define DATA_PIN 6
#define INTERRUPT_PIN 2
#define PWM_INTERVALS 100

CRGB leds[NUM_LEDS];

int glassesCounter = 0;
int brightness = 0;
int base = 10;

int color = 0;
int randomBrightness = 0;

bool fullyHydrated = false;
bool animationComplete = false;

volatile byte buttonState = 0;

// The R value in the graph equation
float R;

void setup() {
  Serial.begin(9600);

  // calculate the R variable (only needs to be done once at setup)
  R = (PWM_INTERVALS * log10(base))/(log10(255));
  
  pinMode(INTERRUPT_PIN, INPUT);

  FastLED.addLeds<NEOPIXEL, DATA_PIN>(leds, NUM_LEDS);
  pinMode(DATA_PIN, OUTPUT);

  attachInterrupt(digitalPinToInterrupt(INTERRUPT_PIN), blink, CHANGE);
}

void loop() {
  if (fullyHydrated == false) {
    pulse();
  }   
}

void blink() {
  buttonState = !buttonState;
}

void twinkle() {
  // delay to not confuse previous button press with final button press
  delay(1000);
  while (animationComplete == false) {
    for (int dot = 0; dot < NUM_LEDS; dot++) {
      randomBrightness = random(0,256);
      color = random(0,3);
        switch (color) {
        case 0:
          leds[dot].setHSV(91,20,randomBrightness);
          break;
        case 1:
          leds[dot].setHSV(232,81,randomBrightness);
          break;
        case 2:
          leds[dot].setHSV(180,73,randomBrightness);
          break;
      }
      if (buttonState == 1) {
        animationComplete = true;  
      }
    }
    FastLED.show();
    delay(100);
  }
}

void buttonPressed() {
  Serial.print("glasses counter is: ");
  Serial.println(glassesCounter);
  if (glassesCounter == 3) { // changed from 8
    fullyHydrated = true;
  }
  if (fullyHydrated == true) {
    Serial.println("Fully hydrated!!!");
    twinkle();
  } else {
    Serial.println("NOT yet fully hydrated!!!");
    for (int i = 0; i < 12; i++) {
      leds[i] = CRGB::Black;
    }
    for (int glasses = 0; glasses < glassesCounter; glasses++) {
      leds[glasses] = CRGB::White;
    }
    FastLED.show();
    delay(3000); // change to 3600000 ms (equal to 1 hr)
  }
}

void set(int brightness) {
  for(int dot = 0; dot < NUM_LEDS; dot++) { 
    leds[dot].setHSV(0,0,brightness); // hue, sat, val or "brightness"
    if (buttonState == 1) {
      glassesCounter++;
      buttonPressed();
    }
    buttonState = 0;
  }
  FastLED.show(); // update only once after entire array is set
}

void updateBrightness(int interval) {
  // calculate the required PWM value for this interval step
  brightness = pow(base, (interval / R)) - 1;
  
  // set the LED ring output to the calculated brightness
  set(brightness);

  delay(10);
}

void brighten() {
  for (int interval = 0; interval <= PWM_INTERVALS; interval++) {
    updateBrightness(interval);
  }
}

void dim() {
  for (int interval = 100; interval >= 0; interval--) {
    updateBrightness(interval);
  }
}

void pulse() {
  brighten();
  dim();
}
