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

#include <FastLED.h>
#define NUM_LEDS 60
#define DATA_PIN 6

CRGB leds[NUM_LEDS];

int brightness = 0;

// The number of Steps between the output being on and off
const int pwmIntervals = 100;

// The R value in the graph equation
float R;

void setup() { 
  Serial.begin(9600);
  FastLED.addLeds<NEOPIXEL, DATA_PIN>(leds, NUM_LEDS);
  
  // set the pin connected to the LED as an output
  pinMode(DATA_PIN, OUTPUT);

  // calculate the R variable (only needs to be done once at setup)
  R = (pwmIntervals * log10(5))/(log10(255)); // increase numerator for smoother fade
}

void loop() { 
  pulse();
}

void dim(int brightness) {
  for(int dot = 0; dot < NUM_LEDS; dot++) { 
    leds[dot].setHSV(0,0,brightness); // hue, sat, val or "brightness"
    FastLED.show();
//    delay(30);
  }
}

void chase() {
  for(int dot = 0; dot < NUM_LEDS; dot++) { 
    leds[dot] = CRGB::White;
    FastLED.show();
    // clear this led for the next time around the loop
    leds[dot] = CRGB::Black;
    delay(30);
  }
}

void on() {
   for (int dot = 0; dot < NUM_LEDS; dot++) {
     leds[dot] = CRGB::White; 
     FastLED.show(); 
     delay(30);   
  }
}

void printStats(int interval, int brightness) {
    Serial.print("INTERVAL :");
    Serial.print("\t");
    Serial.print(interval);
    Serial.println("");
    
    Serial.print("BRIGHTNESS: ");
    Serial.print("\t");
    Serial.print(brightness);
    Serial.println("");
}

void brightenLED() {
  // LED brightness increasing
  Serial.println("RAMPING UP......");

  // loop through from 0 to 100, counting up
  for (int interval = 0; interval <= pwmIntervals; interval++) {
    setLED(interval);
  }
}

void dimLED() {
  // LED brightness decreasing
  Serial.println("WINDING DOWN......");

  // loop through from 100 to 0, counting down
  for (int interval = 100; interval >= 0; interval--) {
    setLED(interval);
  }
}

void setLED(int interval) {
    // calculate the required PWM value for this interval step
    brightness = pow(5, (interval / R)) - 1; // was 2 instead of 5

    // set the LED ring output to the calculated brightness
    dim(brightness);

    // print the loop iteration and brightness levels
    printStats(interval, brightness);

    // delay for 40 ms between each loop
//    delay(40);
}

void pulse() {
  brightenLED();
  dimLED();
}
