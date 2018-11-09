#include <Arduino.h>
#include <FastLED.h>

#define ARRAY_SIZE(A) (sizeof(A) / sizeof((A)[0]))

class CMeltdownLED
{
    public: CMeltdownLED();

    int m_analogTolerance;

    // Serial input commands.
    String m_inputString;
    boolean m_inputStringComplete; // whether the String is complete

    // Global LED values.
    int gCurrentPatternNumber = 0;
    int gCurrentModeNumber = 0;

    int gBrightness = 48;
    int gHue = 0;                  
    int gFps = 500;
    long gAnalogPattern = 0;
    long gAnalogMode = 0;
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

    // List of patterns to cycle through.  Each is defined as a separate function below.
    typedef void (CMeltdownLED::*SimplePatternList)(CRGB*[], int);
    SimplePatternList gPatterns[5] = {
        &CMeltdownLED::Rainbow, 
        &CMeltdownLED::Confetti, 
        &CMeltdownLED::Sinelon, 
        &CMeltdownLED::Bpm, 
        &CMeltdownLED::Juggle
    };

    // List of modes to cycle through.  Each is defined as a separate function below.
    typedef void (CMeltdownLED::*ModeList)(CRGB*[], int);
    ModeList gModes[3] = {
        &CMeltdownLED::NullMode, 
        &CMeltdownLED::GlitterMode, 
        &CMeltdownLED::InvertMode
    };

    // ANALOG //

    int SetAnalogPattern(int pin)
    {
        gAnalogPattern = GetAnalogValue(pin, gAnalogPattern);
        return gAnalogPattern;
    }
    int GetAnalogPattern() { return gAnalogPattern; }
    int GetAnalogPattern(int minVal, int maxVal) { return map(gAnalogPattern, 0, 1023, minVal, maxVal); }

    int SetAnalogMode(int pin)
    {
        gAnalogMode = GetAnalogValue(pin, gAnalogMode);
        return gAnalogMode;
    }
    int GetAnalogMode() { return gAnalogMode; }
    int GetAnalogMode(int minVal, int maxVal) { return map(gAnalogMode, 0, 1023, minVal, maxVal); }

    // BRIGHTNESS //

    int SetBrightness(int pin)
    {
        gBrightness = GetAnalogValue(pin, gBrightness);
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
    void SetPause() { gPause = GetBoolValue(); }
    bool GetPause() { return gPause; }

    #pragma region MODES

    int GetNextModeNumber()
    {
        // TODO - Fix this.
        // return (gCurrentPatternNumber + 1) % ARRAY_SIZE(gPatterns);
        return (gCurrentModeNumber + 1) % 3;
    }

    void NextMode()
    {
        gCurrentModeNumber = GetNextModeNumber();
    }

    void ExecuteMode(CRGB *ledSet[], int numLeds)
    {       
        (this->*(gModes[gCurrentModeNumber]))(ledSet, numLeds);
    }

    void NullMode(CRGB *ledSet[], int numLeds) { }

    void GlitterMode(CRGB *ledSet[], int numLeds)
    {
        int chanceOfGlitter = GetAnalogMode(0, 100);
        if (random8() < chanceOfGlitter)
        {
            int pos = random16(numLeds);
            *GetLed(ledSet, pos) += CRGB::White;
        }
    }

    void InvertMode(CRGB *ledSet[], int numLeds)
    {
        for (int i = 0; i < numLeds; i++)
        {
            *ledSet[i] = -*ledSet[i];
        }
    }

    #pragma endregion MODES

    #pragma region PATTERNS

    int GetNextPatternNumber()
    {
        // TODO - Fix this.
        // return (gCurrentPatternNumber + 1) % ARRAY_SIZE(gPatterns);
        return (gCurrentPatternNumber + 1) % 5;
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
        int deltaHue = GetAnalogPattern(1, 15);
        FillRainbow(ledSet, numLeds, gHue + gPos, deltaHue);
    }

    void Confetti(CRGB *ledSet[], int numLeds)
    {
        int fade = GetAnalogPattern(2, 30);
        FadeSetsToBlackBy(ledSet, numLeds, fade);

        int randLed = random16(numLeds);
        *GetLed(ledSet, randLed) += CHSV(gHue + random8(64), 200, 255);
    }

    void Sinelon(CRGB *ledSet[], int numLeds)
    {
        int fade = GetAnalogPattern(2, 30);
        FadeSetsToBlackBy(ledSet, numLeds, fade);

        int pos = beatsin16(13, 0, numLeds - 1);
        *GetLed(ledSet, pos) += CHSV(gHue, 255, 192);
    }

    void Bpm(CRGB *ledSet[], int numLeds)
    {
        // Colored stripes pulsing at a defined Beats-Per-Minute (BPM).
        CRGBPalette16 palette = PartyColors_p;

        int bpm = 60;
        int lower = GetAnalogPattern(0, 255);

        int beat = beatsin8(bpm, lower, 255);
        for (int i = 0; i < numLeds; i++)
        {
            *GetLed(ledSet, i) = ColorFromPalette(palette, gHue + (i * 2), beat - gHue + (i * 10));
        }
    }

    void Juggle(CRGB *ledSet[], int numLeds)
    {
        // Eight colored dots, weaving in and out of sync with each other.
        int fade = GetAnalogPattern(3, 100);
        FadeSetsToBlackBy(ledSet, numLeds, fade);

        byte dothue = 0;
        for (int i = 0; i < 8; i++)
        {
            int beat = beatsin16(i + 7, 0, numLeds - 1);
            int pos = CalculatePosition(beat, numLeds);

            *GetLed(ledSet, pos) |= CHSV(dothue, 200, 255);
            dothue += 32;
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

    int CalculatePosition(int pos, int numLeds)
    {
        return (pos + gPos) % numLeds;
    }

    CRGB* GetLed(CRGB *ledSet[], int pos)
    {
        return ledSet[pos];
    }

    #pragma endregion PATTERNS

    #pragma region INPUTS

    int GetAnalogValue(int currVal)
    {
        int val = currVal;
        
        if (!m_inputString.equals(""))
        {
            val = GetSerialValue(m_inputString, currVal);
        }

        return val;
    }

    int GetAnalogValue(int pin, int currVal)
    {
        int val = analogRead(pin);
        if (HasChanged(currVal, val))
        {
            return val;
        }

        return currVal;
    }

    int GetSerialValue(String inputString, int currVal)
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
    bool HasChanged(int oldVal, int newVal)
    {
        // Otherwise, check that the value has surpassed the tolerance threshold.
        return newVal <= (oldVal - m_analogTolerance) || newVal >= (oldVal + m_analogTolerance);
    }

    String GetInputString() { return m_inputString; }

    void ClearInputString() { m_inputString = ""; }

    void AddCharToInputString(char inChar) { m_inputString += inChar; }

    bool GetInputStringComplete() { return m_inputStringComplete; }

    void SetInputStringComplete(bool value) { m_inputStringComplete = value; }

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

    String PrepareBoolCommand(String command, bool value) { return PrepareCommand(command, value ? 1 : 0); }

    String PrepareCommand(String command, int value)
    {
        if (value < 0) value = 0;
        if (value > 9999) value = 9999;

        String valueString = "";
        if (value < 1000) valueString += "0";
        if (value < 100) valueString += "0";
        if (value < 10) valueString += "0";
        valueString += value;

        return "#" + command + valueString + "\n";
    }

    #pragma endregion COMMANDS
};

extern CMeltdownLED MeltdownLED;