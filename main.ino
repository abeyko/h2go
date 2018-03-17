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

int brightness = 0;
int base = 10;

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
  pulse(); 
}

void blink() {
  buttonState = !buttonState;
}

void set(int brightness) {
  for(int dot = 0; dot < NUM_LEDS; dot++) { 
    leds[dot].setHSV(0,0,brightness); // hue, sat, val or "brightness"
    if (buttonState == 1) {
      Serial.println("state has changed!");
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

//  Serial.println(brightness);
  delay(10);
}

void brighten() {
//  Serial.println("RAMPING UP......");
  for (int interval = 0; interval <= PWM_INTERVALS; interval++) {
    updateBrightness(interval);
  }
}

void dim() {
//  Serial.println("WINDING DOWN......");
  for (int interval = 100; interval >= 0; interval--) {
    updateBrightness(interval);
  }
}

void pulse() {
  brighten();
  dim();
}
