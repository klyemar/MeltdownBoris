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

#define NUM_PENTS 5
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

CRGB *ledWheelSets[NUM_PENTS][NUM_LEDS_PER_WHEEL * NUM_STRIPS_PER_PENT];
//CRGB *ledSpokeSets[NUM_PENTS][NUM_LEDS_PER_SPOKE * NUM_STRIPS_PER_PENT];

// Global LED values.
bool gActiveSpokes = false;
bool gSpokesOnly = false;
bool gWheelsOnly = true;

void setup()
{
    // initialize serial communication at 9600 bits per second:
    Serial.begin(9600);
    Serial1.begin(9600);

    Serial.println("Serial port opened.");
    
    delay(3000); // 3 second delay for recovery
    LEDS.addLeds<OCTOWS2811>(leds, NUM_LEDS_PER_PENT);

    // set master brightness control
    LEDS.setBrightness(MeltdownLED.GetBrightness());

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
                    Serial.print("ledWheelSets["); Serial.print((j * NUM_LEDS_PER_WHEEL) + k); Serial.print("]["); Serial.print(i); Serial.print("]: "); Serial.println((i * NUM_LEDS_PER_PENT) + (j * NUM_LEDS_PER_STRIP) + k);
                #endif
                ledWheelSets[i][(j * NUM_LEDS_PER_WHEEL) + k] = &leds[(i * NUM_LEDS_PER_PENT) + (j * NUM_LEDS_PER_STRIP) + k];
            }

            for (int k = 0; k < NUM_LEDS_PER_SPOKE; k++)
            {
                #if DEBUG
                    Serial.print("ledSpokeSets["); Serial.print((j * NUM_LEDS_PER_SPOKE) + k); Serial.print("]["); Serial.print(i); Serial.print("]: "); Serial.println((i * NUM_LEDS_PER_PENT) + (j * NUM_LEDS_PER_STRIP) + NUM_LEDS_PER_WHEEL + k);
                #endif
                //ledSpokeSets[i][(j * NUM_LEDS_PER_SPOKE) + k] = &leds[(i * NUM_LEDS_PER_PENT) + (j * NUM_LEDS_PER_STRIP) + NUM_LEDS_PER_WHEEL + k];
            }
        }
    }
}

void loop()
{
    checkModifiers();

    tryExecuteCommand();

    if (!MeltdownLED.GetPause())
    {
        if (!gSpokesOnly)
        {
            // Wheel pattern.
            for (uint8_t i = 0; i < NUM_PENTS; i++)
            {
                // Call the current pattern function once, updating the 'leds' array.
                MeltdownLED.ExecutePattern(ledWheelSets[i], NUM_LEDS_PER_WHEEL * NUM_STRIPS_PER_PENT);
            }
        }
        else
        {
            //setColor(ledWheelSets, NUM_LEDS_PER_WHEEL, CRGB::Black);
        }

        if (!gWheelsOnly)
        {
            // Spoke pattern.
            for (uint8_t i = 0; i < NUM_PENTS; i++)
            {
                // Call the current pattern function once, updating the 'leds' array.
                //MeltdownLED.ExecutePattern(ledSpokeSets[i], NUM_LEDS_PER_SPOKE * NUM_STRIPS_PER_PENT);
            }
        }
        else
        {
            //setColor(ledSpokeSets, NUM_LEDS_PER_SPOKE, CRGB::Black);
        }

        //if (gInverse) invert();
        
        // send the 'leds' array out to the actual LED strip
        LEDS.show();
    }

    // insert a delay to keep the framerate modest
    LEDS.delay(1000 / MeltdownLED.GetFps());
 
    EVERY_N_MILLISECONDS( 1 ) { MeltdownLED.IncrementHue(2); } // slowly cycle the "base color" through the rainbow
    EVERY_N_SECONDS( 20 ) { nextPattern(); } // change patterns periodically

    //gutCheck();
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

void nextPattern()
{
    // Set to black.
    MeltdownLED.SetAllColor(leds, NUM_LEDS, CRGB::Black);

    MeltdownLED.NextPattern();
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

#pragma region COMMANDS

void tryExecuteCommand()
{
    if (MeltdownLED.GetInputStringComplete())
    {
        MeltdownLED.SetInputStringComplete(false);

        String command = MeltdownLED.GetCommand();
        if (!command.equals(""))
        {
            if (command.equals("BRIT"))
                //MeltdownLED.SetBrightness();
            if (command.equals("SPED"))
            {
                MeltdownLED.SetFps();
            }
            if (command.equals("HUE1"))
            {
                MeltdownLED.ToggleHue(1);
                #if DEBUG
                Serial.print("Setting value: ");
                Serial.println(MeltdownLED.GetHue());
                #endif
            }
            if (command.equals("HUE2"))
            {
                MeltdownLED.ToggleHue(2);
                #if DEBUG
                Serial.print("Setting value: ");
                Serial.println(MeltdownLED.GetHue());
                #endif
            }
            if (command.equals("HUE3"))
            {
                MeltdownLED.ToggleHue(3);
                #if DEBUG
                Serial.print("Setting value: ");
                Serial.println(MeltdownLED.GetHue());
                #endif
            }
            if (command.equals("HUE4"))
            {
                MeltdownLED.ToggleHue(4);
                #if DEBUG
                Serial.print("Setting value: ");
                Serial.println(MeltdownLED.GetHue());
                #endif
            }
            if (command.equals("HUE5"))
            {
                MeltdownLED.ToggleHue(5);
                #if DEBUG
                Serial.print("Setting value: ");
                Serial.println(MeltdownLED.GetHue());
                #endif
            }
            if (command.equals("PAUS"))
            {
                MeltdownLED.SetPause();
            }
            if (command.equals("FADE"))
            {
                MeltdownLED.SetFade(-1);
                #if DEBUG
                Serial.print("Setting value: ");
                Serial.println(MeltdownLED.GetFade());
                #endif
            }
            if (command.equals("POSN"))
            {
                MeltdownLED.SetPosition(-1);
                #if DEBUG
                Serial.print("Setting value: ");
                Serial.println(MeltdownLED.GetPosition());
                #endif
            }
            if (command.equals("PTN1"))
            {
                nextPattern();
            }
            if (command.equals("PTN2"))
            {}
            if (command.equals("PTN3"))
            {}
            if (command.equals("PTN4"))
            {}
            if (command.equals("PTN5"))
            {}
            if (command.equals("INVR"))
            {
                MeltdownLED.SetInverse();
            }
            if (command.equals("SPOK"))
            {
                setSpokes(MeltdownLED.GetBoolValue());
            }
            if (command.equals("SPON"))
            {
                setSpokesOnly(MeltdownLED.GetBoolValue());
            }
            if (command.equals("WHON"))
            {
                setWheelsOnly(MeltdownLED.GetBoolValue());
            }
        }

        MeltdownLED.ClearInputString();
    }
}

void serialEvent()
{
    while (Serial1.available() && !MeltdownLED.GetInputStringComplete())
    {
        // get the new byte:
        char inChar = (char)Serial1.read();
        // add it to the inputString:
        MeltdownLED.AddCharToInputString(inChar);
        // if the incoming character is a newline, set a flag
        // so the main loop can do something about it:
        if (inChar == '\n')
        {
            Serial.println("Received input string: " + MeltdownLED.GetInputString());
            MeltdownLED.SetInputStringComplete(true);
        }
    }
}

#pragma endregion COMMANDS


