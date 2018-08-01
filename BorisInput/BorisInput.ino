#include <FastLED.h>
#include <MeltdownLED.h>

// Boris, short for Explora Borealis, is an interactive light display
// controlled by several individual user inputs.
//
// This project builds off of the FastLED library and some of its existing patterns
// and examples.

#define DEBUG true

#define DATA_PIN 22

#define BUTTON_PIN_1 2
#define BUTTON_PIN_2 3
#define BUTTON_PIN_3 4
#define BUTTON_PIN_4 5
#define BUTTON_PIN_5 6
#define BUTTON_PIN_6 7
#define BUTTON_PIN_7 8
#define BUTTON_PIN_8 9
#define BUTTON_PIN_9 10
#define BUTTON_PIN_10 11
#define BUTTON_PIN_11 12
#define BUTTON_PIN_12 13
#define ANALOG_PIN_1 A0
#define ANALOG_PIN_2 A1
#define ANALOG_PIN_3 A2

#define PATTERN_PIN BUTTON_PIN_1
#define BRIGHTNESS_PIN ANALOG_PIN_1
#define FPS_PIN ANALOG_PIN_2
#define FADE_PIN ANALOG_PIN_3

#define NUM_MED_RINGS 0
#define NUM_LARGE_RINGS 0

#define NUM_LEDS_PER_MED_RING 16
#define NUM_LEDS_PER_LARGE_RING 24
#define NUM_MED_RING_LEDS (NUM_LEDS_PER_MED_RING + NUM_MED_RINGS)
#define NUM_LARGE_RING_LEDS (NUM_LEDS_PER_LARGE_RING + NUM_LARGE_RINGS)
#define NUM_RING_LEDS (NUM_LARGE_RING_LEDS + NUM_MED_RING_LEDS)
#define NUM_LEDS (NUM_RING_LEDS)

CRGB leds[NUM_LEDS];

CRGB *ledLargeRingSets[NUM_LARGE_RINGS];
CRGB *ledMedRingSets[NUM_MED_RINGS];

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

// Buttons.
struct Button
{
    uint8_t pin;
    uint8_t state;
    bool canChangeState;
    void (*callback)();
    
    Button(uint8_t pin) : pin (pin), state {0}, canChangeState {true}
    {}
};

Button patternButton = { PATTERN_PIN };
Button button1 = { BUTTON_PIN_1 };
Button button2 = { BUTTON_PIN_2 };
Button button3 = { BUTTON_PIN_3 };
Button button4 = { BUTTON_PIN_4 }; 

// Ring options.
enum RingPosition { Top, Bottom, Full, Empty };
enum RingSize { Large, Medium };

void setup()
{
    // initialize serial communication at 9600 bits per second:
    Serial.begin(9600);
    Serial2.begin(9600);

    Serial.println("Serial port opened.");
    
    delay(3000); // 3 second delay for recovery
    //FastLED
    FastLED.addLeds<WS2812B, DATA_PIN, RGB>(leds, NUM_LEDS);  

    // set master brightness control
    LEDS.setBrightness(gBrightness);

    setupLedArrays();

    setupButtons();

    pinMode(BUTTON_PIN_1, INPUT);
    pinMode(BUTTON_PIN_2, INPUT);
    pinMode(BUTTON_PIN_3, INPUT);
    pinMode(BUTTON_PIN_4, INPUT);
    pinMode(BUTTON_PIN_5, INPUT);
    pinMode(BUTTON_PIN_6, INPUT);
    pinMode(BUTTON_PIN_7, INPUT);
    pinMode(BUTTON_PIN_8, INPUT);
    pinMode(BUTTON_PIN_9, INPUT);
    pinMode(BUTTON_PIN_10, INPUT);
    pinMode(BUTTON_PIN_11, INPUT);
    pinMode(BUTTON_PIN_12, INPUT);
    pinMode(ANALOG_PIN_1, INPUT);
    pinMode(ANALOG_PIN_2, INPUT);
    pinMode(ANALOG_PIN_3, INPUT);
}

void setupLedArrays()
{
    for (int i = 0; i < NUM_LARGE_RINGS; i++)
    {
        for (int j = 0; j < NUM_LEDS_PER_LARGE_RING; j++)
        {
            ledLargeRingSets[i] = &leds[(i * NUM_LEDS_PER_LARGE_RING) + j];
        }
    }
    for (int i = 0; i < NUM_MED_RINGS; i++)
    {
        for (int j = 0; j < NUM_LEDS_PER_MED_RING; j++)
        {
            ledMedRingSets[i] = &leds[(i * NUM_LEDS_PER_MED_RING) + j];
        }
    }
}

void setupButtons()
{  
    patternButton.callback = nextPattern;
}

// List of patterns to cycle through.  Each is defined as a separate function below.
typedef void (*SimplePatternList[])(CRGB*[], int);
SimplePatternList gPatterns = {rainbow, rainbowWithGlitter, confetti, sinelon, bpm, juggle};

