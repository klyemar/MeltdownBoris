#include <Arduino.h>
#include <FastLED.h>

class CMeltdownLED
{
    public: CMeltdownLED();

    uint8_t m_analogTolerance;

    // Global LED values.
    uint8_t gCurrentPatternNumber = 0; // Index number of which pattern is current
    uint8_t gBrightness = 96;
    uint8_t gHue = 0;                  // rotating "base color" used by many of the patterns
    uint16_t gFps = 1000;
    bool gInverse = false;
    bool gPause = false;
    float gFade = 20;
    float gPos = 0;
    bool gHue1 = false;
    bool gHue2 = false;
    bool gHue3 = false;
    bool gHue4 = false;
    bool gHue5 = false;

    // List of patterns to cycle through.  Each is defined as a separate function below.
    typedef void (*SimplePatternList[])(CRGB*[], int);
    SimplePatternList gPatterns = {rainbow, rainbowWithGlitter, confetti, sinelon, bpm, juggle};

    float GetAnalogValue(uint8_t pin, float currVal, int32_t minVal, int32_t maxVal)
    {
        uint16_t rawVal = analogRead(pin);
        float normalizedVal = map(rawVal, 0, 1023, minVal, maxVal);

        if (HasChanged(currVal, normalizedVal))
        {
            return normalizedVal;
        }

        return currVal;
    }

    float GetSerialValue(String inputString, float currVal, int32_t minVal, int32_t maxVal)
    {
        if (inputString[0] == '#' && inputString.length() >= 10)
        {
            String valString = inputString.substring(5, 9);
            int val = valString.toInt();

            // A value of 0 indicates an error but is acceptable if the intended value is actually 0.
            if (val != 0 || valString.equals("0000"))
            {
                return val;
            }
        }

        return currVal;
    }

    // Because an analog read can waiver between values, we need to determine if an analog value has changed enough
    // for us to do anything about.
    bool HasChanged(float oldVal, float newVal)
    {
        // If the value has changed to the min or max, return true regardless of tolerance.
        // if ((oldVal != minVal && newVal == minVal) || (oldVal != maxVal && newVal == maxVal))
        // {
        //     return true;
        // }
        float normalizedOldVal = map(oldVal, 0, 1023, 0, 1023);
        float normalizedNewVal = map(newVal, 0, 1023, 0, 1023);

        // Otherwise, check that the value has surpassed the tolerance threshold.
        return normalizedNewVal <= (normalizedOldVal - m_analogTolerance) || normalizedNewVal >= (normalizedOldVal + m_analogTolerance);
    }

    // Because an analog read can waiver between values, we need to determine if an analog value has changed enough
    // for us to do anything about.
    // bool HasChanged(float oldVal, float newVal)
    // {
    //     // If the value has changed to the min or max, return true regardless of tolerance.
    //     // if ((oldVal != minVal && newVal == minVal) || (oldVal != maxVal && newVal == maxVal))
    //     // {
    //     //     return true;
    //     // }
    //     // Otherwise, check that the value has surpassed the tolerance threshold.
    //     return newVal <= (oldVal - m_analogTolerance) || newVal >= (oldVal + m_analogTolerance);
    // }

    void setBrightness()
    {
        gBrightness = getAnalogValue(gBrightness, 0, 100);
        LEDS.setBrightness(gBrightness);

        #if DEBUG
            Serial.print("Setting Brightness: ");
            Serial.println(gBrightness);
        #endif
    }

    void setInverse()
    {
        gInverse = getBoolValue();

        #if DEBUG
            Serial.print("Setting Inverse: ");
            Serial.println(gInverse);
        #endif
    }

    void setFps()
    {
        gFps = getAnalogValue(gFps, 500, 5000);

        #if DEBUG
            Serial.print("Setting FPS: ");
            Serial.println(gFps);
        #endif
    }

    void setHue()
    {
        int val = 0;
        if (gHue1) val++;
        if (gHue2) val++;
        if (gHue3) val++;
        if (gHue4) val++;
        if (gHue5) val++;

        gHue = map(val, 0, 6, 0, 255);

        #if DEBUG
            Serial.print("Setting Hue: ");
            Serial.println(gHue);
        #endif
    }

    void setFade()
    {
        gFade = getAnalogValue(gFade, 3, 100);

        #if DEBUG
            Serial.print("Setting Fade: ");
            Serial.println(gFade);
        #endif
    }

