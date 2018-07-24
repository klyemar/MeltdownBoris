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
#define BRIGHTNESS_PIN -1
#define FPS_PIN -1
#define HUE_PIN -1

#define LED_TYPE WS2812B
#define NUM_LEDS 129
#define NUM_LEDS_WHEEL 69
#define NUM_LEDS_SPOKE 60
#define NUM_SETS 3
#define LEDS_PER_SET 25
#define NUM_SUPERSET_LEDS (NUM_SETS * LEDS_PER_SET)
CRGBArray<NUM_LEDS> leds;

CRGBSet leds1 = leds(0, 24);
CRGBSet leds2 = leds(50, 74);
CRGBSet leds3 = leds(100, 124);

CRGBSet ledSuperSet[3] = { leds1, leds2, leds3 };

// Serial input commands.
String inputString = "";
boolean inputStringComplete = false; // whether the String is complete

// Button input.
int buttonState = 0;         
bool canChangeState = true;

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
    FastLED.addLeds<LED_TYPE, DATA_PIN>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);

    // set master brightness control
    FastLED.setBrightness(gBrightness);

    pinMode(SWITCH_PIN, INPUT);
}

// List of patterns to cycle through.  Each is defined as a separate function below.
typedef void (*SimplePatternList[])(CRGBSet[], int);
SimplePatternList gPatterns = {rainbow, rainbowWithGlitter, confetti, sinelon, bpm, juggle};

void loop()
{
    tryExecuteCommand();

    if (gInverse)
    {
        invert();
    }

    if (!gPause)
    {
        // Wheel pattern.
        // Call the current pattern function once, updating the 'leds' array
        gPatterns[gCurrentPatternNumber](ledSuperSet, NUM_SUPERSET_LEDS);
        
        // // Spoke pattern.
        // // Call the current pattern function once, updating the 'leds' array
        // gPatterns[getNextPatternNumber()](leds(NUM_LEDS_WHEEL, NUM_LEDS - 1), NUM_LEDS_SPOKE);

        // send the 'leds' array out to the actual LED strip
        FastLED.show();
    }

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

float getAnalogValue(uint8_t pin, float currVal, int32_t minVal, int32_t maxVal)
{
    float val = currVal;
    
    if (!inputString.equals(""))
        val = MeltdownLED.GetAnalogValue(inputString, currVal, minVal, maxVal);
    else
        val = MeltdownLED.GetAnalogValue(pin, currVal, minVal, maxVal);

    Serial.println(val);

    return val;
}

void setBrightness()
{
    Serial.print("Setting Brightness: ");
    gBrightness = getAnalogValue(BRIGHTNESS_PIN, gBrightness, 0, 100);

    FastLED.setBrightness(gBrightness);
}

void setInverse()
{
    gInverse = getBoolValue();

    Serial.print("Setting Inverse: ");
    Serial.println(gInverse);
}

void setFps(int overrideVal = -1)
{
    gFps = getAnalogValue(FPS_PIN, gFps, 500, 5000);
    
    Serial.print("Setting FPS: ");
    Serial.println(gFps);
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

    Serial.print("Setting Hue: ");
    Serial.println(gHue);
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

    Serial.print("Setting Pause: ");
    Serial.println(gPause);
}

void rainbow(CRGBSet ledSets[], int numLeds)
{
    // FastLED's built-in rainbow generator
    fillRainbow(ledSets, NUM_SETS, LEDS_PER_SET, gHue, 7);
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
        *getLed(ledSets, random16(numLeds)) += CRGB::White;
    }
}

void confetti(CRGBSet ledSets[], int numLeds)
{
    // random colored speckles that blink in and fade smoothly
    fadeSetsToBlackBy(ledSets, NUM_SETS, LEDS_PER_SET, 10);

    int pos = random16(numLeds);
    *getLed(ledSets, pos) += CHSV(gHue + random8(64), 200, 255);
}

void sinelon(CRGBSet ledSets[], int numLeds)
{
    fadeSetsToBlackBy(ledSets, NUM_SETS, LEDS_PER_SET, 20);

    int pos = beatsin16(13, 0, numLeds - 1);
    *getLed(ledSets, pos) += CHSV(gHue, 255, 192);
}

void bpm(CRGBSet ledSets[], int numLeds)
{
    // colored stripes pulsing at a defined Beats-Per-Minute (BPM)
    uint8_t BeatsPerMinute = 62;
    CRGBPalette16 palette = PartyColors_p;
    uint8_t beat = beatsin8(BeatsPerMinute, 64, 255);
    for (int i = 0; i < numLeds; i++)
    { //9948
        *getLed(ledSets, i) = ColorFromPalette(palette, gHue + (i * 2), beat - gHue + (i * 10));
    }
}

void juggle(CRGBSet ledSets[], int numLeds)
{
    // eight colored dots, weaving in and out of sync with each other
    fadeSetsToBlackBy(ledSets, NUM_SETS, LEDS_PER_SET, 20);

    byte dothue = 0;
    for (int i = 0; i < 8; i++)
    {
        *getLed(ledSets, beatsin16(i + 7, 0, numLeds - 1)) |= CHSV(dothue, 200, 255);
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

void fadeSetsToBlackBy(CRGBSet ledSets[], uint8_t numSets, uint16_t numLeds, uint8_t fade)
{
    for (int i = 0; i < numSets; i++)
    {
        fadeToBlackBy(ledSets[i], numLeds, 20);
    }
}

void fillRainbow(CRGBSet ledSets[], uint8_t numSets, uint16_t numLeds, uint8_t initialHue, uint8_t deltaHue)
{
    for (int i = 0; i < numSets; i++)
    {
        fill_rainbow(ledSets[i], numLeds, initialHue, deltaHue);
    }
}

CRGB* getLed(CRGBSet ledSets[], int index)
{
    for (int i = 0; i < NUM_SETS; i++)
    {
        if (index < ((i + 1) * LEDS_PER_SET))
        {
            return &ledSets[i][index - (i * LEDS_PER_SET)];
        }
    }
}

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