void loop()
{
    checkButtonStates();

    checkModifiers();

    if (!gPause)
    {
        // Wheel pattern.
        // Call the current pattern function once, updating the 'leds' array
        //gPatterns[gCurrentPatternNumber](ledLargeRingSets, NUM_LARGE_RING_LEDS);

        //if (gInverse) invert(leds, NUM_LEDS);
        
        // Spoke pattern.
        // Call the current pattern function once, updating the 'leds' array
        //gPatterns[getNextPatternNumber()](ledMedRingSets, NUM_MED_RING_LEDS);

        // send the 'leds' array out to the actual LED strip
        //LEDS.show();
    }

    // insert a delay to keep the framerate modest
    //LEDS.delay(1000 / gFps);
 
    //gutCheck();
}

void gutCheck()
{   
   
}

#define ARRAY_SIZE(A) (sizeof(A) / sizeof((A)[0]))

int getNextPatternNumber()
{
    return (gCurrentPatternNumber + 1) % ARRAY_SIZE(gPatterns);
}

void nextPattern()
{
    // Set to black.
    setColor(leds, NUM_LEDS, CRGB::Black);
    // add one to the current pattern number, and wrap around at the end
    gCurrentPatternNumber = getNextPatternNumber();

    #if DEBUG
        Serial.println("Next Pattern...");
    #endif
    sendBoolCommand("PTN1", true);
}

#pragma region SET MODIFIERS

void checkModifiers()
{
    setBrightness();
    // setFps();
    // setFade();
}

void setBrightness()
{
    int32_t minVal = 0;
    int32_t maxVal = 100;
    float currVal = gBrightness;

    gBrightness = MeltdownLED.GetAnalogValue(BRIGHTNESS_PIN, currVal, minVal, maxVal);
    LEDS.setBrightness(gBrightness);

    if (MeltdownLED.HasChanged(currVal, gBrightness))
    {
    #if DEBUG
        Serial.print("Setting Brightness: ");
        Serial.println(gBrightness);
    #endif
        sendCommand("BRIT", gBrightness);
    }
}

void setInverse()
{
    //gInverse = getBoolValue();

    #if DEBUG
        Serial.print("Setting Inverse: ");
        Serial.println(gInverse);
    #endif
}

