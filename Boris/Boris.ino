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

#define NUM_WHEEL_LEDS_PER_STRIP 299
#define NUM_SPOKE_LEDS_PER_STRIP 1
#define NUM_LEDS_PER_STRIP (NUM_WHEEL_LEDS_PER_STRIP + NUM_SPOKE_LEDS_PER_STRIP)
#define NUM_WHEEL_LEDS_PER_PENT (NUM_STRIPS_PER_PENT * NUM_WHEEL_LEDS_PER_STRIP)
#define NUM_SPOKE_LEDS_PER_PENT (NUM_STRIPS_PER_PENT * NUM_SPOKE_LEDS_PER_STRIP)
#define NUM_LEDS_PER_PENT (NUM_LEDS_PER_STRIP * NUM_STRIPS_PER_PENT)
#define NUM_WHEEL_LEDS (NUM_PENTS * NUM_WHEEL_LEDS_PER_PENT)
#define NUM_SPOKE_LEDS (NUM_PENTS * NUM_SPOKE_LEDS_PER_PENT)
#define NUM_LEDS (NUM_PENTS * NUM_LEDS_PER_PENT)

	CRGB leds[NUM_LEDS];
	uint16_t ledIndexes[NUM_LEDS];

	bool gDuplicatePents = true;

#pragma region INDEX SETUP

	const String INDEXES_NONE = "NONE";
	const String INDEXES_ALL = "ALL";
	const String INDEXES_WHEELS = "WHEELS";
	const String INDEXES_WHEEL = "WHEEL";
	const String INDEXES_SPOKES = "SPOKES";
	const String INDEXES_SPOKE = "SPOKE";

	String gSelectedIndexes = INDEXES_NONE;

	void clearLedIndexes()
	{
		for (int i = 0; i < NUM_LEDS; i++)
		{
			ledIndexes[i] = 0;
		}
	}

	void setIndexesForAll()
	{
		if (gSelectedIndexes != INDEXES_ALL)
		{
			gSelectedIndexes = INDEXES_ALL;

			for (int i = 0; i < NUM_LEDS; i++)
			{
				ledIndexes[i] = i;
			}
		}
	}

	/// Set the indexes for all wheel sections, treating them as one long contiguous strip.
	void setIndexesForWheels()
	{
		if (gSelectedIndexes != INDEXES_WHEELS)
		{
			gSelectedIndexes = INDEXES_WHEELS;

			clearLedIndexes();
			for (int i = 0; i < NUM_PENTS; i++)
			{
				for (int j = 0; j < NUM_STRIPS_PER_PENT; j++)
				{
					// Temporary workaround for hardware fault.
					if (i == 3 && j == 4) continue;

					for (int k = 0; k < NUM_WHEEL_LEDS_PER_STRIP; k++)
					{
						ledIndexes[(i * NUM_WHEEL_LEDS_PER_PENT) + (j * NUM_WHEEL_LEDS_PER_STRIP) + k] = (i * NUM_LEDS_PER_PENT) + (j * NUM_LEDS_PER_STRIP) + k;
					}
				}
			}
		}
	}

	/// Set the indexes for the wheel section of a single pent.
	void setIndexesForWheel(int pentNumber)
	{
		if (gSelectedIndexes != INDEXES_WHEEL + pentNumber)
		{
			gSelectedIndexes = INDEXES_WHEEL + pentNumber;

			clearLedIndexes();
			for (int j = 0; j < NUM_STRIPS_PER_PENT; j++)
			{
				// Temporary workaround for hardware fault.
				if (pentNumber == 3 && j == 4) continue;

				for (int k = 0; k < NUM_WHEEL_LEDS_PER_STRIP; k++)
				{
					ledIndexes[(j * NUM_WHEEL_LEDS_PER_STRIP) + k] = (pentNumber * NUM_LEDS_PER_PENT) + (j * NUM_LEDS_PER_STRIP) + k;
				}
			}
		}
	}

	/// Set the indexes for all spoke sections, treating them as one long contigous strip.
	void setIndexesForSpokes()
	{
		if (gSelectedIndexes != INDEXES_SPOKES)
		{
			gSelectedIndexes = INDEXES_SPOKES;

			clearLedIndexes();
			for (int i = 0; i < NUM_PENTS; i++)
			{
				for (int j = 0; j < NUM_STRIPS_PER_PENT; j++)
				{
					for (int k = 0; k < NUM_SPOKE_LEDS_PER_STRIP; k++)
					{
						ledIndexes[(i * NUM_SPOKE_LEDS_PER_PENT) + (j * NUM_SPOKE_LEDS_PER_STRIP) + k] = (i * NUM_LEDS_PER_PENT) + (j * NUM_LEDS_PER_STRIP) + NUM_WHEEL_LEDS_PER_STRIP + k;
					}
				}
			}
		}
	}

	/// Set the indexes for the spokes of a single pent.
	void setIndexesForSpokes(int pentNumber)
	{
		if (gSelectedIndexes != INDEXES_SPOKE + pentNumber)
		{
			gSelectedIndexes = INDEXES_SPOKE + pentNumber;

			clearLedIndexes();
			for (int j = 0; j < NUM_STRIPS_PER_PENT; j++)
			{
				for (int k = 0; k < NUM_SPOKE_LEDS_PER_STRIP; k++)
				{
					ledIndexes[(j * NUM_SPOKE_LEDS_PER_STRIP) + k] = (pentNumber * NUM_LEDS_PER_PENT) + (j * NUM_LEDS_PER_STRIP) + NUM_WHEEL_LEDS_PER_STRIP + k;
				}
			}
		}
	}


