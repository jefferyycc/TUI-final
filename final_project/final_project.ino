/*
 * TUI - Final Project
 * Jeffery, Eugenia, Cristal
 * 
 */
#include <FastLED.h>


#define led_A_Pin     13
#define led_B_Pin     12
#define COLOR_ORDER GRB
#define CHIPSET     WS2811
#define NUM_LEDS    30

#define BRIGHTNESS  200
#define FRAMES_PER_SECOND 60
#define SAMPLE_WINDOW   10  // Sample window for average level

bool gReverseDirection = false;

// Output
//int led_A_Pin  = 13;   // LED,   connected to digital pin 9
//int led_B_Pin  = 12;   // LED,   connected to digital pin 9
int motor_A_Pin = 9;
int motor_B_Pin = 10;

// Input 
int fs_A_Pin   = A0;   // 
int fs_B_Pin   = A1; 
int mic_A_Pin  = A2; 
int mic_B_Pin  = A3; 

// Program variables
CRGB leds[NUM_LEDS];
CRGBPalette16 gPal;
int DEBUG = 1; // DEBUG counter; if set to 1, will write values back via serial
int motor_A_Val = 0; // motor control
int motor_B_Val = 0; // motor control
int pre_motr_val = 10;
int fs_A_Val = 0;    // val for the force sensor
int fs_B_Val = 0;
int mic_A_Val = 0;    // val for the force sensor
int mic_B_Val = 0;
unsigned int sample;

void setup() {
  // sets the pins as Output
//  pinMode(led_A_Pin,   OUTPUT);
//  pinMode(led_B_Pin,   OUTPUT);
  delay(3000); // sanity delay
  FastLED.addLeds<CHIPSET, led_A_Pin, COLOR_ORDER>(leds, NUM_LEDS).setCorrection( TypicalLEDStrip );
  FastLED.addLeds<CHIPSET, led_B_Pin, COLOR_ORDER>(leds, NUM_LEDS).setCorrection( TypicalLEDStrip );
  FastLED.setBrightness( BRIGHTNESS );
  gPal = HeatColors_p;

  pinMode(motor_A_Pin,   OUTPUT);
  pinMode(motor_B_Pin,   OUTPUT);

  // sets the pins as Input
  pinMode(fs_A_Pin, INPUT);
  pinMode(fs_B_Pin, INPUT);
  pinMode(mic_A_Pin, INPUT);
  pinMode(mic_B_Pin, INPUT);

  // Begin the Serial at 9600 Baud
  if (DEBUG) {           // If we want to see the pin values for debugging...
    Serial.begin(9600);  // ...set up the serial ouput on 0004 style
  }
  
}

// Main program
void loop() {
  // Outputs
  random16_add_entropy(random());
  Fire2012WithPalette(); // run simulation frame, using palette colors
  FastLED.show(); // display this frame
  FastLED.delay(1000 / FRAMES_PER_SECOND);

  //motorCtrlVal = map(potVal, 0, 1024, 0, 255);
  motor_A_Val = pre_motr_val; // rang [0, 255]
  motor_B_Val = motor_A_Val;
  Serial.print("motor_A_lVal: ");Serial.println(motor_A_Val);
  Serial.print("motor_B_lVal: ");Serial.println(motor_B_Val);
  analogWrite(motor_A_Pin, motor_A_Val);
  analogWrite(motor_B_Pin, motor_B_Val);

  // Inputs
  fs_A_Val = analogRead(fs_A_Pin);
  fs_B_Val = analogRead(fs_B_Pin);
  Serial.print(" fs_A_Val: ");Serial.println(fs_A_Val);
  Serial.print(" fs_B_Val: ");Serial.println(fs_B_Val);
  //delay (1000);

  // Microphone
  unsigned long startMillis= millis();  // Start of sample window
  float peakToPeak = 0;   // peak-to-peak level
  unsigned int signalMax = 0;
  unsigned int signalMin = 1023;
  unsigned int c, y;

  // collect data for length of sample window (in mS)
  while (millis() - startMillis < SAMPLE_WINDOW)
  {
    sample = analogRead(mic_A_Pin);
    if (sample < 1024)  // toss out spurious readings
    {
      if (sample > signalMax)
      {
        signalMax = sample;  // save just the max levels
      }
      else if (sample < signalMin)
      {
        signalMin = sample;  // save just the min levels
      }
    }
  }
  peakToPeak = signalMax - signalMin;  // max - min = peak-peak amplitude
  Serial.print(" peakToPeak: ");Serial.println(peakToPeak);
//   Serial.print(" sample: ");Serial.println(sample);
  delay(200);
  
}

//================================================================================
// FastLED functions
#define COOLING  55

// SPARKING: What chance (out of 255) is there that a new spark will be lit?
// Higher chance = more roaring fire.  Lower chance = more flickery fire.
// Default 120, suggested range 50-200.
#define SPARKING 120


void Fire2012WithPalette()
{
// Array of temperature readings at each simulation cell
  static byte heat[NUM_LEDS];

  // Step 1.  Cool down every cell a little
    for( int i = 0; i < NUM_LEDS; i++) {
      heat[i] = qsub8( heat[i],  random8(0, ((COOLING * 10) / NUM_LEDS) + 2));
    }
  
    // Step 2.  Heat from each cell drifts 'up' and diffuses a little
    for( int k= NUM_LEDS - 1; k >= 2; k--) {
      heat[k] = (heat[k - 1] + heat[k - 2] + heat[k - 2] ) / 3;
    }
    
    // Step 3.  Randomly ignite new 'sparks' of heat near the bottom
    if( random8() < SPARKING ) {
      int y = random8(7);
      heat[y] = qadd8( heat[y], random8(160,255) );
    }

    // Step 4.  Map from heat cells to LED colors
    for( int j = 0; j < NUM_LEDS; j++) {
      // Scale the heat value from 0-255 down to 0-240
      // for best results with color palettes.
      byte colorindex = scale8( heat[j], 240);
      CRGB color = ColorFromPalette( gPal, colorindex);
      int pixelnumber;
      if( gReverseDirection ) {
        pixelnumber = (NUM_LEDS-1) - j;
      } else {
        pixelnumber = j;
      }
      leds[pixelnumber] = color;
    }
}
