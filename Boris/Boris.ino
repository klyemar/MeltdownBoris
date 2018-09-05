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

#define LED_TYPE OCTOWS2811

#define NUM_PENTS 1
#define NUM_STRIPS_PER_PENT 5
#define NUM_BASES 0
#define NUM_STRIPS_PER_BASE 0

#define NUM_LEDS_PER_WHEEL 69
#define NUM_LEDS_PER_SPOKE 60
#define NUM_LEDS_PER_STRIP (NUM_LEDS_PER_WHEEL + NUM_LEDS_PER_SPOKE)
#define NUM_LEDS_PER_PENT (NUM_LEDS_PER_STRIP * NUM_STRIPS_PER_PENT)
#define NUM_PENT_LEDS (NUM_PENTS * NUM_LEDS_PER_PENT)
#define NUM_LEDS_PER_BASE (NUM_LEDS_PER_STRIP * NUM_STRIPS_PER_BASE)
#define NUM_BASE_LEDS (NUM_LEDS_PER_BASE * NUM_BASES)
#define NUM_LEDS (NUM_BASE_LEDS + NUM_PENT_LEDS)

CRGB leds[NUM_LEDS];

CRGB *ledWheelSets[NUM_LEDS_PER_WHEEL * NUM_STRIPS_PER_PENT][NUM_PENTS];
CRGB *ledSpokeSets[NUM_LEDS_PER_SPOKE * NUM_STRIPS_PER_PENT][NUM_PENTS];

// Serial input commands.
String inputString = "";
boolean inputStringComplete = false; // whether the String is complete

// Global LED values.
bool gActiveSpokes = false;
bool gSpokesOnly = false;
bool gWheelsOnly = true;

void setup()
{
    // initialize serial communication at 9600 bits per second:
    Serial.begin(9600);
    Serial1.begin(9600);
    inputString.reserve(100);

    Serial.println("Serial port opened.");
    
    delay(3000); // 3 second delay for recovery
    LEDS.addLeds<OCTOWS2811>(leds, NUM_LEDS_PER_PENT);

    // set master brightness control
    LEDS.setBrightness(gBrightness);

    setupLedArrays();
}

void setupLedArrays()
{
    for (int i = 0; i < NUM_PENTS; i++)
    {
        for (int j = 0; j < NUM_STRIPS_PER_PENT; j++)
        {
            for (int k = 0; k < NUM_LEDS_PER_WHEEL; k++)
            {
                #if DEBUG
                    Serial.print("ledWheelSets[");
                    Serial.print((j * NUM_LEDS_PER_WHEEL) + k);
                    Serial.print("][");
                    Serial.print(i);
                    Serial.print("]: ");
                    Serial.println((i * NUM_LEDS_PER_PENT) + (j * NUM_LEDS_PER_STRIP) + k);
                #endif
                ledWheelSets[(j * NUM_LEDS_PER_WHEEL) + k][i] = &leds[(i * NUM_LEDS_PER_PENT) + (j * NUM_LEDS_PER_STRIP) + k];
            }

            for (int k = 0; k < NUM_LEDS_PER_SPOKE; k++)
            {
                #if DEBUG
                    Serial.print("ledSpokeSets[");
                    Serial.print((j * NUM_LEDS_PER_SPOKE) + k);
                    Serial.print("][");
                    Serial.print(i);
                    Serial.print("]: ");
                    Serial.println((i * NUM_LEDS_PER_PENT) + (j * NUM_LEDS_PER_STRIP) + NUM_LEDS_PER_WHEEL + k);
                #endif
                ledSpokeSets[(j * NUM_LEDS_PER_SPOKE) + k][i] = &leds[(i * NUM_LEDS_PER_PENT) + (j * NUM_LEDS_PER_STRIP) + NUM_LEDS_PER_WHEEL + k];
            }
        }
    }
}

