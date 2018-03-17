// import library
#include <FastLED.h>

// define constants
#define NUM_LEDS 60 // CHANGE THIS?
#define DATA_PIN 6
#define BUTTON_PIN 2
#define PWM_INTERVALS 100
#define LED_UPDATE_INTERVAL 2000 // INSTEAD OF 20 
#define BUTTON_INTERVAL 100 // INSTEAD OF 100
#define ANIMATION_UPDATE_INTERVAL 1000 // INSTEAD OF 10

CRGB leds[NUM_LEDS];

int animation_frame = 0;
int brightness = 0;
int base = 10;

unsigned long ledUpdateInterval_previous = 0; 
unsigned long buttonInterval_previous = 0; 
unsigned long animationUpdateInterval_previous = 0;

// The R value in the graph equation
float R;

void setup() {
  Serial.begin(9600);

  // calculate the R variable (only needs to be done once at setup)
  R = (PWM_INTERVALS * log10(base))/(log10(255));
  
  pinMode(BUTTON_PIN, INPUT);

  FastLED.addLeds<NEOPIXEL, DATA_PIN>(leds, NUM_LEDS);
  pinMode(DATA_PIN, OUTPUT);
}

void loop() {
  unsigned long currentMillis1 = millis();
  int millisElapsed1 = currentMillis1 - ledUpdateInterval_previous;

  if (millis() - animationUpdateInterval_previous >= ANIMATION_UPDATE_INTERVAL) {
    pulse();
//    animate_leds();
    animationUpdateInterval_previous = millis();
  }

  if (millis() - ledUpdateInterval_previous >= LED_UPDATE_INTERVAL) {
    
    FastLED.show();
    ledUpdateInterval_previous = millis();
  }

  if (millis() - buttonInterval_previous >= BUTTON_INTERVAL) {

    int buttonState = digitalRead(BUTTON_PIN);
    
    Serial.print("button state is: ");
    Serial.println(buttonState);

    buttonInterval_previous = millis();
  }
}

void set(int brightness) {
  for(int dot = 0; dot < NUM_LEDS; dot++) {
    if (dot <= animation_frame)  {
      leds[dot].setHSV(0,0,brightness);
    }
  }

  animation_frame++;
  if (animation_frame > NUM_LEDS) {
    animation_frame=0;
  }

  FastLED.show();
}

void updateBrightness(int interval) {
  // calculate the required PWM value for this interval step
  brightness = pow(base, (interval / R)) - 1;
  
  // set the LED ring output to the calculated brightness
  set(brightness);

//  Serial.println(brightness);
  delay(5); // WAS 10
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

void animate_leds() {
  for(int dot = 0; dot < NUM_LEDS; dot++) { 
    if (dot <= animation_frame)  {
      leds[dot] = CRGB::Blue;
    }
    else {
      leds[dot] = CRGB::Black;
    }
  }

  animation_frame++;
  if (animation_frame > NUM_LEDS) {
    animation_frame=0;
  }
}
