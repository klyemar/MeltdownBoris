#include "FastLED.h"
#include "MeltdownLED.h"
#include "MeltdownLogger.h"

// Boris, short for Explora Borealis, is an interactive light display
// controlled by several individual user inputs.
//
// This project builds off of the FastLED library and some of its existing patterns
// and examples.

namespace Meltdown
{
#define DEBUG true

//#define LED_PIN 0 // Trinket M0
#define LED_PIN 11 // Metro Mini

#define BUTTON_PIN_1 2
#define BUTTON_PIN_2 3
#define BUTTON_PIN_3 4

#define PATTERN_PIN BUTTON_PIN_1
#define MODE_PIN BUTTON_PIN_2
#define EFFECT_PIN BUTTON_PIN_3

#define NUM_LEDS_PER_STRIP 20 // Neon Lights
//#define NUM_LEDS_PER_STRIP 50 // Christmas Lights
//#define NUM_LEDS_PER_STRIP 100 // Christmas Lights x2
//#define NUM_LEDS_PER_STRIP 93 // Circle Lamp
//#define NUM_LEDS_PER_STRIP 34 // 60ppm Umbrella
//#define NUM_LEDS_PER_STRIP 17 // 30ppm Umbrella

#define NUM_STRIPS 1 // Non-Umbrella
//#define NUM_STRIPS 8 // Umbrella
#define NUM_LEDS NUM_LEDS_PER_STRIP * NUM_STRIPS

#define LED_TYPE WS2811 // Neon Lights
//#define LED_TYPE WS2812B // Other Lights

#define RGB_ORDER GRB // Umbrella and Lamp
//#define RGB_ORDER RGB // Christmas Lights and Neon Lights

	CRGB leds[NUM_LEDS];
	uint16_t ledIndexes[NUM_LEDS];

	struct Button
	{
		int pin;
		int state;
		int previousState;
		bool isToggle;
		void(*callback)();

		Button(int pin) : pin(pin), state{ HIGH }, previousState{ HIGH }, isToggle{ false }
		{}
	};

	Button patternButton = { PATTERN_PIN };
	Button modeButton = { MODE_PIN };
	Button effectButton = { EFFECT_PIN };

#pragma region PATTERNS

	void setColor(CRGB ledSets[], int numLeds, CRGB::HTMLColorCode color)
	{
		for (int i = 0; i < numLeds; i++)
		{
			ledSets[i] = color;
		}
	}

	void setAllColor(CRGB::HTMLColorCode color)
	{
		setColor(leds, NUM_LEDS, color);
	}

	void initPattern()
	{
		MeltdownLED.SetPatternNumber(0);
		MeltdownLogger.Debug(Serial, "Initializing Pattern...");
	}

	void initMode()
	{
		MeltdownLED.SetModeNumber(0);
		MeltdownLogger.Debug(Serial, "Initializing Mode...");
	}

	void initEffect()
	{
		MeltdownLED.SetEffectNumber(0);
		MeltdownLogger.Debug(Serial, "Initializing Effect...");
	}

	void nextPattern()
	{
		// Reset the mode.
		initMode();

		// Set to black.
		MeltdownLED.SetAllColor(leds, ledIndexes, NUM_LEDS, CRGB::Black);
		MeltdownLED.IncrementPatternNumber();
		MeltdownLogger.Debug(Serial, "Next Pattern...");
	}

	void nextMode()
	{
		int modeNumber = MeltdownLED.IncrementModeNumber();
		MeltdownLogger.Debug(Serial, "Next Mode", modeNumber);
	}

	void nextEffect()
	{
		int effectNumber = MeltdownLED.IncrementEffectNumber();
		MeltdownLogger.Debug(Serial, "Next Effect...");
	}

	void executeAutoMode()
	{
		if (!MeltdownLED.GetAutoModeActive()) return;

		if (MeltdownLED.IsAutoPattern())
		{
			EVERY_N_SECONDS(15) { nextPattern(); }
		}
		else if (MeltdownLED.IsAutoMode())
		{
			EVERY_N_SECONDS(10) { nextMode(); }
		}
		else if (MeltdownLED.IsAutoPatternMode())
		{
			EVERY_N_SECONDS(30)
			{
				// If we've reached the limit of modes for this pattern, get the next pattern.
				if (MeltdownLED.GetModeNumber() >= MeltdownLED.GetNumModes())
				{
					nextPattern();
				}
				else
				{
					nextMode();
				}
			}
		}
	}

#pragma endregion PATTERNS

#pragma region SET MODIFIERS

