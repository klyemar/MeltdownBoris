#include <Arduino.h>
#include <FastLED.h>
#include <MeltdownSerial.h>

#ifndef MELTDOWN_LED
#define MELTDOWN_LED

#define ARRAY_SIZE(A) (sizeof(A) / sizeof((A)[0]))

class CMeltdownLED
{
    public: CMeltdownLED();

    // Global LED values.
    int gCurrentPatternNumber = 0;
    int gCurrentEffectNumber = 0;
    int gCurrentModeNumber = 0;

    int gBrightness = 48;
    int gHue = 0;                  
    int gFps = 500;
    long gAnalogPattern = 0;
    long gAnalogEffect = 0;
    long gPos = 0;
    long gFade = 20;
    bool gHue1 = false;
    bool gHue2 = false;
    bool gHue3 = false;
    bool gHue4 = false;
    bool gHue5 = false;
    bool gInverse = false;
    bool gGlitter = false;
    bool gPause = false;
    bool gTop = false;
    bool gBottom = false;
    bool gSleeping = false;

    unsigned long sleepStartMillis;
    unsigned long currentMillis;
    const unsigned long sleepPeriod = 1000L * 60 * 30; // 30 minutes

    // List of patterns to cycle through.  Each is defined as a separate function below.
    typedef void (CMeltdownLED::*SimplePatternList)(CRGB*[], int, int);
    SimplePatternList gPatterns[5] = {
        &CMeltdownLED::Rainbow, 
        &CMeltdownLED::Confetti, 
        &CMeltdownLED::Sinelon, 
        &CMeltdownLED::Bpm, 
        &CMeltdownLED::Juggle
    };

    // List of modes to cycle through.  Each is defined as a separate function below.
    typedef void (CMeltdownLED::*ModeList)(CRGB*[], int);
    ModeList gEffects[2] = {
        &CMeltdownLED::NullEffect, 
        &CMeltdownLED::GlitterEffect
    };

    // ANALOG //

    int SetAnalogPattern(int pin)
    {
        gAnalogPattern = MeltdownSerial.GetAnalogValue(pin, gAnalogPattern);
        return gAnalogPattern;
    }
    int GetAnalogPattern() { return gAnalogPattern; }

    int GetAnalogPattern(int minVal, int maxVal) { return map(gAnalogPattern, 0, 1023, minVal, maxVal); }

    int SetAnalogEffect(int pin)
    {
        gAnalogEffect = MeltdownSerial.GetAnalogValue(pin, gAnalogEffect);
        return gAnalogEffect;
    }
    int GetAnalogEffect() { return gAnalogEffect; }
    
    int GetAnalogEffect(int minVal, int maxVal) { return map(gAnalogEffect, 0, 1023, minVal, maxVal); }

    // BRIGHTNESS //

    int SetBrightness(int pin)
    {
        gBrightness = MeltdownSerial.GetAnalogValue(pin, gBrightness);
        LEDS.setBrightness(gBrightness);

        return gBrightness;
    }
    int GetBrightness() { return gBrightness; }

    int GetBrightness(int minVal, int maxVal) { return map(gBrightness, 0, 1023, minVal, maxVal); }

    // FPS //

    int GetFps() { return gFps; }

    // HUE //

    bool ToggleHue(int index)
    {
        bool hueVal = false;
        switch(index)
        {
            case 1:
                gHue1 = !gHue1;
                hueVal = gHue1;
            case 2:
                gHue2 = !gHue2;
                hueVal = gHue2;
            case 3:
                gHue3 = !gHue3;
                hueVal = gHue3;
            case 4:
                gHue4 = !gHue4;
                hueVal = gHue4;
            case 5:
                gHue5 = !gHue5;
                hueVal = gHue5;
        }
        SetHue();

        return hueVal;
    }
    void SetHue()
    {
        int val = 0;
        if (gHue1) val++;
        if (gHue2) val++;
        if (gHue3) val++;
        if (gHue4) val++;
        if (gHue5) val++;

        gHue = map(val, 0, 6, 0, 255);
    }  
    int GetHue() { return gHue; }

    // PAUSE //

    bool TogglePause()
    {
        gPause = !gPause;
        return gPause;
    }
    void SetPause() { gPause = MeltdownSerial.GetBoolValue(); }

    bool GetPause() { return gPause; }

    // SLEEP //

    bool SetSleeping(bool isSleeping)
    {
        gSleeping = isSleeping;
        // If we're waking up, reset the timer.
        if (!gSleeping)
        {
            sleepStartMillis = millis();
        }
        return gSleeping;
    }
    void SetSleeping() { gSleeping = MeltdownSerial.GetBoolValue(); }

    bool GetSleeping() { return gSleeping; }

    // POSITION //