void setFps()
{
    int32_t minVal = 500;
    int32_t maxVal = 5000;
    float currVal = gFps;

    gFps = MeltdownLED.GetAnalogValue(FPS_PIN, currVal, minVal, maxVal);

    if (MeltdownLED.HasChanged(currVal, gFps))
    {
    #if DEBUG
        Serial.print("Setting FPS: ");
        Serial.println(gFps);
    #endif
        sendCommand("SPED", gFps);
    }
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
    int32_t minVal = 3;
    int32_t maxVal = 100;
    float currVal = gFade;

    gFade = MeltdownLED.GetAnalogValue(FADE_PIN, currVal, minVal, maxVal);

    if (MeltdownLED.HasChanged(currVal, gFade))
    {
    #if DEBUG
        Serial.print("Setting Fade: ");
        Serial.println(gFade);
    #endif
        sendCommand("FADE", gFade);
    }
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

void setColor(CRGB ledSets[], int numLeds, CRGB::HTMLColorCode color)
{
    for (int i = 0; i < numLeds; i++)
    {
        ledSets[i] = color;
    }
}

void setRingColor(CRGB *ringLeds[], RingPosition position, RingSize size, CRGB::HTMLColorCode color)
{
    uint8_t numLeds = getNumRingLeds(position, size);

    CRGB** leds = getRingLeds(ringLeds, position, size);

    if (position == Empty) color = CRGB::Black;

    for (int i = 0; i < numLeds; i++)
    {
        *leds[i] = color;
    }
}

void rainbow(CRGB *ledSets[], int numLeds)
{
    // FastLED's built-in rainbow generator
    fill_rainbow(*ledSets, numLeds, gHue, 7);
}

void rainbowWithGlitter(CRGB *ledSets[], int numLeds)
{
    // built-in FastLED rainbow, plus some random sparkly glitter
    rainbow(ledSets, numLeds);
    addGlitter(80, ledSets, numLeds);
}

void addGlitter(fract8 chanceOfGlitter, CRGB *ledSets[], int numLeds)
{
    if (random8() < chanceOfGlitter)
    {
        *getLed(ledSets, random16(numLeds)) += CRGB::White;
    }
}

void confetti(CRGB *ledSets[], int numLeds)
{
    // random colored speckles that blink in and fade smoothly
    fadeToBlackBy(*ledSets, numLeds, gFade);

    int pos = random16(numLeds);

    *getLed(ledSets, pos) += CHSV(gHue + random8(64), 200, 255);
}

void sinelon(CRGB *ledSets[], int numLeds)
{
    fadeToBlackBy(*ledSets, numLeds, gFade);

    int pos = beatsin16(13, 0, numLeds - 1);

    *getLed(ledSets, pos) += CHSV(gHue, 255, 192);
}

void bpm(CRGB *ledSets[], int numLeds)
{
    // colored stripes pulsing at a defined Beats-Per-Minute (BPM)
    uint8_t BeatsPerMinute = 62;
    CRGBPalette16 palette = PartyColors_p;
    uint8_t beat = beatsin8(BeatsPerMinute, 64, 255);
    
    //9948
    for (int i = 0; i < numLeds; i++)
    {
        *getLed(ledSets, i) = ColorFromPalette(palette, gHue + (i * 2), beat - gHue + (i* 10));
    }
}

void juggle(CRGB *ledSets[], int numLeds)
{
    // eight colored dots, weaving in and out of sync with each other
    fadeToBlackBy(*ledSets, numLeds, gFade);

    byte dothue = 0;
    for (int i = 0; i < 8; i++)
    {
        int pos = beatsin16(i + 7, 0, (numLeds) - 1);
        *getLed(ledSets, pos) |= CHSV(dothue, 200, 255);
    }
    dothue += 32;
}

void invert(CRGB ledSets[], int numLeds)
{
    if (gInverse)
    {
        for (int i = 0; i < numLeds; i++)
        {
            ledSets[i] = -ledSets[i];
        }
    }
}

CRGB* getLed(CRGB *ledSets[], int pos)
{
    return ledSets[pos];
}

CRGB** getRingLeds(CRGB *ringLeds[], RingPosition position, RingSize size)
{
    // If the position is full or empty, we'll want all of our lights.
    if (position == Full || position == Empty) return ringLeds;

    uint8_t numLeds = getNumRingLeds(position, size);

    CRGB *halfLeds[numLeds];

    for (int i = 0; i < numLeds; i++)
    {
        if (position == Top)
        {
            halfLeds[i] = ringLeds[i];
        }
        else
        {
            halfLeds[i] = ringLeds[i + numLeds];
        }
    }

    return halfLeds;
}

uint8_t getNumRingLeds(RingPosition position, RingSize size)
{
    uint8_t numLeds = NUM_LEDS_PER_LARGE_RING;
    if (size == Medium)
    {
        numLeds = NUM_LEDS_PER_MED_RING;
    }

    if (position == Top || position == Bottom)
    {
        return numLeds / 2;
    }
    else
    {
        return numLeds;
    }
}

#pragma endregion PATTERNS

#pragma region INPUTS

void checkButtonStates()
{
    checkButtonState(&patternButton);
}

void checkButtonState(Button *button)
{
    // Read the state of the button pin.
    button->state = digitalRead(button->pin);
    // Check if the button is pressed. If it is, the buttonState is HIGH.
    if (button->state == LOW && button->canChangeState) 
    {
        button->callback();
        button->canChangeState = false;
    }
    else if (button->state == HIGH && !(button->canChangeState))
    {
        button->canChangeState = true;
    }
}

#pragma endregion INPUTS

#pragma region COMMANDS

void tryExecuteCommand()
{
    // if (inputStringComplete)
    // {
    //     inputStringComplete = false;

    //     String command = getCommand();
    //     if (!command.equals(""))
    //     {
    //         if (command.equals("BRIT"))
    //             setBrightness();
    //         if (command.equals("SPED"))
    //             setFps();
    //         if (command.equals("HUE1"))
    //         {
    //             gHue1 = getBoolValue();
    //             setHue();
    //         }
    //         if (command.equals("HUE2"))
    //         {
    //             gHue2 = getBoolValue();
    //             setHue();
    //         }
    //         if (command.equals("HUE3"))
    //         {
    //             gHue3 = getBoolValue();
    //             setHue();
    //         }
    //         if (command.equals("HUE4"))
    //         {
    //             gHue4 = getBoolValue();
    //             setHue();
    //         }
    //         if (command.equals("HUE5"))
    //         {
    //             gHue5 = getBoolValue();
    //             setHue();
    //         }
    //         if (command.equals("PAUS"))
    //             setPause(getBoolValue());
    //         if (command.equals("FADE"))
    //             setFade();
    //         if (command.equals("POSN"))
    //             setPosition();
    //         if (command.equals("PTN1"))
    //             nextPattern();
    //         if (command.equals("PTN2"))
    //         {}
    //         if (command.equals("PTN3"))
    //         {}
    //         if (command.equals("PTN4"))
    //         {}
    //         if (command.equals("PTN5"))
    //         {}
    //         if (command.equals("INVR"))
    //             setInverse();
    //     }

    //     inputString = "";
    // }
}

void sendBoolCommand(String command, bool value)
{
    sendCommand(command, value ? 1 : 0);
}

void sendCommand(String command, float value)
{
    if (value < 0) value = 0;
    if (value > 9999) value = 9999;

    String valueString = "";
    if (value < 1000) valueString += "0";
    if (value < 100) valueString += "0";
    if (value < 10) valueString += "0";
    valueString += (uint16_t)value;

    String serialCommand = "#" + command + valueString + "\n";
    
    if (Serial2.available())
    {
        Serial.print("Sending command: " + serialCommand);
        for (int i = 0; i < serialCommand.length(); i++)
        {
            Serial2.write(serialCommand[i]);   // Push each char 1 by 1 on each loop pass
        }
    }
}

#pragma endregion COMMANDS