	void deactivateAutoMode()
	{
		MeltdownLogger.Debug(Serial, "Deactivating Auto Mode...");
		MeltdownLED.SetAutoModeActive(false);
	}

#pragma endregion SET MODIFIERS

#pragma region INPUTS

	void checkButtonState(Button *button)
	{
		// Read the state of the button pin.
		button->state = digitalRead(button->pin);

		// Check if the button is pressed. If it is, the buttonState is LOW.
		if (button->state == LOW && button->previousState == HIGH)
		{
			// This is awkward, but for now if this is the first that the code button has been pressed, we must activate it here.
			if (!MeltdownLED.GetAutoModeActive())
			{
				button->callback();
			}
			else
			{
				deactivateAutoMode();
			}

			button->previousState = LOW;
		}
		// Check if the button is depressed. If it is, the buttonState is HIGH.
		else if (button->state == HIGH && button->previousState == LOW)
		{
			// If auto mode is not active, call the button callback. Otherwise, deactivate auto mode.
			if (!MeltdownLED.GetAutoModeActive())
			{
				if (!button->isToggle)
				{
					button->callback();
				}
			}
			else
			{
				deactivateAutoMode();
			}

			button->previousState = HIGH;
		}
	}

	void checkButtonStates()
	{
		EVERY_N_MILLISECONDS(50)
		{
			checkButtonState(&patternButton);
			checkButtonState(&modeButton);
			checkButtonState(&effectButton);
		}
	}

#pragma endregion INPUTS

	void setupButtons()
	{
		patternButton.callback = nextPattern;
		patternButton.isToggle = true;
		modeButton.callback = nextMode;
		modeButton.isToggle = true;
		effectButton.callback = nextEffect;
		effectButton.isToggle = true;
	}

	void clearLedIndexes()
	{
		for (int i = 0; i < NUM_LEDS; i++)
		{
			ledIndexes[i] = 0;
		}
	}

	void setLedIndexesForAll()
	{
		clearLedIndexes();

		for (int i = 0; i < NUM_LEDS; i++)
		{
			ledIndexes[i] = i;
		}
	}

	void setLedIndexesForStrip(int stripNumber)
	{
		clearLedIndexes();

		for (int i = 0; i < NUM_LEDS_PER_STRIP; i++)
		{
			ledIndexes[i] = (stripNumber * NUM_LEDS_PER_STRIP) + i;
		}
	}

	void executeSetup()
	{
		// initialize serial communication at 9600 bits per second:
		//Serial.begin(9600);
		//Serial1.begin(9600);

		Serial.println("Serial port opened.");

		MeltdownLogger.InitSerial(DEBUG);

		MeltdownLED.InitTimers();

		delay(3000);

		FastLED.addLeds<LED_TYPE, LED_PIN, RGB_ORDER>(leds, NUM_LEDS);

		LEDS.setBrightness(110);

		setupButtons();

		setLedIndexesForAll();

		MeltdownLED.SetAutoMode(MeltdownLED.PatternMode);

		pinMode(BUTTON_PIN_1, INPUT_PULLUP);
		pinMode(BUTTON_PIN_2, INPUT_PULLUP);
		pinMode(BUTTON_PIN_3, INPUT_PULLUP);

		initPattern();
		initMode();
		initEffect();
	}

	void executeLoop()
	{
		checkButtonStates();

		if (MeltdownLED.GetAutoModeActive())
		{
			executeAutoMode();
		}

		setLedIndexesForAll();
		MeltdownLED.ExecutePattern(leds, ledIndexes, NUM_LEDS, 0, 0);
		MeltdownLED.ExecuteEffect(leds, ledIndexes, NUM_LEDS, 0);

		MeltdownLED.IncrementFrame();

    delay(10);

		LEDS.show();
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