    bool ToggleTop()
    {
        gBottom = false;
        gTop = !gTop;
        return gTop;
    }
    void SetTop() { gTop = MeltdownSerial.GetBoolValue(); }

    bool GetTop() { return gTop; }

    bool ToggleBottom()
    {
        gTop = false;
        gBottom = !gBottom;
        return gBottom;
    }
    void SetBottom() { gBottom = MeltdownSerial.GetBoolValue(); }

    bool GetBottom() { return gBottom; }

    #pragma region EFFECTS

    int GetEffectNumber(int offset = 0) { return (gCurrentEffectNumber + offset) % ARRAY_SIZE(gEffects); }

    void NextEffect() { gCurrentEffectNumber = GetEffectNumber(1); }

    void ExecuteEffect(CRGB *ledSet[], int numLeds, int offset = 0) { (this->*(gEffects[GetEffectNumber(offset)]))(ledSet, numLeds); }

    void NullEffect(CRGB *ledSet[], int numLeds) { }

    void GlitterEffect(CRGB *ledSet[], int numLeds)
    {
        int chanceOfGlitter = GetAnalogEffect(15, 115);
        if (random8() < chanceOfGlitter)
        {
            SetRandomColor(ledSet, numLeds, 3, CRGB::White);
        }
    }

    void HueIncrementEffect(CRGB *ledSet[], int numLeds)
    {
        static int hue = 0;
        
        int effectVal = GetAnalogEffect(100, 500);
        EVERY_N_MILLIS (effectVal)
        {
            for (int i = 0; i < numLeds; i++)
            {
                // *ledSet[i]
            }
        }
    }

    #pragma endregion EFFECTS

    #pragma region MODES

    int GetModeNumber(int numModes, int offset = 0) { return (gCurrentModeNumber + offset) % (numModes + 1); }

    int NextMode() { return gCurrentModeNumber++; }

    void Invert(CRGB *ledSet[], int numLeds)
    {
        for (int i = 0; i < numLeds; i++)
        {
            *ledSet[i] = -*ledSet[i];
        }
    }

    void MaximizeBrightness(CRGB *ledSet[], int numLeds)
    {
        for (int i = 0; i < numLeds; i++)
        {
            (*ledSet[i]).maximizeBrightness();
        }
    }

    #pragma endregion MODES

    #pragma region TIMERS

    void InitTimers()
    {
        sleepStartMillis = millis();
    }

    bool IsTimerOver(unsigned long period, unsigned long timerMillis)
    {
        currentMillis = millis();
        return currentMillis - timerMillis >= period;
    }

    // Returns true as soon as the sleep timer has been exceeded.
    bool CheckSleepTimer()
    {
        // Skip if already sleeping.
        if (!GetSleeping()) 
        {
            if (IsTimerOver(sleepPeriod, sleepStartMillis))
            {
                return SetSleeping(true);
            }
        }
        return false;
    }

    #pragma endregion TIMERS

    #pragma region PATTERNS

    int GetPatternNumber(int offset = 0) { return (gCurrentPatternNumber + offset) % ARRAY_SIZE(gPatterns); }

    void NextPattern() { gCurrentPatternNumber = GetPatternNumber(1); }

    void ExecutePattern(CRGB *ledSet[], int numLeds, int patternOffset = 0, int modeOffset = 0) { (this->*(gPatterns[GetPatternNumber(patternOffset)]))(ledSet, numLeds, modeOffset); }

    void SetAllColor(CRGB *ledSet[], int numLeds, CRGB::HTMLColorCode color)
    {
        for (int i = 0; i < numLeds; i++)
        {
            *ledSet[i] = color;
        }
    }

    void SetAllColor(CRGB *ledSet[], int numLeds, CRGB color)
    {
        for (int i = 0; i < numLeds; i++)
        {
            *ledSet[i] = color;
        }
    }

    void SetRandomColor(CRGB *ledSet[], int numLeds, int numPositions, int hueOffset = 0)
    {
        for (int i = 0; i < numPositions; i++)
        {
            *ledSet[random16(numLeds)] += CHSV(gHue + random8(hueOffset), 200, 255);
        }
    }

    void SetRandomColor(CRGB *ledSet[], int numLeds, int numPositions, CRGB::HTMLColorCode color)
    {
        for (int i = 0; i < numPositions; i++)
        {
            *ledSet[random16(numLeds)] += color;
        }
    }


    void Rainbow(CRGB *ledSet[], int numLeds, int modeOffset = 0)
    {
        int deltaHue = GetAnalogPattern(1, 15);
        FillRainbow(ledSet, numLeds, gHue + gPos, deltaHue);

        // Modes
        int numModes = 1;
        switch (GetModeNumber(numModes, modeOffset))
        {
            case 1:
                // Invert rainbox colors.
                Invert(ledSet, numLeds);
                break;
        }
    }