    void setPosition()
    {
        gPos = getAnalogValue(gPos, 0, 500);

        #if DEBUG
            Serial.print("Setting Position: ");
            Serial.println(gPos);
        #endif
    }

    void setPause(bool isPaused)
    {
        gPause = isPaused;

        #if DEBUG
            Serial.print("Setting Pause: ");
            Serial.println(gPause);
        #endif
    }

    #pragma region PATTERNS

    void setAllColor(CRGB ledSet[], int numLeds, CRGB::HTMLColorCode color)
    {
        for (int i = 0; i < numLeds; i++)
        {
            ledSet[i] = color;
        }
    }

    void setColor(CRGB *ledSet[], int numLeds, CRGB::HTMLColorCode color)
    {
        for (int i = 0; i < numLeds; i++)
        {
            *getLed(ledSet, i) = color;
        }
    }

    void rainbow(CRGB *ledSet[], int numLeds)
    {
        fillRainbow(ledSets, numLed, gHue + gPos, 7);
    }

    void rainbowWithGlitter(CRGB *ledSet[], int numLeds)
    {
        rainbow(ledSet, numLeds);

        addGlitter(80, ledSet, numLeds);
    }

    void addGlitter(fract8 chanceOfGlitter, CRGB *ledSet[], int numLeds)
    {
        if (random8() < chanceOfGlitter)
        {
            uint8_t pos = random16(numLeds);
            *getLed(ledSet, pos) += CRGB::White;
        }
    }

    void confetti(CRGB *ledSet[], int numLeds)
    {
        // Random colored speckles that blink in and fade smoothly.
        fadeSetsToBlackBy(ledSet, numLeds, 1);

        int randLed = random16(numLeds);
        uint8_t pos = getPosition(randLed, numLeds);

        *getLed(ledSet, pos) += CHSV(gHue + random8(64), 200, 255);
    }

    void sinelon(CRGB *ledSet[], int numLeds)
    {
        fadeSetsToBlackBy(ledSet, numLeds, 2);

        uint8_t beat = beatsin16(13, 0, numLeds - 1);
        uint8_t pos = getPosition(beat, numLeds);

        *getLed(ledSet, pos) += CHSV(gHue, 255, 192);
    }

    void bpm(CRGB *ledSet[], int numLeds)
    {
        // Colored stripes pulsing at a defined Beats-Per-Minute (BPM).
        CRGBPalette16 palette = PartyColors_p;
        uint8_t bpm = 62;
        uint8_t beat = beatsin8(bpm, 64, 255);

        for (int i = 0; i < numLeds; i++)
        {
            uint8_t pos = getPosition(i, numLeds);
            *getLed(ledSet, pos) = ColorFromPalette(palette, gHue + (i * 2), beat - gHue + (i * 10));
        }
    }

    void juggle(CRGB *ledSet[], int numLeds)
    {
        // Eight colored dots, weaving in and out of sync with each other.
        fadeSetsToBlackBy(ledSet, numLeds, 2);

        byte dothue = 0;
        for (int i = 0; i < 8; i++)
        {
            uint8_t beat = beatsin16(i + 7, 0, numLeds - 1);
            uint8_t pos = getPosition(beat, numLeds);

            *getLed(ledSet, pos) |= CHSV(dothue, 200, 255);
        }
        dothue += 32;
    }

    void invert()
    {
        // if (gInverse)
        // {
        //     for (int i = 0; i < NUM_PENT_LEDS; i++)
        //     {
        //         leds[i] = -leds[i];
        //     }
        // }
    }

    void fadeSetsToBlackBy(CRGB *ledSet[], uint16_t numLeds, uint8_t fade)
    {
        for( int i = 0; i < numLeds; i++) 
        {
            uint8_t scale = 255 - (fade * gFade);
            (*ledSet[i]).nscale8(scale);
        }
    }

    void fillRainbow(CRGB *ledSet[], uint16_t numLeds, uint8_t initialHue, uint8_t deltaHue)
    {
        CHSV hsv;
        hsv.hue = initialHue;
        hsv.val = 255;
        hsv.sat = 240;
        for( int i = 0; i < numLeds; i++) 
        {
            *ledSet[i] = hsv;
            hsv.hue += deltaHue;
        }
    }

    int getPosition(int pos, int numLeds)
    {
        return (pos + (int)gPos) % numLeds;
    }

    CRGB* getLed(CRGB *ledSets[], int pos)
    {
        return ledSets[pos];
    }

    #pragma endregion PATTERNS
};

extern CMeltdownLED MeltdownLED;