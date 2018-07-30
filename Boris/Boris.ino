#define USE_OCTOWS2811
#include <OctoWS2811.h>
#include <FastLED.h>
#include <MeltdownLED.h>

// Boris, short for Explora Borealis, is an interactive light display
// controlled by several individual user inputs.
//
// This project builds off of the FastLED library and some of its existing patterns
// and examples.

#define DEBUG true

#define PATTERN_PIN 0
#define BRIGHTNESS_PIN 1
#define FPS_PIN 23
#define HUE_PIN 22
#define FADE_PIN 19
#define COOL_PIN 18
#define TERA_PIN 17

#define LED_TYPE OCTOWS2811
#define NUM_SETS 1
#define NUM_STRIPS_PER_SET 1
#define NUM_LEDS_PER_WHEEL 69
#define NUM_LEDS_PER_SPOKE 60
#define NUM_LEDS_PER_STRIP (NUM_LEDS_PER_WHEEL + NUM_LEDS_PER_SPOKE)
#define NUM_LEDS_PER_SET (NUM_LEDS_PER_STRIP * NUM_STRIPS_PER_SET)
#define NUM_LEDS (NUM_SETS * NUM_LEDS_PER_SET)

CRGB leds[NUM_LEDS];

CRGB *ledWheelSets[NUM_LEDS_PER_WHEEL * NUM_STRIPS_PER_SET][NUM_SETS];
CRGB *ledSpokeSets[NUM_LEDS_PER_SPOKE * NUM_STRIPS_PER_SET][NUM_SETS];

// Serial input commands.
String inputString = "";
boolean inputStringComplete = false; // whether the String is complete

// Global LED values.
uint8_t gCurrentPatternNumber = 0; // Index number of which pattern is current
uint8_t gBrightness = 96;
uint8_t gHue = 0;                  // rotating "base color" used by many of the patterns
uint16_t gFps = 1000;
float gFade = 20;
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
    Serial.begin(9600);
    inputString.reserve(100);

    Serial.println("Serial port opened.");
    
    delay(3000); // 3 second delay for recovery
    LEDS.addLeds<OCTOWS2811>(leds, NUM_LEDS_PER_SET);

    // set master brightness control
    LEDS.setBrightness(gBrightness);

    setupLedArrays();

    pinMode(PATTERN_PIN, INPUT);
}

void setupLedArrays()
{
    for (int i = 0; i < NUM_SETS; i++)
    {
        for (int j = 0; j < NUM_STRIPS_PER_SET; j++)
        {
            for (int k = 0; k < NUM_LEDS_PER_WHEEL; k++)
            {
                #if DEBUG
                    Serial.print("ledWheelSets[");
                    Serial.print((j * NUM_LEDS_PER_WHEEL) + k);
                    Serial.print("][");
                    Serial.print(i);
                    Serial.print("]: ");
                    Serial.println((i * NUM_LEDS_PER_SET) + (j * NUM_LEDS_PER_STRIP) + k);
                #endif
                ledWheelSets[(j * NUM_LEDS_PER_WHEEL) + k][i] = &leds[(i * NUM_LEDS_PER_SET) + (j * NUM_LEDS_PER_STRIP) + k];
            }

            for (int k = 0; k < NUM_LEDS_PER_SPOKE; k++)
            {
                #if DEBUG
                    Serial.print("ledSpokeSets[");
                    Serial.print((j * NUM_LEDS_PER_SPOKE) + k);
                    Serial.print("][");
                    Serial.print(i);
                    Serial.print("]: ");
                    Serial.println((i * NUM_LEDS_PER_SET) + (j * NUM_LEDS_PER_STRIP) + NUM_LEDS_PER_WHEEL + k);
                #endif
                ledSpokeSets[(j * NUM_LEDS_PER_SPOKE) + k][i] = &leds[(i * NUM_LEDS_PER_SET) + (j * NUM_LEDS_PER_STRIP) + NUM_LEDS_PER_WHEEL + k];
            }
        }
    }
}

