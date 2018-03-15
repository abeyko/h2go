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

unsigned long startTime;
unsigned long endTime;

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
  R = (pwmIntervals * log10(10))/(log10(255)); // increase numerator for smoother fade
}

void loop() { 
  pulse();
}

void set(int brightness) {
  for(int dot = 0; dot < NUM_LEDS; dot++) { 
    leds[dot].setHSV(0,0,brightness); // hue, sat, val or "brightness"
  }
  FastLED.show(); // update only once after entire array is set
}

//void chase() {
//  for(int dot = 0; dot < NUM_LEDS; dot++) { 
//    leds[dot] = CRGB::White;
//    FastLED.show();
//    // clear this led for the next time around the loop
//    leds[dot] = CRGB::Black;
//    delay(30);
//  }
//}
//
//void on() {
//   for (int dot = 0; dot < NUM_LEDS; dot++) {
//     leds[dot] = CRGB::White; 
//     FastLED.show(); 
//     delay(30);   
//  }
//}

void updateBrightness(int interval) {
    // calculate the required PWM value for this interval step
    brightness = pow(10, (interval / R)) - 1; // power input should match numerator above

    // set the LED ring output to the calculated brightness
    set(brightness);

    // print the loop iteration and brightness levels
    Serial.println(brightness);

    // delay for 10 ms between each loop
    delay(10);
}

void brighten() {
  // LED ring brightness increasing
  Serial.println("RAMPING UP......");

  // loop through from 0 to 100, counting up
  for (int interval = 0; interval <= pwmIntervals; interval++) {
    updateBrightness(interval);
  }
}

void dim() {
  // LED ring brightness decreasing
  Serial.println("WINDING DOWN......");

  // loop through from 100 to 0, counting down
  for (int interval = 100; interval >= 0; interval--) {
    updateBrightness(interval);
  }
}

void pulse() {
  startTime = micros();
  
  brighten();
  dim();

  endTime = micros();
  unsigned long difference = endTime - startTime;
  Serial.print("startTime is: ");
  Serial.println(startTime);
  Serial.print("endTime is: ");
  Serial.println(endTime);
  Serial.print("PULSE FUNCTION EXECUTION TIME: ");
  Serial.print(difference / 1000); // converting microseconds to milliseconds
  Serial.println(" ms");
}
