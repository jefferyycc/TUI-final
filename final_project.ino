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

bool gReverseDirection = false;

// Output
//int led_A_Pin  = 13;   // LED,   connected to digital pin 9
//int led_B_Pin  = 12;   // LED,   connected to digital pin 9
int fs_A_Pin   = A0;   // 
int fs_B_Pin   = A1;   // 

// Input 


// Program variables
CRGB leds[NUM_LEDS];
CRGBPalette16 gPal;
int DEBUG = 0; // DEBUG counter; if set to 1, will write values back via serial

void setup() {
  // sets the pins as Output
//  pinMode(led_A_Pin,   OUTPUT);
//  pinMode(led_B_Pin,   OUTPUT);
  delay(3000); // sanity delay
  FastLED.addLeds<CHIPSET, led_A_Pin, COLOR_ORDER>(leds, NUM_LEDS).setCorrection( TypicalLEDStrip );
  FastLED.addLeds<CHIPSET, led_B_Pin, COLOR_ORDER>(leds, NUM_LEDS).setCorrection( TypicalLEDStrip );
  FastLED.setBrightness( BRIGHTNESS );
  gPal = HeatColors_p;

  // Begin the Serial at 9600 Baud
  if (DEBUG) {           // If we want to see the pin values for debugging...
    Serial.begin(9600);  // ...set up the serial ouput on 0004 style
  }
  
}

// Main program
void loop() {
  random16_add_entropy(random());
  Fire2012WithPalette(); // run simulation frame, using palette colors
  FastLED.show(); // display this frame
  FastLED.delay(1000 / FRAMES_PER_SECOND);
  
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