// List of patterns to cycle through.  Each is defined as a separate function below.
typedef void (*SimplePatternList[])(CRGB*[][NUM_SETS], int);
SimplePatternList gPatterns = {rainbow, rainbowWithGlitter, confetti, sinelon, bpm, juggle};

void loop()
{
    checkButtonStates();

    tryExecuteCommand();

    if (!gPause)
    {
        // Wheel pattern.
        // Call the current pattern function once, updating the 'leds' array
        gPatterns[gCurrentPatternNumber](ledWheelSets, NUM_LEDS_PER_WHEEL);

        if (gInverse) invert();
        
        // Spoke pattern.
        // Call the current pattern function once, updating the 'leds' array
        gPatterns[getNextPatternNumber()](ledSpokeSets, NUM_LEDS_PER_SPOKE);

        // send the 'leds' array out to the actual LED strip
        LEDS.show();
    }

    // insert a delay to keep the framerate modest
    LEDS.delay(1000 / gFps);
 
    //gutCheck();
}

void gutCheck()
{   
    static uint8_t hue = 0;
    for(int i = 0; i < NUM_SETS; i++) 
    {
        for(int j = 0; j < (NUM_LEDS_PER_SET) ; j++) 
        {
            leds[(i*(NUM_LEDS_PER_SET)) + j] = CHSV((32*i) + hue+j,192,255);
        }
    }

    // Set the first n leds on each strip to show which strip it is
    for(int i = 0; i < NUM_SETS; i++) 
    {
        for(int j = 0; j <= i; j++) 
        {
            leds[(i*(NUM_LEDS_PER_SET)) + j] = CRGB::Red;
        }
    }

    hue++;

    LEDS.show();
    LEDS.delay(10);
}

#define ARRAY_SIZE(A) (sizeof(A) / sizeof((A)[0]))

int getNextPatternNumber()
{
    return (gCurrentPatternNumber + 1) % ARRAY_SIZE(gPatterns);
}

void nextPattern()
{
    // Set to black.
    setColor(CRGB::Black);
    // add one to the current pattern number, and wrap around at the end
    gCurrentPatternNumber = getNextPatternNumber();

    #if DEBUG
        Serial.println("Next Pattern...");
    #endif
}

#pragma region SET MODIFIERS

void setBrightness()
{
    gBrightness = getAnalogValue(BRIGHTNESS_PIN, gBrightness, 0, 100);
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
    gFps = getAnalogValue(FPS_PIN, gFps, 500, 5000);

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
    gFade = getAnalogValue(FADE_PIN, gFade, 3, 100);

    #if DEBUG
        Serial.print("Setting Fade: ");
        Serial.println(gFade);
    #endif
}

void setPosition()
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

void setColor(CRGB::HTMLColorCode color)
{
    for (int i = 0; i < NUM_LEDS; i++)
    {
        leds[i] = color;
    }
}

void rainbow(CRGB *ledSets[][NUM_SETS], int numLeds)
{
    // FastLED's built-in rainbow generator
    fillRainbow(ledSets, numLeds, gHue, 7);
}

void rainbowWithGlitter(CRGB *ledSets[][NUM_SETS], int numLeds)
{
    // built-in FastLED rainbow, plus some random sparkly glitter
    rainbow(ledSets, numLeds);
    addGlitter(80, ledSets, numLeds);
}

void addGlitter(fract8 chanceOfGlitter, CRGB *ledSets[][NUM_SETS], int numLeds)
{
    if (random8() < chanceOfGlitter)
    {
        for (int i = 0; i < NUM_SETS; i++)
        {
            *getLed(ledSets, i, random16(numLeds * NUM_STRIPS_PER_SET)) += CRGB::White;
        }
    }
}

