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
 * Last modified on Jan 29, 2019 with help from Eric Marks
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

#define TWINKLE_SPEED 100
#define PULSE_SPEED 10

#define PULSE_DELAY_TIME 10000
#define TWINKLE_DELAY_TIME 5000

#define GOAL_GLASSES 3

#define PWM_INTERVALS 100

CRGB leds[NUM_LEDS];

int glassesCounter = 0;

//device states
#define DEVICE_STATE_PULSE 0
#define DEVICE_STATE_TWINKLE 1
#define DEVICE_STATE_PULSE_DELAY 2
#define DEVICE_STATE_DONE 3
int device_state = DEVICE_STATE_PULSE_DELAY;

//keeps track of when state changes happen
unsigned long int device_state_change_time = 0;
//keeps track of the previous state so the program can detect state changes by itself and I dont need to manually record state change times
int device_state_previous = device_state;

//pre-calculate R for pulsing animation since it will never change at runtime
float R = 41.553430;
int base = 10;   

volatile bool button_pressed = false;

void button_press() {
  if (device_state == DEVICE_STATE_PULSE) button_pressed = true;
}

void setup() {
  Serial.begin(9600);

  
  R = (PWM_INTERVALS * log10(base)) / (log10(255));
  pinMode(INTERRUPT_PIN, INPUT); //button is pulled high, grounded when pressed

  FastLED.addLeds<NEOPIXEL, DATA_PIN>(leds, NUM_LEDS);
  pinMode(DATA_PIN, OUTPUT);

  attachInterrupt(digitalPinToInterrupt(INTERRUPT_PIN), button_press, RISING);  //only tigger on press
}


void loop() {

  if (device_state == DEVICE_STATE_PULSE_DELAY || device_state == DEVICE_STATE_DONE) {
    for (int i = 0; i < 12; i++) leds[i] = CRGB::Black;
  }

  if (device_state == DEVICE_STATE_PULSE)    pulse();
  if (device_state == DEVICE_STATE_TWINKLE)  twinkle();

  if (button_pressed) {
    button_pressed = false;  //reset button pressed flag
    glassesCounter++;
    Serial.print("glasses counter is: ");
    Serial.println(glassesCounter);

    if (glassesCounter == GOAL_GLASSES) {
      Serial.println("Fully hydrated!!!");
      device_state = DEVICE_STATE_TWINKLE;
    }
    else {
      device_state = DEVICE_STATE_PULSE_DELAY;
    }
  }

  FastLED.show();

  //set timer if state changed
  if (device_state_previous != device_state) {
    device_state_change_time = millis();
  }

  //automatic changes between states
  if (device_state == DEVICE_STATE_PULSE_DELAY) {
    if (millis() - device_state_change_time > PULSE_DELAY_TIME) {
      device_state = DEVICE_STATE_PULSE;
    }
  }

  if (device_state ==  DEVICE_STATE_TWINKLE) {
    if (millis() - device_state_change_time > TWINKLE_DELAY_TIME) {
      device_state = DEVICE_STATE_DONE;
    }
  }

  //record last state
  device_state_previous = device_state;
}

//non-blocking twinkle
void twinkle() {
  static unsigned long int last_twinkle_time = 0;  //static makes this variable be remembered between function calls
  //this could be a global, but using a static keeps the scope cleaner
  //uint_32t is the same as a long unsigned int on arduino, probably better to use the explit uint32_t type if you port code to different cpus

  if (millis() - last_twinkle_time > TWINKLE_SPEED) {  //only twinkle when enough time (TWINKLE_SPEED milliseconds) has passed
    last_twinkle_time = millis(); // re-set the timer

    for (int dot = 0; dot < NUM_LEDS; dot++) {
      int randomBrightness = random(0, 256);
      int color = random(0, 3);
      switch (color) {
        case 0:
          leds[dot].setHSV(91, 20, randomBrightness);
          break;
        case 1:
          leds[dot].setHSV(232, 81, randomBrightness);
          break;
        case 2:
          leds[dot].setHSV(180, 73, randomBrightness);
          break;
      }
    }
  }
}


//non-blocking pulse
//brighten and dim are not used anywhere else in the code
//so its better to just bring them in to this function
void pulse() {
  static unsigned long int last_pulse_time = 0;
  static bool counting_up = true;
  static int interval = 0;
  if (millis() - last_pulse_time > PULSE_SPEED) {

    //reset animation if it has been a long time since called
    if (millis() - last_pulse_time > 4 * PULSE_SPEED) {
      interval = 0;
      counting_up = true;
    }

    last_pulse_time = millis();

    //this animation could be redone with cleaner code, but I don't want to risk changing how it looks right now

    if (counting_up) interval++;
    else             interval--;

    int brightness = pow(base, (interval / R)) - 1;

    // set the LED ring output to the calculated brightness
    for (int dot = 0; dot < NUM_LEDS; dot++) {
      leds[dot].setHSV(0, 0, brightness); // hue, sat, val or "brightness"
    }

    if (interval == 100) counting_up = false;
    if (interval == 0)   counting_up = true;
  }
}
