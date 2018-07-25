#include "FastLED.h"
#include "MeltdownLED.h"

FASTLED_USING_NAMESPACE

// Boris, short for Explora Borealis, is an interactive light display
// controlled by several individual user inputs.
//
// This project builds off of the FastLED library and some of its existing patterns
// and examples.

#define DATA_PIN_1 2
#define DATA_PIN_2 3
#define DATA_PIN_3 4
#define DATA_PIN_4 5

#define PATTERN_PIN 13
#define BRIGHTNESS_PIN -1
#define FPS_PIN -1
#define HUE_PIN -1

#define LED_TYPE WS2812B
#define NUM_LEDS_WHEEL 69
#define NUM_LEDS_SPOKE 60
#define NUM_LEDS_STRIP (NUM_LEDS_WHEEL + NUM_LEDS_SPOKE)
#define NUM_SETS 4
#define NUM_WHEEL_SETS_LEDS (NUM_SETS * NUM_LEDS_WHEEL)
#define NUM_SPOKE_SETS_LEDS (NUM_SETS * NUM_LEDS_SPOKE)
CRGBArray<NUM_LEDS_STRIP> leds1;
CRGBArray<NUM_LEDS_STRIP> leds2;
CRGBArray<NUM_LEDS_STRIP> leds3;
CRGBArray<NUM_LEDS_STRIP> leds4;

CRGBSet wheelLeds1 = leds1(0, NUM_LEDS_WHEEL - 1);
CRGBSet wheelLeds2 = leds2(0, NUM_LEDS_WHEEL - 1);
CRGBSet wheelLeds3 = leds3(0, NUM_LEDS_WHEEL - 1);
CRGBSet wheelLeds4 = leds4(0, NUM_LEDS_WHEEL - 1);

CRGBSet spokeLeds1 = leds1(NUM_LEDS_WHEEL, NUM_LEDS_STRIP - 1);
CRGBSet spokeLeds2 = leds2(NUM_LEDS_WHEEL, NUM_LEDS_STRIP - 1);
CRGBSet spokeLeds3 = leds3(NUM_LEDS_WHEEL, NUM_LEDS_STRIP - 1);
CRGBSet spokeLeds4 = leds4(NUM_LEDS_WHEEL, NUM_LEDS_STRIP - 1);

CRGBSet ledWheelSets[] = { wheelLeds1, wheelLeds2, wheelLeds3, wheelLeds4 };
CRGBSet ledSpokeSets[] = { spokeLeds1, spokeLeds2, spokeLeds3, spokeLeds4 };

// Serial input commands.
String inputString = "";
boolean inputStringComplete = false; // whether the String is complete

// Global LED values.
uint8_t gCurrentPatternNumber = 0; // Index number of which pattern is current
uint8_t gBrightness = 96;
uint8_t gHue = 0;                  // rotating "base color" used by many of the patterns
uint16_t gFps = 1000;
bool gHue1 = false;
bool gHue2 = false;
bool gHue3 = false;
bool gHue4 = false;
bool gHue5 = false;
bool gInverse = false;
bool gPause = false;

void setup()
{
    // initialize serial communication at 9600 bits per second:
    Serial.begin(1200);
    inputString.reserve(100);

    Serial.print("Serial port opened.");
    
    delay(3000); // 3 second delay for recovery

    // tell FastLED about the LED strip configuration
    FastLED.addLeds<LED_TYPE, DATA_PIN_1>(leds1, NUM_LEDS_STRIP).setCorrection(TypicalLEDStrip);
    FastLED.addLeds<LED_TYPE, DATA_PIN_2>(leds2, NUM_LEDS_STRIP).setCorrection(TypicalLEDStrip);
    FastLED.addLeds<LED_TYPE, DATA_PIN_3>(leds3, NUM_LEDS_STRIP).setCorrection(TypicalLEDStrip);
    FastLED.addLeds<LED_TYPE, DATA_PIN_4>(leds4, NUM_LEDS_STRIP).setCorrection(TypicalLEDStrip);

    // set master brightness control
    FastLED.setBrightness(gBrightness);

    pinMode(PATTERN_PIN, INPUT);
}

// List of patterns to cycle through.  Each is defined as a separate function below.
typedef void (*SimplePatternList[])(CRGBSet[], int);
SimplePatternList gPatterns = {rainbow, rainbowWithGlitter, confetti, sinelon, bpm, juggle};

