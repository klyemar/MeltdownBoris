#include <Arduino.h>
#include <FastLED.h>

#define ARRAY_SIZE(A) (sizeof(A) / sizeof((A)[0]))

class CMeltdownLED
{
    public: CMeltdownLED();

    uint8_t m_analogTolerance;

    // Serial input commands.
    String m_inputString;
    boolean m_inputStringComplete; // whether the String is complete

    // Global LED values.
    uint8_t gCurrentPatternNumber = 0; // Index number of which pattern is current
    uint8_t gBrightness = 48;
    uint8_t gHue = 0;                  // rotating "base color" used by many of the patterns
    uint16_t gFps = 500;
    float gPos = 0;
    float gFade = 20;
    bool gHue1 = false;
    bool gHue2 = false;
    bool gHue3 = false;
    bool gHue4 = false;
    bool gHue5 = false;
    bool gInverse = false;
    bool gPause = false;

    // List of patterns to cycle through.  Each is defined as a separate function below.
    typedef void (CMeltdownLED::*SimplePatternList)(CRGB*[], int);
    SimplePatternList gPatterns[6] = {&CMeltdownLED::Rainbow, &CMeltdownLED::RainbowWithGlitter, &CMeltdownLED::Confetti, &CMeltdownLED::Sinelon, &CMeltdownLED::Bpm, &CMeltdownLED::Juggle};

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

    int GetNextPatternNumber()
    {
        // TODO - Fix this.
        // return (gCurrentPatternNumber + 1) % ARRAY_SIZE(gPatterns);
        return (gCurrentPatternNumber + 1) % 6;
    }

    void NextPattern()
    {
        // add one to the current pattern number, and wrap around at the end
        gCurrentPatternNumber = GetNextPatternNumber();
    }

    void ExecutePattern(CRGB *ledSet[], int numLeds)
    {       
        (this->*(gPatterns[gCurrentPatternNumber]))(ledSet, numLeds);
    }

    uint8_t SetBrightness(uint8_t pin)
    {
        int32_t minVal = 0;
        int32_t maxVal = 100;
        float currVal = gBrightness;

        gBrightness = GetAnalogValue(pin, currVal, minVal, maxVal);
        LEDS.setBrightness(gBrightness);

        return gBrightness;
    }

    uint8_t GetBrightness()
    {
        return gBrightness;
    }

    bool ToggleInverse()
    {
        gInverse = !gInverse;
        
        return gInverse;
    }

    void SetInverse()
    {
        gInverse = GetBoolValue();
    }

    bool GetInverse()
    {
        return gInverse;
    }

    void SetFps()
    {
        gFps = GetAnalogValue(gFps, 500, 5000);
    }

    uint16_t GetFps()
    {
        return gFps;
    }

    void IncrementHue(int factor)
    {
        gHue += factor;
    }

    bool ToggleHue(uint8_t index)
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

