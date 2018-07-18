#include "FastLED.h"
#include "MeltdownLED.h"

FASTLED_USING_NAMESPACE

// Boris, short for Explora Borealis, is an interactive light display
// controlled by several individual user inputs.
//
// This project builds off of the FastLED library and some of its existing patterns
// and examples.

#define DATA_PIN 2
#define SWITCH_PIN 13
#define LED_TYPE WS2812B
#define NUM_LEDS 300
CRGB leds[NUM_LEDS];

#define BRIGHTNESS 96

int buttonState = 0;         // variable for reading the pushbutton status
bool canChangeState = true;

void setup()
{
    // initialize serial communication at 9600 bits per second:
    Serial.begin(9600);
    
    delay(3000); // 3 second delay for recovery

    // tell FastLED about the LED strip configuration
    FastLED.addLeds<LED_TYPE, DATA_PIN>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);

    // set master brightness control
    FastLED.setBrightness(BRIGHTNESS);

    pinMode(SWITCH_PIN, INPUT);
}

// List of patterns to cycle through.  Each is defined as a separate function below.
typedef void (*SimplePatternList[])();
SimplePatternList gPatterns = {rainbow, rainbowWithGlitter, confetti, sinelon, juggle, bpm};

uint8_t gCurrentPatternNumber = 0; // Index number of which pattern is current
uint8_t gFps = 120;
uint8_t gHue = 0;                  // rotating "base color" used by many of the patterns

void loop()
{
    gHue = MeltdownLED.GetAnalogValue(A5, gHue, 0, 255);
    gFps = MeltdownLED.GetAnalogValue(A4, gFps, 10, 255);

    // Call the current pattern function once, updating the 'leds' array
    gPatterns[gCurrentPatternNumber]();

    // send the 'leds' array out to the actual LED strip
    FastLED.show();
    // insert a delay to keep the framerate modest
    FastLED.delay(1000 / gFps);

    // read the state of the pushbutton value:
    buttonState = digitalRead(SWITCH_PIN);
    // check if the pushbutton is pressed. If it is, the buttonState is HIGH:
    if (buttonState == LOW && canChangeState) 
    {
        nextPattern();
        canChangeState = false;
    }
    else if (buttonState == HIGH && !canChangeState)
    {
        canChangeState = true;
    }

    // do some periodic updates
    EVERY_N_MILLISECONDS(20) { gHue++; }   // slowly cycle the "base color" through the rainbow
    // EVERY_N_SECONDS(10) { nextPattern(); } // change patterns periodically
}

#define ARRAY_SIZE(A) (sizeof(A) / sizeof((A)[0]))

void nextPattern()
{
    // add one to the current pattern number, and wrap around at the end
    gCurrentPatternNumber = (gCurrentPatternNumber + 1) % ARRAY_SIZE(gPatterns);
}

void rainbow()
{
    // FastLED's built-in rainbow generator
    fill_rainbow(leds, NUM_LEDS, gHue, 7);
}

void rainbowWithGlitter()
{
    // built-in FastLED rainbow, plus some random sparkly glitter
    rainbow();
    addGlitter(80);
}

void addGlitter(fract8 chanceOfGlitter)
{
    if (random8() < chanceOfGlitter)
    {
        leds[random16(NUM_LEDS)] += CRGB::White;
    }
}

void confetti()
{
    // random colored speckles that blink in and fade smoothly
    fadeToBlackBy(leds, NUM_LEDS, 10);
    int pos = random16(NUM_LEDS);
    leds[pos] += CHSV(gHue + random8(64), 200, 255);
}

void sinelon()
{
    // a colored dot sweeping back and forth, with fading trails
    fadeToBlackBy(leds, NUM_LEDS, 20);
    int pos = beatsin16(13, 0, NUM_LEDS - 1);
    leds[pos] += CHSV(gHue, 255, 192);
}

void bpm()
{
    // colored stripes pulsing at a defined Beats-Per-Minute (BPM)
    uint8_t BeatsPerMinute = 62;
    CRGBPalette16 palette = PartyColors_p;
    uint8_t beat = beatsin8(BeatsPerMinute, 64, 255);
    for (int i = 0; i < NUM_LEDS; i++)
    { //9948
        leds[i] = ColorFromPalette(palette, gHue + (i * 2), beat - gHue + (i * 10));
    }
}

void juggle()
{
    // eight colored dots, weaving in and out of sync with each other
    fadeToBlackBy(leds, NUM_LEDS, 20);
    byte dothue = 0;
    for (int i = 0; i < 8; i++)
    {
        leds[beatsin16(i + 7, 0, NUM_LEDS - 1)] |= CHSV(dothue, 200, 255);
        dothue += 32;
    }
}
