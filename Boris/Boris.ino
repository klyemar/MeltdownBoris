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

namespace Meltdown
{

#define DEBUG true

#define LED_TYPE OCTOWS2811

#define NUM_PENTS 1
#define NUM_STRIPS_PER_PENT 1

#define NUM_WHEEL_LEDS_PER_STRIP 160 //18
#define NUM_SPOKE_LEDS_PER_STRIP 130 //18
#define NUM_LEDS_PER_STRIP (NUM_WHEEL_LEDS_PER_STRIP + NUM_SPOKE_LEDS_PER_STRIP)
#define NUM_WHEEL_LEDS_PER_PENT (NUM_STRIPS_PER_PENT * NUM_WHEEL_LEDS_PER_STRIP)
#define NUM_SPOKE_LEDS_PER_PENT (NUM_STRIPS_PER_PENT * NUM_SPOKE_LEDS_PER_STRIP)
#define NUM_LEDS_PER_PENT (NUM_LEDS_PER_STRIP * NUM_STRIPS_PER_PENT)
#define NUM_WHEEL_LEDS (NUM_PENTS * NUM_WHEEL_LEDS_PER_PENT)
#define NUM_SPOKE_LEDS (NUM_PENTS * NUM_SPOKE_LEDS_PER_PENT)
#define NUM_PENT_LEDS (NUM_PENTS * NUM_LEDS_PER_PENT)
#define NUM_LEDS (NUM_PENT_LEDS)

	CRGB leds[NUM_LEDS];

	CRGB *ledWheelSets[NUM_WHEEL_LEDS];
	CRGB *ledSpokeSets[NUM_SPOKE_LEDS];

#pragma region COMMANDS

	void executeSleepPattern()
	{
		MeltdownLED.Sunrise(ledWheelSets, NUM_WHEEL_LEDS);
		MeltdownLED.Sunrise(ledSpokeSets, NUM_SPOKE_LEDS);
	}

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
					MeltdownLogger.Debug(Serial, "Toggling Hue", hueValue);
				}
				else if (command.equals(MeltdownSerial.HUE2))
				{
					bool hueValue = MeltdownLED.ToggleHue(2);
					MeltdownLogger.Debug(Serial, "Toggling Hue", hueValue);
				}
				else if (command.equals(MeltdownSerial.HUE3))
				{
					bool hueValue = MeltdownLED.ToggleHue(3);
					MeltdownLogger.Debug(Serial, "Toggling Hue", hueValue);
				}
				else if (command.equals(MeltdownSerial.HUE4))
				{
					bool hueValue = MeltdownLED.ToggleHue(4);
					MeltdownLogger.Debug(Serial, "Toggling Hue", hueValue);
				}
				else if (command.equals(MeltdownSerial.HUE5))
				{
					bool hueValue = MeltdownLED.ToggleHue(5);
					MeltdownLogger.Debug(Serial, "Toggling Hue", hueValue);
				}
				else if (command.equals(MeltdownSerial.PAUSE))
				{
					MeltdownLED.SetPause();
				}
				else if (command.equals(MeltdownSerial.PATTERN))
				{
					// Set to black.
					MeltdownLED.SetAllColor(ledWheelSets, NUM_WHEEL_LEDS, CRGB::Black);
					MeltdownLED.SetAllColor(ledSpokeSets, NUM_SPOKE_LEDS, CRGB::Black);
					MeltdownLED.NextPattern();
					MeltdownLogger.Debug(Serial, "Next Pattern");
				}
				else if (command.equals(MeltdownSerial.TOP))
				{
					bool topVal = MeltdownLED.ToggleTop();
					MeltdownLogger.Debug(Serial, "Setting top position", topVal);
				}
				else if (command.equals(MeltdownSerial.BOTTOM))
				{
					bool bottomVal = MeltdownLED.ToggleBottom();
					MeltdownLogger.Debug(Serial, "Setting bottom position", bottomVal);
				}
				else if (command.equals(MeltdownSerial.EFFECT))
				{
					MeltdownLED.NextEffect();
					MeltdownLogger.Debug(Serial, "Next Effect");
				}
				else if (command.equals(MeltdownSerial.MODE))
				{
					int modeNumber = MeltdownLED.NextMode();
					MeltdownLogger.Debug(Serial, "Next Mode", modeNumber);
				}
				else if (command.equals(MeltdownSerial.ANALOG_EFFECT))
				{
					int currVal = MeltdownLED.GetAnalogEffect();
					int modeVal = MeltdownLED.SetAnalogEffect(-1);

					if (MeltdownSerial.HasChanged(currVal, modeVal))
					{
						MeltdownLogger.Debug(Serial, "Setting Analog Effect", modeVal);
					}
				}
				else if (command.equals(MeltdownSerial.ANALOG_PATTERN))
				{
					int currVal = MeltdownLED.GetAnalogPattern();
					int patternVal = MeltdownLED.SetAnalogPattern(-1);

					if (MeltdownSerial.HasChanged(currVal, patternVal))
					{
						MeltdownLogger.Debug(Serial, "Setting Analog Pattern", patternVal);
					}
				}
				else if (command.equals(MeltdownSerial.SLEEP))
				{
					MeltdownLED.SetSleeping();
				}
				else
				{
					MeltdownLogger.Debug(Serial, "Something went wrong reading serial command", command);
				}
			}
			else
			{
				MeltdownLogger.Debug(Serial, "Something went wrong reading serial, command was blank.");
			}

			MeltdownSerial.ClearInputString();
		}
	}