    void Confetti(CRGB *ledSet[], int numLeds, int modeOffset = 0)
    {
        int fade = GetAnalogPattern(2, 30);
        FadeSetsToBlackBy(ledSet, numLeds, fade);

        // Modes
        int numModes = 2;
        switch (GetModeNumber(numModes, modeOffset))
        {
            case 1:
                SetRandomColor(ledSet, numLeds, 12, 64);
                break;
            case 2:
                SetRandomColor(ledSet, numLeds, 18, 64);
                break;
            default:
                SetRandomColor(ledSet, numLeds, 6, 64);
                break;
        }
    }

    void Sinelon(CRGB *ledSet[], int numLeds, int modeOffset = 0)
    {
        // Display a dot moving back and forth in a sin wave pattern.
        int fade = GetAnalogPattern(2, 30);
        FadeSetsToBlackBy(ledSet, numLeds, fade);

        int pos = beatsin16(8, 0, numLeds - 1);
        *ledSet[pos] += CHSV(gHue + 64, 255, 192);

        // Modes
        int numModes = 4;
        switch (GetModeNumber(numModes, modeOffset))
        {
            case 1:
                {
                    // Add a second, opposite dot moving in the opposite direction.
                    int opppositePos = (numLeds - 1) - (beatsin16(8, 0, numLeds - 1));
                    *ledSet[opppositePos] += CHSV(gHue + 128, 255, 192);
                }
                break;
            case 2:
                GenerateSinelons(ledSet, numLeds, 2, pos);
                break;
            case 3:
                GenerateSinelons(ledSet, numLeds, 3, pos);
                break;
            case 4:
                GenerateSinelons(ledSet, numLeds, 5, pos);
                break;
        }
    }

    void GenerateSinelons(CRGB *ledSet[], int numLeds, int numSinelons, int pos)
    {
        for (int i = 0; i < numSinelons; i++)
        {
            int hue = gHue + (int)((255 / numSinelons) * i);
            
            *ledSet[(pos + (int)(numLeds / numSinelons) * (i + 1)) % numLeds] += CHSV(hue, 255, 192);
        }
    }

    void Bpm(CRGB *ledSet[], int numLeds, int modeOffset = 0)
    {
        // Colored stripes pulsing at a defined Beats-Per-Minute (BPM).
        CRGBPalette16 palette;

        // Modes, select palette color.
        int numModes = 4;
        switch (GetModeNumber(numModes, modeOffset))
        {
            case 1:
                palette = ForestColors_p;
                break;
            case 2:
                palette = CloudColors_p;
                break;
            case 3:
                palette = HeatColors_p;
                break;
            case 4:
                palette = RainbowColors_p;
                break;
            default:
                palette = PartyColors_p;
                break;
        }


        int bpm = 60;
        int beat = beatsin8(bpm, 63, 255);
        int multiplier = GetAnalogPattern(2, 12);
        for (int i = 0; i < numLeds; i++)
        {
            *ledSet[i] = ColorFromPalette(palette, gHue + (i * multiplier), beat - gHue + (i * 10));
        }
    }

    void Juggle(CRGB *ledSet[], int numLeds, int modeOffset = 0)
    {
        // Eight colored dots, weaving in and out of sync with each other.
        int fade = GetAnalogPattern(3, 100);
        FadeSetsToBlackBy(ledSet, numLeds, fade);

        int numBalls = 3;

        // Modes
        int numModes = 3;
        switch (GetModeNumber(numModes, modeOffset))
        {
            case 1:
                numBalls = 5;
                break;
            case 2:
                numBalls = 7;
                break;
            case 3:
                numBalls = 9;
                break;
        }

        for (int i = 0; i < numBalls; i++)
        {
            int dotHue = i * (255 / numBalls);
            int pos = beatsin16(i + 7, 0, numLeds - 1);

            *ledSet[pos] |= CHSV(dotHue + gHue, 200, 255);
        }
    }

    void Sunrise(CRGB *ledSet[], int numLeds, int modeOffset = 0)
    {      
        static const uint8_t sunriseMinutes = 8;

        static uint8_t heatIndex = 0;
        CRGB color = ColorFromPalette(HeatColors_p, (heatIndex % 255));

        SetAllColor(ledSet, numLeds, color);

        static const uint8_t interval = (sunriseMinutes * 60) / 256;
        EVERY_N_SECONDS(interval) {
            heatIndex++;
        }
    }

    void FadeSetsToBlackBy(CRGB *ledSet[], int numLeds, int fade)
    {
        for( int i = 0; i < numLeds; i++) 
        {
            int scale = 255 - fade;
            (*ledSet[i]).nscale8(scale);
        }
    }

    void FillRainbow(CRGB *ledSet[], int numLeds, int initialHue, int deltaHue)
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

    #pragma endregion PATTERNS
};

extern CMeltdownLED MeltdownLED;

#endif