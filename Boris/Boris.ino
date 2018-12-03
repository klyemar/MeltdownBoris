#define USE_OCTOWS2811
#include <OctoWS2811.h>
#include <FastLED.h>
#include <MeltdownLED.h>
#include <MeltdownLogger.h>
#include <MeltdownSerial.h>

// Boris, short for Explora Borealis, is an interactive light display
// controlled by several individual user inputs.
//
// This project builds off of the FastLED library and some of its existing patterns
// and examples.

#define DEBUG true

#define LED_TYPE OCTOWS2811

#define NUM_PENTS 1
#define NUM_STRIPS_PER_PENT 1

#define NUM_LEDS_PER_WHEEL 36 //69
#define NUM_LEDS_PER_SPOKE 0 //60
#define NUM_LEDS_PER_STRIP (NUM_LEDS_PER_WHEEL + NUM_LEDS_PER_SPOKE)
#define NUM_LEDS_PER_PENT (NUM_LEDS_PER_STRIP * NUM_STRIPS_PER_PENT)
#define NUM_PENT_LEDS (NUM_PENTS * NUM_LEDS_PER_PENT)
#define NUM_LEDS (NUM_PENT_LEDS)

CRGB leds[NUM_LEDS];

CRGB ledWheelSets[NUM_PENTS][NUM_LEDS_PER_WHEEL * NUM_STRIPS_PER_PENT];
//CRGB ledSpokeSets[NUM_PENTS][NUM_LEDS_PER_SPOKE * NUM_STRIPS_PER_PENT];

void setup()
{
    // initialize serial communication at 9600 bits per second:
    Serial.begin(9600);
    Serial1.begin(9600);

    Serial.println("Serial port opened.");

    MeltdownLogger.InitSerial(DEBUG);
    
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
                ledWheelSets[i][(j * NUM_LEDS_PER_WHEEL) + k] = leds[(i * NUM_LEDS_PER_PENT) + (j * NUM_LEDS_PER_STRIP) + k];
            }

            for (int k = 0; k < NUM_LEDS_PER_SPOKE; k++)
            {
                #if DEBUG
                    Serial.print("ledSpokeSets["); Serial.print((j * NUM_LEDS_PER_SPOKE) + k); Serial.print("]["); Serial.print(i); Serial.print("]: "); Serial.println((i * NUM_LEDS_PER_PENT) + (j * NUM_LEDS_PER_STRIP) + NUM_LEDS_PER_WHEEL + k);
                #endif
                //ledSpokeSets[i][(j * NUM_LEDS_PER_SPOKE) + k] = leds[(i * NUM_LEDS_PER_PENT) + (j * NUM_LEDS_PER_STRIP) + NUM_LEDS_PER_WHEEL + k];
            }
        }
    }
}