    uint8_t GetHue()
    {
        return gHue;
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

    float SetFade(uint8_t pin)
    {
        int32_t minVal = 3;
        int32_t maxVal = 100;
        float currVal = gFade;

        if (pin > -1)
        {
            gFade = GetAnalogValue(pin, currVal, minVal, maxVal);
        }
        else
        {
            gFade = GetAnalogValue(currVal, minVal, maxVal);
        }

        return gFade;
    }

    float GetFade()
    {
        return gFade;
    }

    float SetPosition(uint8_t pin)
    {
        int32_t minVal = 0;
        int32_t maxVal = 500;
        float currVal = gPos;

        if (pin > -1)
        {
            gPos = GetAnalogValue(pin, currVal, minVal, maxVal);
        }
        else
        {
            gPos = GetAnalogValue(currVal, minVal, maxVal);
        }

        return gPos;
    }

    float GetPosition()
    {
        return gPos;
    }

    bool TogglePause()
    {
        gPause = !gPause;

        return gPause;
    }

    void SetPause()
    {
        gPause = GetBoolValue();
    }

    bool GetPause()
    {
        return gPause;
    }

    #pragma region PATTERNS

    void SetAllColor(CRGB ledSet[], int numLeds, CRGB::HTMLColorCode color)
    {
        for (int i = 0; i < numLeds; i++)
        {
            ledSet[i] = color;
        }
    }

    void SetColor(CRGB *ledSet[], int numLeds, CRGB::HTMLColorCode color)
    {
        for (int i = 0; i < numLeds; i++)
        {
            *GetLed(ledSet, i) = color;
        }
    }

    void Rainbow(CRGB *ledSet[], int numLeds)
    {
        FillRainbow(ledSet, numLeds, gHue + gPos, 7);
    }

    void RainbowWithGlitter(CRGB *ledSet[], int numLeds)
    {
        Rainbow(ledSet, numLeds);

        AddGlitter(80, ledSet, numLeds);
    }

    void AddGlitter(fract8 chanceOfGlitter, CRGB *ledSet[], int numLeds)
    {
        if (random8() < chanceOfGlitter)
        {
            uint8_t pos = random16(numLeds);
            *GetLed(ledSet, pos) += CRGB::White;
        }
    }

    void Confetti(CRGB *ledSet[], int numLeds)
    {
        // Random colored speckles that blink in and fade smoothly.
        FadeSetsToBlackBy(ledSet, numLeds, 1);

        int randLed = random16(numLeds);
        uint8_t pos = GetPosition(randLed, numLeds);

        *GetLed(ledSet, pos) += CHSV(gHue + random8(64), 200, 255);
    }

    void Sinelon(CRGB *ledSet[], int numLeds)
    {
        FadeSetsToBlackBy(ledSet, numLeds, 2);

        uint8_t beat = beatsin16(13, 0, numLeds - 1);
        uint8_t pos = GetPosition(beat, numLeds);

        *GetLed(ledSet, pos) += CHSV(gHue, 255, 192);
    }

    void Bpm(CRGB *ledSet[], int numLeds)
    {
        // Colored stripes pulsing at a defined Beats-Per-Minute (BPM).
        CRGBPalette16 palette = PartyColors_p;
        uint8_t bpm = 62;
        uint8_t beat = beatsin8(bpm, 64, 255);

        for (int i = 0; i < numLeds; i++)
        {
            uint8_t pos = GetPosition(i, numLeds);
            *GetLed(ledSet, pos) = ColorFromPalette(palette, gHue + (i * 2), beat - gHue + (i * 10));
        }
    }

    void Juggle(CRGB *ledSet[], int numLeds)
    {
        // Eight colored dots, weaving in and out of sync with each other.
        FadeSetsToBlackBy(ledSet, numLeds, 2);

        byte dothue = 0;
        for (int i = 0; i < 8; i++)
        {
            uint8_t beat = beatsin16(i + 7, 0, numLeds - 1);
            uint8_t pos = GetPosition(beat, numLeds);

            *GetLed(ledSet, pos) |= CHSV(dothue, 200, 255);
        }
        dothue += 32;
    }

    void Invert()
    {
        // if (gInverse)
        // {
        //     for (int i = 0; i < NUM_PENT_LEDS; i++)
        //     {
        //         leds[i] = -leds[i];
        //     }
        // }
    }

    void FadeSetsToBlackBy(CRGB *ledSet[], uint16_t numLeds, uint8_t fade)
    {
        for( int i = 0; i < numLeds; i++) 
        {
            uint8_t scale = 255 - (fade * gFade);
            (*ledSet[i]).nscale8(scale);
        }
    }

    void FillRainbow(CRGB *ledSet[], uint16_t numLeds, uint8_t initialHue, uint8_t deltaHue)
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

    int GetPosition(int pos, int numLeds)
    {
        return (pos + (int)gPos) % numLeds;
    }

    CRGB* GetLed(CRGB *ledSet[], int pos)
    {
        return ledSet[pos];
    }

    #pragma endregion PATTERNS

    #pragma region INPUTS

    float GetAnalogValue(float currVal, int32_t minVal, int32_t maxVal)
    {
        float val = currVal;
        
        if (!m_inputString.equals(""))
        {
            val = GetSerialValue(m_inputString, currVal, minVal, maxVal);
        }

        return val;
    }

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

    String GetInputString()
    {
        return m_inputString;
    }

    void ClearInputString()
    {
        m_inputString = "";
    }

    void AddCharToInputString(char inChar)
    {
        m_inputString += inChar;
    }

    bool GetInputStringComplete()
    {
        return m_inputStringComplete;
    }

    void SetInputStringComplete(bool value)
    {
        m_inputStringComplete = value;
    }

    String GetCommand()
    {
        if (m_inputString[0] == '#' && m_inputString.length() >= 6)
        {
            return m_inputString.substring(1, 5);
        }
        return "";
    }

    int GetValue()
    {    
        int val = 0;
        if (m_inputString[0] == '#' && m_inputString.length() >= 10)
        {
            String valString = m_inputString.substring(5, 9);
            val = valString.toInt();
        }
        return val;
    }

    bool GetBoolValue()
    {
        int val = GetValue();
        return val != 0 ? true : false;
    }

    #pragma endregion INPUTS

    #pragma region COMMANDS

    String PrepareBoolCommand(String command, bool value)
    {
        return PrepareCommand(command, value ? 1 : 0);
    }

    String PrepareCommand(String command, float value)
    {
        if (value < 0) value = 0;
        if (value > 9999) value = 9999;

        String valueString = "";
        if (value < 1000) valueString += "0";
        if (value < 100) valueString += "0";
        if (value < 10) valueString += "0";
        valueString += (uint16_t)value;

        return "#" + command + valueString + "\n";
    }

    #pragma endregion COMMANDS
};

extern CMeltdownLED MeltdownLED;