#pragma endregion INDEX SETUP

	void displayDebugColors()
	{
		for (int i = 0; i < NUM_PENTS; i++)
		{
			for (int j = 0; j < NUM_STRIPS_PER_PENT; j++)
			{
				for (int k = 0; k <= j; k++)
				{
					leds[(i * NUM_LEDS_PER_PENT) + (j * NUM_LEDS_PER_STRIP) + k] = CRGB::Blue;
				}

				for (int k = 0; k <= i; k++)
				{
					leds[(i * NUM_LEDS_PER_PENT) + k] = CRGB::Red;
				}
			}
		}
	}

#pragma region COMMANDS

	void executeSleepPattern()
	{
		setIndexesForAll();
		MeltdownLED.Sunrise(leds, ledIndexes, NUM_LEDS);
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
					MeltdownLogger.Debug(Serial, F("Toggling Hue"), hueValue);
				}
				else if (command.equals(MeltdownSerial.HUE2))
				{
					bool hueValue = MeltdownLED.ToggleHue(2);
					MeltdownLogger.Debug(Serial, F("Toggling Hue"), hueValue);
				}
				else if (command.equals(MeltdownSerial.HUE3))
				{
					bool hueValue = MeltdownLED.ToggleHue(3);
					MeltdownLogger.Debug(Serial, F("Toggling Hue"), hueValue);
				}
				else if (command.equals(MeltdownSerial.HUE4))
				{
					bool hueValue = MeltdownLED.ToggleHue(4);
					MeltdownLogger.Debug(Serial, F("Toggling Hue"), hueValue);
				}
				else if (command.equals(MeltdownSerial.HUE5))
				{
					bool hueValue = MeltdownLED.ToggleHue(5);
					MeltdownLogger.Debug(Serial, F("Toggling Hue"), hueValue);
				}
				else if (command.equals(MeltdownSerial.PAUSE))
				{
					MeltdownLED.SetPause();
				}
				else if (command.equals(MeltdownSerial.BLACK))
				{
					MeltdownLED.SetBlack();
				}
				else if (command.equals(MeltdownSerial.MIRROR))
				{
					MeltdownLED.SetMirror();
				}
				else if (command.equals(MeltdownSerial.PATTERN))
				{
					// Set to black.
					setIndexesForAll();
					MeltdownLED.SetAllColor(leds, ledIndexes, NUM_LEDS, CRGB::Black);

					int patternNumber = MeltdownLED.SetPatternNumber();
					MeltdownLogger.Debug(Serial, F("Setting pattern number"), patternNumber);
				}
				else if (command.equals(MeltdownSerial.TOP))
				{
					bool topVal = MeltdownLED.ToggleTop();
					MeltdownLogger.Debug(Serial, F("Setting top position"), topVal);
				}
				else if (command.equals(MeltdownSerial.BOTTOM))
				{
					bool bottomVal = MeltdownLED.ToggleBottom();
					MeltdownLogger.Debug(Serial, F("Setting bottom position"), bottomVal);
				}
				else if (command.equals(MeltdownSerial.EFFECT))
				{
					int effectNumber = MeltdownLED.SetEffectNumber();
					MeltdownLogger.Debug(Serial, F("Setting effect number"), effectNumber);
				}
				else if (command.equals(MeltdownSerial.MODE))
				{
					int modeNumber = MeltdownLED.SetModeNumber();
					MeltdownLogger.Debug(Serial, F("Setting mode number"), modeNumber);
				}
				else if (command.equals(MeltdownSerial.ANALOG_EFFECT))
				{
					int currVal = MeltdownLED.GetAnalogEffect();
					int modeVal = MeltdownLED.SetAnalogEffect(-1);

					if (MeltdownSerial.HasChanged(currVal, modeVal))
					{
						MeltdownLogger.Debug(Serial, F("Setting Analog Effect"), modeVal);
					}
				}
				else if (command.equals(MeltdownSerial.ANALOG_PATTERN))
				{
					int currVal = MeltdownLED.GetAnalogPattern();
					int patternVal = MeltdownLED.SetAnalogPattern(-1);

					if (MeltdownSerial.HasChanged(currVal, patternVal))
					{
						MeltdownLogger.Debug(Serial, F("Setting Analog Pattern"), patternVal);
					}
				}
				else if (command.equals(MeltdownSerial.AUTO_NONE))
				{
					MeltdownLED.SetAutoMode(MeltdownLED.None);
					MeltdownLogger.Debug(Serial, F("Disabling Auto Mode."));
				}
				else if (command.equals(MeltdownSerial.AUTO_PATTERN))
				{
					MeltdownLED.SetAutoMode(MeltdownLED.Pattern);
					MeltdownLogger.Debug(Serial, F("Setting Auto Pattern Mode."));
				}
				else if (command.equals(MeltdownSerial.AUTO_MODE))
				{
					MeltdownLED.SetAutoMode(MeltdownLED.Mode);
					MeltdownLogger.Debug(Serial, F("Setting Auto Mode Mode."));
				}
				else if (command.equals(MeltdownSerial.AUTO_PATTERN_MODE))
				{
					MeltdownLED.SetAutoMode(MeltdownLED.PatternMode);
					MeltdownLogger.Debug(Serial, F("Setting Auto Pattern Mode Mode."));
				}
				else if (command.equals(MeltdownSerial.AUTO_SLEEP))
				{
					MeltdownLED.SetAutoMode(MeltdownLED.Sleep);
					MeltdownLogger.Debug(Serial, F("Setting Auto Sleep Mode."));
				}
				else
				{
					MeltdownLogger.Debug(Serial, F("Something went wrong reading serial command"), command);
				}
			}
			else
			{
				MeltdownLogger.Debug(Serial, F("Something went wrong reading serial, command was blank."));
			}

			MeltdownSerial.ClearInputString();
		}
	}