void confetti(CRGB *ledSets[][NUM_SETS], int numLeds)
{
    // random colored speckles that blink in and fade smoothly
    fadeSetsToBlackBy(ledSets, numLeds, 1);

    int pos = random16(numLeds * NUM_STRIPS_PER_SET);
    for (int i = 0; i < NUM_SETS; i++)
    {
        *getLed(ledSets, i, pos) += CHSV(gHue + random8(64), 200, 255);
    }
}

void sinelon(CRGB *ledSets[][NUM_SETS], int numLeds)
{
    fadeSetsToBlackBy(ledSets, numLeds, 2);

    int pos = beatsin16(13, 0, (numLeds * NUM_STRIPS_PER_SET) - 1);
    for (int i = 0; i < NUM_SETS; i++)
    {
        *getLed(ledSets, i, pos) += CHSV(gHue, 255, 192);
    }
}

void bpm(CRGB *ledSets[][NUM_SETS], int numLeds)
{
    // colored stripes pulsing at a defined Beats-Per-Minute (BPM)
    uint8_t BeatsPerMinute = 62;
    CRGBPalette16 palette = PartyColors_p;
    uint8_t beat = beatsin8(BeatsPerMinute, 64, 255);
    for (int i = 0; i < NUM_SETS; i++)
    { //9948
        for (int j = 0; j < numLeds * NUM_STRIPS_PER_SET; j++)
        {
            *getLed(ledSets, i, j) = ColorFromPalette(palette, gHue + (j * 2), beat - gHue + (j* 10));
        }
    }
}

void juggle(CRGB *ledSets[][NUM_SETS], int numLeds)
{
    // eight colored dots, weaving in and out of sync with each other
    fadeSetsToBlackBy(ledSets, numLeds, 2);

    for (int i = 0; i < NUM_SETS; i++)
    {
        byte dothue = 0;
        for (int j = 0; j < 8; j++)
        {
            int pos = beatsin16(j + 7, 0, (numLeds * NUM_STRIPS_PER_SET) - 1);
            *getLed(ledSets, i, pos) |= CHSV(dothue, 200, 255);
        }
        dothue += 32;
    }
}

void invert()
{
    if (gInverse)
    {
        for (int i = 0; i < NUM_LEDS; i++)
        {
            leds[i] = -leds[i];
        }
    }
}

void fadeSetsToBlackBy(CRGB *ledSets[][NUM_SETS], uint16_t numLeds, uint8_t fade)
{
    for (int i = 0; i < NUM_SETS; i++)
    {
        CRGB* ledSet[numLeds * NUM_STRIPS_PER_SET];
        for (int j = 0; j < numLeds * NUM_STRIPS_PER_SET; j++)
        {
            ledSet[j] = ledSets[j][i];
        }

        for( int j = 0; j < numLeds * NUM_STRIPS_PER_SET; j++) 
        {
            uint8_t scale = 255 - (fade * gFade);
            (*ledSet[j]).nscale8(scale);
        }
    }
}

void fillRainbow(CRGB *ledSets[][NUM_SETS], uint16_t numLeds, uint8_t initialHue, uint8_t deltaHue)
{
    for (int i = 0; i < NUM_SETS; i++)
    {
        CRGB* ledSet[numLeds * NUM_STRIPS_PER_SET];
        for (int j = 0; j < numLeds * NUM_STRIPS_PER_SET; j++)
        {
            ledSet[j] = ledSets[j][i];
        }

        CHSV hsv;
        hsv.hue = initialHue;
        hsv.val = 255;
        hsv.sat = 240;
        for( int j = 0; j < numLeds * NUM_STRIPS_PER_SET; j++) 
        {
            *ledSet[j] = hsv;
            hsv.hue += deltaHue;
        }
    }
}

CRGB* getLed(CRGB *ledSets[][NUM_SETS], int setIndex, int pos)
{
    return ledSets[pos][setIndex];
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

    return val;
}

#pragma endregion INPUTS

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
            if (command.equals("FADE"))
                setFade();
            if (command.equals("POSN"))
                setPosition();
            if (command.equals("PTN1"))
                nextPattern();
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