void loop()
{
    checkButtonStates();

    tryExecuteCommand();

    if (gInverse) invert();

    if (!gPause)
    {
        // Wheel pattern.
        // Call the current pattern function once, updating the 'leds' array
        gPatterns[gCurrentPatternNumber](ledWheelSets, NUM_LEDS_WHEEL);
        
        // Spoke pattern.
        // Call the current pattern function once, updating the 'leds' array
        //gPatterns[getNextPatternNumber()](ledSpokeSets, NUM_SPOKE_SETS_LEDS);

        // send the 'leds' array out to the actual LED strip
        FastLED.show();
    }

    // insert a delay to keep the framerate modest
    FastLED.delay(1000 / gFps);

    // do some periodic updates
    // EVERY_N_MILLISECONDS(20) { gHue++; }   // slowly cycle the "base color" through the rainbow
    // EVERY_N_SECONDS(10) { nextPattern(); } // change patterns periodically
}

#define ARRAY_SIZE(A) (sizeof(A) / sizeof((A)[0]))

int getNextPatternNumber()
{
    return (gCurrentPatternNumber + 1) % ARRAY_SIZE(gPatterns);
}

void nextPattern()
{
    // add one to the current pattern number, and wrap around at the end
    gCurrentPatternNumber = getNextPatternNumber();
}

#pragma region SET MODIFIERS