void loop()
{
    checkModifiers();

    tryExecuteCommand();

    if (!gPause)
    {
        if (!gSpokesOnly)
        {
            // Wheel pattern.
            for (uint8_t i; i < NUM_PENTS; i++)
            {
                CRGB *ledSet[] = ledWheelSets[i];
                // Call the current pattern function once, updating the 'leds' array.
                gPatterns[gCurrentPatternNumber](ledSet, NUM_LEDS_PER_WHEEL * NUM_STRIPS_PER_PENT);
            }
        }
        else
        {
            setColor(ledWheelSets, NUM_LEDS_PER_WHEEL, CRGB::Black);
        }

        if (!gWheelsOnly)
        {
            // Spoke pattern.
            for (uint8_t i; i < NUM_PENTS; i++)
            {
                CRGB *ledSet[] = ledSpokeSets[i];
                // Call the current pattern function once, updating the 'leds' array.
                gPatterns[gCurrentPatternNumber](ledSet, NUM_LEDS_PER_SPOKE * NUM_STRIPS_PER_PENT);
            }
        }
        else
        {
            setColor(ledSpokeSets, NUM_LEDS_PER_SPOKE, CRGB::Black);
        }

        if (gInverse) invert();
        

        // send the 'leds' array out to the actual LED strip
        LEDS.show();
    }

    // insert a delay to keep the framerate modest
    LEDS.delay(1000 / gFps);
 
    //gutCheck();
    EVERY_N_MILLISECONDS( 1 ) { gHue++; } // slowly cycle the "base color" through the rainbow
    EVERY_N_SECONDS( 20 ) { nextPattern(); } // change patterns periodically
}

void gutCheck()
{   
    static uint8_t hue = 0;
    for(int i = 0; i < NUM_PENTS; i++) 
    {
        for(int j = 0; j < (NUM_LEDS_PER_PENT) ; j++) 
        {
            leds[(i*(NUM_LEDS_PER_PENT)) + j] = CHSV((32*i) + hue+j,192,255);
        }
    }

    // Set the first n leds on each strip to show which strip it is
    for(int i = 0; i < NUM_PENTS; i++) 
    {
        for(int j = 0; j <= i; j++) 
        {
            leds[(i*(NUM_LEDS_PER_PENT)) + j] = CRGB::Red;
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
    setAllColor(leds, NUM_LEDS, CRGB::Black);
    // add one to the current pattern number, and wrap around at the end
    gCurrentPatternNumber = getNextPatternNumber();

    #if DEBUG
        Serial.println("Next Pattern...");
    #endif
}

#pragma region SET MODIFIERS

void checkModifiers()
{
    //setBrightness();
}

void setSpokes(bool isActive)
{
    gActiveSpokes = isActive;

    if (!isActive)
    {
        gSpokesOnly = false;
    }

    #if DEBUG
        Serial.print("Setting spokes: ");
        Serial.println(gActiveSpokes);
    #endif
}

void setSpokesOnly(bool isActive)
{
    gSpokesOnly = isActive;

    #if DEBUG
        Serial.print("Setting spokes only: ");
        Serial.println(gSpokesOnly);
    #endif
}

void setWheelsOnly(bool isActive)
{
    gWheelsOnly = isActive;

    #if DEBUG
        Serial.print("Setting wheels only: ");
        Serial.println(gWheelsOnly);
    #endif
}

#pragma endregion SET MODIFIERS

#pragma region INPUTS

float getAnalogValue(float currVal, int32_t minVal, int32_t maxVal)
{
    float val = currVal;
    
    if (!inputString.equals(""))
    {
        val = MeltdownLED.GetSerialValue(inputString, currVal, minVal, maxVal);
    }

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
            if (command.equals("SPOK"))
                setSpokes(getBoolValue());
            if (command.equals("SPON"))
                setSpokesOnly(getBoolValue());
            if (command.equals("WHON"))
                setWheelsOnly(getBoolValue());
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
    while (Serial1.available() && !inputStringComplete)
    {
        // get the new byte:
        char inChar = (char)Serial1.read();
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