#pragma endregion COMMANDS

	void executeSetup()
	{
		// initialize serial communication at 9600 bits per second:
		Serial.begin(9600);
		Serial1.begin(9600);

		Serial.println(F("Serial port opened."));
		MeltdownLogger.InitSerial(DEBUG);

		delay(3000);

		LEDS.addLeds<OCTOWS2811>(leds, NUM_LEDS_PER_PENT);

		LEDS.setBrightness(MeltdownLED.GetBrightness());
	}

	void executeWheels(int numLeds)
	{
		if (MeltdownLED.GetBottom())
		{
			// Call the current pattern function once, updating the 'leds' array.
			MeltdownLED.SetAllColor(leds, ledIndexes, numLeds, CRGB::Black);
		}
		else
		{
			// Call the current pattern function once, updating the 'leds' array.
			MeltdownLED.ExecutePattern(leds, ledIndexes, numLeds);
			MeltdownLED.ExecuteEffect(leds, ledIndexes, numLeds);
		}
	}

	void executeSpokes(int numLeds)
	{
		if (MeltdownLED.GetTop())
		{
			// Call the current pattern function once, updating the 'leds' array.
			MeltdownLED.SetAllColor(leds, ledIndexes, numLeds, CRGB::Black);
		}
		else
		{
			// Call the current pattern function once, updating the 'leds' array.
			MeltdownLED.ExecutePattern(leds, ledIndexes, numLeds);
			MeltdownLED.ExecuteEffect(leds, ledIndexes, numLeds);
		}
	}

	void executeLoop()
	{
		tryExecuteCommand();

		if (!MeltdownLED.GetPause() && !MeltdownLED.GetBlack())
		{
			if (MeltdownLED.GetMirror())
			{
				for (int i = 0; i < NUM_PENTS; i++)
				{
					// WHEELS
					setIndexesForWheel(i);
					executeWheels(NUM_WHEEL_LEDS_PER_PENT);

					// SPOKES
					setIndexesForSpokes(i);
					executeSpokes(NUM_SPOKE_LEDS_PER_PENT);
				}

				MeltdownLED.IncrementFrame();
			}
			else
			{
				// WHEELS
				setIndexesForWheels();
				executeWheels(NUM_WHEEL_LEDS);

				// SPOKES
				setIndexesForSpokes();
				executeSpokes(NUM_SPOKE_LEDS);

				MeltdownLED.IncrementFrame(1.5f);
			}
		}
		else if (MeltdownLED.GetBlack())
		{
			// Set to black.
			setIndexesForAll();
			MeltdownLED.SetAllColor(leds, ledIndexes, NUM_LEDS, CRGB::Black);
		}

		if (MeltdownLED.GetDelay() > 0)
		{
			delay(MeltdownLED.GetDelay());
		}

		//displayDebugColors();

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
				MeltdownLogger.Debug(Serial, F("Received input string"), MeltdownSerial.GetInputString());
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

void serialEvent1()
{
	Meltdown::executeSerialEvent();
}