void setBrightness()
{
    gBrightness = getAnalogValue(BRIGHTNESS_PIN, gBrightness, 0, 100);
    FastLED.setBrightness(gBrightness);

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

void setFps(int overrideVal = -1)
{
    gFps = getAnalogValue(FPS_PIN, gFps, 500, 5000);

    #if DEBUG
        Serial.print("Setting FPS: ");
        Serial.println(gFps);
    #endif
}

void setHue(int overrideVal = -1)
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

void setBlur(int overrideVal = -1)
{
    
}

void setPosition(int overrideVal = -1)
{

}

void setPause(bool isPaused)
{
    gPause = isPaused;

    #if DEBUG
        Serial.print("Setting Pause: ");
        Serial.println(gPause);
    #endif
}

#pragma endregion SET MODIFIERS

#pragma region PATTERNS

void rainbow(CRGBSet ledSets[], int numLeds)
{
    // FastLED's built-in rainbow generator
    fillRainbow(ledSets, numLeds, gHue, 7);
}

void rainbowWithGlitter(CRGBSet ledSets[], int numLeds)
{
    // built-in FastLED rainbow, plus some random sparkly glitter
    rainbow(ledSets, numLeds);
    addGlitter(80, ledSets, numLeds);
}

void addGlitter(fract8 chanceOfGlitter, CRGBSet ledSets[], int numLeds)
{
    if (random8() < chanceOfGlitter)
    {
        *getLed(ledSets, random16(numLeds), numLeds) += CRGB::White;
    }
}

void confetti(CRGBSet ledSets[], int numLeds)
{
    // random colored speckles that blink in and fade smoothly
    fadeSetsToBlackBy(ledSets, numLeds, 10);

    int pos = random16(numLeds * NUM_SETS);
    *getLed(ledSets, pos, numLeds) += CHSV(gHue + random8(64), 200, 255);
}

void sinelon(CRGBSet ledSets[], int numLeds)
{
    fadeSetsToBlackBy(ledSets, numLeds, 20);

    int pos = beatsin16(13, 0, (numLeds * NUM_SETS) - 1);
    *getLed(ledSets, pos, numLeds) += CHSV(gHue, 255, 192);
}

void bpm(CRGBSet ledSets[], int numLeds)
{
    // colored stripes pulsing at a defined Beats-Per-Minute (BPM)
    uint8_t BeatsPerMinute = 62;
    CRGBPalette16 palette = PartyColors_p;
    uint8_t beat = beatsin8(BeatsPerMinute, 64, 255);
    for (int i = 0; i < numLeds * NUM_SETS; i++)
    { //9948
        *getLed(ledSets, i, numLeds) = ColorFromPalette(palette, gHue + (i * 2), beat - gHue + (i * 10));
    }
}

void juggle(CRGBSet ledSets[], int numLeds)
{
    // eight colored dots, weaving in and out of sync with each other
    fadeSetsToBlackBy(ledSets, numLeds, 20);

    byte dothue = 0;
    for (int i = 0; i < 8; i++)
    {
        int pos = beatsin16(i + 7, 0, (numLeds * NUM_SETS) - 1);
        *getLed(ledSets, pos, numLeds) |= CHSV(dothue, 200, 255);
        dothue += 32;
    }
}

void invert()
{
    // if (gInverse)
    // {
    //     for (int i = 0; i < NUM_LEDS; i++)
    //     {
    //         leds[i] = -leds[i];
    //     }
    // }
}

void fadeSetsToBlackBy(CRGBSet ledSets[], uint16_t numLeds, uint8_t fade)
{
    for (int i = 0; i < NUM_SETS; i++)
    {
        fadeToBlackBy(ledSets[i], numLeds, 20);
    }
}

void fillRainbow(CRGBSet ledSets[], uint16_t numLeds, uint8_t initialHue, uint8_t deltaHue)
{
    for (int i = 0; i < NUM_SETS; i++)
    {
        fill_rainbow(ledSets[i], numLeds, initialHue, deltaHue);
    }
}

#pragma endregion PATTERNS

#pragma region INPUTS

// Button input.
int patternState = 0;         
bool canChangePatternState = true;

void checkButtonStates()
{
    checkButtonState(PATTERN_PIN, &patternState, &canChangePatternState, nextPattern);
}

void checkButtonState(uint8_t pin, int *buttonState, bool *canChangeState, void (*func)())
{
    // Read the state of the button pin.
    *buttonState = digitalRead(pin);
    // Check if the button is pressed. If it is, the buttonState is HIGH.
    if (*buttonState == LOW && *canChangeState) 
    {
        func();
        *canChangeState = false;
    }
    else if (*buttonState == HIGH && !*canChangeState)
    {
        *canChangeState = true;
    }
}

float getAnalogValue(uint8_t pin, float currVal, int32_t minVal, int32_t maxVal)
{
    float val = currVal;
    
    if (!inputString.equals(""))
        val = MeltdownLED.GetAnalogValue(inputString, currVal, minVal, maxVal);
    else
        val = MeltdownLED.GetAnalogValue(pin, currVal, minVal, maxVal);
}

#pragma endregion INPUTS

CRGB* getLed(CRGBSet ledSets[], int index, uint16_t numLeds)
{
    for (int i = 0; i < NUM_SETS; i++)
    {
        if (index < ((i + 1) * numLeds))
        {
            return &ledSets[i][index - (i * numLeds)];
        }
    }
}

int getLedsPerSet(uint16_t numLeds)
{
    return numLeds / NUM_SETS;
}

#pragma region COMMANDS

void tryExecuteCommand()
{
    if (inputStringComplete)
    {
        inputStringComplete = false;

        String command = getCommand();
        if (!command.equals(""))
        {
            if (command.equals("BRIT"))
                setBrightness();
            if (command.equals("SPED"))
                setFps();
            if (command.equals("HUE1"))
            {
                gHue1 = getBoolValue();
                setHue();
            }
            if (command.equals("HUE2"))
            {
                gHue2 = getBoolValue();
                setHue();
            }
            if (command.equals("HUE3"))
            {
                gHue3 = getBoolValue();
                setHue();
            }
            if (command.equals("HUE4"))
            {
                gHue4 = getBoolValue();
                setHue();
            }
            if (command.equals("HUE5"))
            {
                gHue5 = getBoolValue();
                setHue();
            }
            if (command.equals("PAUS"))
                setPause(getBoolValue());
            if (command.equals("BLUR"))
                setBlur();
            if (command.equals("POSN"))
                setPosition();
            if (command.equals("PTN1"))
            {}  
            if (command.equals("PTN2"))
            {}
            if (command.equals("PTN3"))
            {}
            if (command.equals("PTN4"))
            {}
            if (command.equals("PTN5"))
            {}
            if (command.equals("INVR"))
                setInverse();
        }

        inputString = "";
    }
}

String getCommand()
{
    if (inputString[0] == '#' && inputString.length() >= 6)
    {
        return inputString.substring(1, 5);
    }
    return "";
}

int getValue()
{    
    int val = 0;
    if (inputString[0] == '#' && inputString.length() >= 10)
    {
        String valString = inputString.substring(5, 9);
        val = valString.toInt();
    }
    return val;
}

bool getBoolValue()
{
    int val = getValue();
    return val != 0 ? true : false;
}

void serialEvent()
{
    while (Serial.available() && !inputStringComplete)
    {
        // get the new byte:
        char inChar = (char)Serial.read();

        // add it to the inputString:
        inputString += inChar;
        // if the incoming character is a newline, set a flag
        // so the main loop can do something about it:
        if (inChar == '\n')
        {
            Serial.println("Received input string: " + inputString);
            inputStringComplete = true;
        }
    }
}

#pragma endregion COMMANDS