void loop()
{
    // gutCheck();

    checkModifiers();

    tryExecuteCommand();

    if (!MeltdownLED.GetPause())
    {
        // Wheel pattern.
        for (uint8_t i = 0; i < NUM_PENTS; i++)
        {
            // Call the current pattern function once, updating the 'leds' array.
            MeltdownLED.ExecutePattern(&leds[i], NUM_LEDS_PER_WHEEL * NUM_STRIPS_PER_PENT);
            MeltdownLED.ExecuteEffect(&leds[i], NUM_LEDS_PER_WHEEL * NUM_STRIPS_PER_PENT);
        }
        // if (!gSpokesOnly)
        // {
        //     // Wheel pattern.
        //     for (uint8_t i = 0; i < NUM_PENTS; i++)
        //     {
        //         // Call the current pattern function once, updating the 'leds' array.
        //         MeltdownLED.ExecutePattern(ledWheelSets[i], NUM_LEDS_PER_WHEEL * NUM_STRIPS_PER_PENT);
        //     }
        // }
        // else
        // {
        //     //setColor(ledWheelSets, NUM_LEDS_PER_WHEEL, CRGB::Black);
        // }

        // if (!gWheelsOnly)
        // {
        //     // Spoke pattern.
        //     for (uint8_t i = 0; i < NUM_PENTS; i++)
        //     {
        //         // Call the current pattern function once, updating the 'leds' array.
        //         //MeltdownLED.ExecutePattern(ledSpokeSets[i], NUM_LEDS_PER_SPOKE * NUM_STRIPS_PER_PENT);
        //     }
        // }
        // else
        // {
        //     //setColor(ledSpokeSets, NUM_LEDS_PER_SPOKE, CRGB::Black);
        // }
        
        // send the 'leds' array out to the actual LED strip
        //LEDS.show();
    }

    // insert a delay to keep the framerate modest
    LEDS.delay(1000 / MeltdownLED.GetFps());
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

#pragma region SET MODIFIERS

void checkModifiers()
{
}

void setBoth()
{
    // gSpokesOnly = false;
    // gWheelsOnly = false;

    // MeltdownLogger.Debug(Serial, "Setting both...");   
    // MeltdownSerial.SendBoolCommand(Serial, Serial2, MeltdownSerial.SPOKE, false);
    // MeltdownSerial.SendBoolCommand(Serial, Serial2, MeltdownSerial.WHEEL, false);
}

void setSpokesOnly()
{
    // gSpokesOnly = !gSpokesOnly;

    // MeltdownLogger.Debug(Serial, "Setting spokes only: ", gSpokesOnly);   
    // MeltdownSerial.SendBoolCommand(Serial, Serial2, MeltdownSerial.SPOKE, gSpokesOnly);
}

void setWheelsOnly()
{
    // gWheelsOnly = !gWheelsOnly;

    // MeltdownLogger.Debug(Serial, "Setting wheels only: ", gWheelsOnly);   
    // MeltdownSerial.SendBoolCommand(Serial, Serial2, MeltdownSerial.WHEEL, gWheelsOnly);
}

#pragma endregion SET MODIFIERS

#pragma region COMMANDS

void tryExecuteCommand()
{
    if (MeltdownSerial.GetInputStringComplete())
    {
        MeltdownSerial.SetInputStringComplete(false);

        String command = MeltdownSerial.GetCommand();
        if (!command.equals(""))
        {
            if (command.equals(MeltdownSerial.BRIGHTNESS))
            {
                MeltdownLED.SetBrightness(-1);
            }
            else if (command.equals(MeltdownSerial.HUE1))
            {
                bool hueValue = MeltdownLED.ToggleHue(1);
                MeltdownLogger.Debug(Serial, "Toggling Hue...", hueValue);   
            }
            else if (command.equals(MeltdownSerial.HUE2))
            {
                bool hueValue = MeltdownLED.ToggleHue(2);
                MeltdownLogger.Debug(Serial, "Toggling Hue...", hueValue);  
            }
            else if (command.equals(MeltdownSerial.HUE3))
            {
                bool hueValue = MeltdownLED.ToggleHue(3);
                MeltdownLogger.Debug(Serial, "Toggling Hue...", hueValue);  
            }
            else if (command.equals(MeltdownSerial.HUE4))
            {
                bool hueValue = MeltdownLED.ToggleHue(4);
                MeltdownLogger.Debug(Serial, "Toggling Hue...", hueValue);  
            }
            else if (command.equals(MeltdownSerial.HUE5))
            {
                bool hueValue = MeltdownLED.ToggleHue(5);
                MeltdownLogger.Debug(Serial, "Toggling Hue...", hueValue);  
            }
            else if (command.equals(MeltdownSerial.PAUSE))
            {
                MeltdownLED.SetPause();
            }
            else if (command.equals(MeltdownSerial.PATTERN))
            {
                // Set to black.
                MeltdownLED.SetAllColor(leds, NUM_LEDS, CRGB::Black);
                MeltdownLED.NextPattern();

                MeltdownLogger.Debug(Serial, "Next Pattern...");  
            }
            else if (command.equals(MeltdownSerial.SPOKE))
            {
                // setSpokes(MeltdownLED.GetBoolValue());
            }
            else if (command.equals(MeltdownSerial.WHEEL))
            {
                // setSpokesOnly(MeltdownLED.GetBoolValue());
            }
            else if (command.equals(MeltdownSerial.BOTH))
            {
                // setWheelsOnly(MeltdownLED.GetBoolValue());
            }
            else if (command.equals(MeltdownSerial.EFFECT))
            {
                MeltdownLED.NextEffect();
                MeltdownLogger.Debug(Serial, "Next Effect...");  
            }
            else if (command.equals(MeltdownSerial.MODE))
            {
                int modeNumber = MeltdownLED.NextMode();
                MeltdownLogger.Debug(Serial, "Next Mode: ", modeNumber);
            }
            else if (command.equals(MeltdownSerial.ANALOG_EFFECT))
            {
                int currVal = MeltdownLED.GetAnalogEffect();
                int modeVal = MeltdownLED.SetAnalogEffect(-1);

                if (MeltdownSerial.HasChanged(currVal, modeVal))
                {
                    MeltdownLogger.Debug(Serial, "Setting Analog Effect: ", modeVal);   
                }
            }
            else if (command.equals(MeltdownSerial.ANALOG_PATTERN))
            {
                int currVal = MeltdownLED.GetAnalogPattern();
                int patternVal = MeltdownLED.SetAnalogPattern(-1);

                if (MeltdownSerial.HasChanged(currVal, patternVal))
                {
                    MeltdownLogger.Debug(Serial, "Setting Analog Pattern: ", patternVal);   
                }
            }
            else
            {
                MeltdownLogger.Debug(Serial, "Something went wrong reading serial command: ", command);
            }
        }
        else
        {
            MeltdownLogger.Debug(Serial, "Something went wrong reading serial, command was blank.");
        }

        MeltdownSerial.ClearInputString();
    }
}

void serialEvent()
{
    while (Serial1.available() && !MeltdownSerial.GetInputStringComplete())
    {
        // get the new byte:
        char inChar = (char)Serial1.read();
        // add it to the inputString:
        MeltdownSerial.AddCharToInputString(inChar);
        // if the incoming character is a newline, set a flag
        // so the main loop can do something about it:
        if (inChar == '\n')
        {
            Serial.println("Received input string: " + MeltdownSerial.GetInputString());
            MeltdownSerial.SetInputStringComplete(true);
        }
    }
}

#pragma endregion COMMANDS