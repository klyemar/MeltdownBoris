#include <FastLED.h>
#include <MeltdownLED.h>

#define NUM_LEDS 6

#define HUE_PIN A5
#define FADE_PIN A4

CRGBArray<NUM_LEDS> leds;

uint8_t hue = 0;
uint8_t fade = 0;

void setup() { FastLED.addLeds<NEOPIXEL, 2>(leds, NUM_LEDS); }

void loop()
{
    hue = MeltdownLED.GetAnalogValue(HUE_PIN, hue, 0, 255);
    fade = MeltdownLED.GetAnalogValue(FADE_PIN, fade, 0, 255);

    for (int i = 0; i < NUM_LEDS / 2; i++)
    {
        // fade everything out
        leds.fadeToBlackBy(fade);

        // let's set an led value
        leds[i] = CHSV(hue, 255, 255);

        // now, let's first 20 leds to the top 20 leds,
        leds(NUM_LEDS / 2, NUM_LEDS - 1) = leds(NUM_LEDS / 2 - 1, 0);
        FastLED.delay(150);
    }
}