#pragma endregion COMMANDS

	void setupLedArrays()
	{
		for (int i = 0; i < NUM_PENTS; i++)
		{
			for (int j = 0; j < NUM_STRIPS_PER_PENT; j++)
			{
				for (int k = 0; k < NUM_WHEEL_LEDS_PER_STRIP; k++)
				{
#if DEBUG
					Serial.print("ledWheelSets["); Serial.print((i * NUM_STRIPS_PER_PENT * NUM_WHEEL_LEDS_PER_STRIP) + (j * NUM_WHEEL_LEDS_PER_STRIP) + k); Serial.print("]: "); Serial.println((i * NUM_LEDS_PER_PENT) + (j * NUM_LEDS_PER_STRIP) + k);
#endif
					ledWheelSets[(i * NUM_LEDS_PER_PENT) + (j * NUM_WHEEL_LEDS_PER_STRIP) + k] = &leds[(i * NUM_LEDS_PER_PENT) + (j * NUM_LEDS_PER_STRIP) + k];
				}

				for (int k = 0; k < NUM_SPOKE_LEDS_PER_STRIP; k++)
				{
#if DEBUG
					Serial.print("ledSpokeSets["); Serial.print((i * NUM_STRIPS_PER_PENT * NUM_SPOKE_LEDS_PER_STRIP) + (j * NUM_SPOKE_LEDS_PER_STRIP) + k); Serial.print("]: "); Serial.println((i * NUM_LEDS_PER_PENT) + (j * NUM_LEDS_PER_STRIP) + NUM_WHEEL_LEDS_PER_STRIP + k);
#endif
					ledSpokeSets[(i * NUM_LEDS_PER_PENT) + (j * NUM_SPOKE_LEDS_PER_STRIP) + k] = &leds[(i * NUM_LEDS_PER_PENT) + (j * NUM_LEDS_PER_STRIP) + NUM_WHEEL_LEDS_PER_STRIP + k];
				}
			}
		}
	}

	void executeSetup()
	{
		// initialize serial communication at 9600 bits per second:
		Serial.begin(9600);
		Serial1.begin(9600);

		Serial.println("Serial port opened.");
		MeltdownLogger.InitSerial(DEBUG);

		delay(3000);
		LEDS.addLeds<OCTOWS2811>(leds, NUM_LEDS_PER_PENT);

		LEDS.setBrightness(MeltdownLED.GetBrightness());

		setupLedArrays();
	}

	void executeLoop()
	{
		tryExecuteCommand();

		if (!MeltdownLED.GetSleeping())
		{
			if (!MeltdownLED.GetPause())
			{
				if (MeltdownLED.GetTop())
				{
					// Call the current pattern function once, updating the 'leds' array.
					MeltdownLED.ExecutePattern(ledWheelSets, NUM_WHEEL_LEDS);
					MeltdownLED.ExecuteEffect(ledWheelSets, NUM_WHEEL_LEDS);
					MeltdownLED.SetAllColor(ledSpokeSets, NUM_SPOKE_LEDS, CRGB::Black);
				}
				else if (MeltdownLED.GetBottom())
				{
					// Call the current pattern function once, updating the 'leds' array.
					MeltdownLED.ExecutePattern(ledSpokeSets, NUM_SPOKE_LEDS);
					MeltdownLED.ExecuteEffect(ledSpokeSets, NUM_SPOKE_LEDS);
					MeltdownLED.SetAllColor(ledWheelSets, NUM_WHEEL_LEDS, CRGB::Black);
				}
				else
				{
					// Call the current pattern function once, updating the 'leds' array.
					MeltdownLED.ExecutePattern(ledWheelSets, NUM_WHEEL_LEDS);
					MeltdownLED.ExecutePattern(ledSpokeSets, NUM_SPOKE_LEDS);
					MeltdownLED.ExecuteEffect(ledWheelSets, NUM_WHEEL_LEDS);
					MeltdownLED.ExecuteEffect(ledSpokeSets, NUM_SPOKE_LEDS);
				}
			}
		}
		else
		{
			executeSleepPattern();
		}

		LEDS.show();
	}

	void executeSerialEvent()
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
				MeltdownLogger.Debug(Serial, "Received input string", MeltdownSerial.GetInputString());
				MeltdownSerial.SetInputStringComplete(true);
			}
		}
	}
}

void setup()
{
	Meltdown::executeSetup();
}

void loop()
{
	Meltdown::executeLoop();
}

void serialEvent()
{
	Meltdown::executeSerialEvent();
}