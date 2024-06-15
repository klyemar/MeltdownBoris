#include <FastLED.h>
#include <MeltdownSerial.h>
#include <MeltdownLogger.h>
#include <MobileMeltdown.h>

// Glow Worms is an interactive light display made of multiple vacuum tubes with LEDs and polyfil to create a cloudy effect inside.
// It is controlled by several individual user inputs with a large number of combinations of effects possible.
// 
// Created by Kyle Hadley
//
// This project builds off of the FastLED library and some of its existing patterns
// and examples.

namespace Meltdown
{
#define DEBUG true

#define LED_TYPE WS2812B 
	const uint8_t gDataPin = 0; 
	const int gBrightness = 255; 
	const EOrder gOrder = GRB;

#define NUM_STRIPS 5
#define NUM_LEDS_PER_STRIP 170
#define NUM_LEDS (NUM_STRIPS * NUM_LEDS_PER_STRIP)

#define BUTTON_PIN_1 1
#define BUTTON_PIN_2 2
#define BUTTON_PIN_3 3
#define BUTTON_PIN_4 A4

#define PATTERN_PIN			BUTTON_PIN_1
#define MODE_PIN			BUTTON_PIN_2
#define SOLID_COLOR_PIN		BUTTON_PIN_3
#define ANALOG_PIN			BUTTON_PIN_4

CRGB leds[NUM_LEDS];

#pragma region PATTERNS

	void initPattern()
	{
		MobileMeltdown.SetPatternNumber(0);
		MeltdownLogger.Debug(Serial, "Initializing Pattern...");
	}

	void initMode()
	{
		MobileMeltdown.SetModeNumber(0);
		MeltdownLogger.Debug(Serial, "Initializing Mode...");
	}

	// TODO - Consider using effects, I'll need to think of more to make it worth it
	void initEffect()
	{
		MobileMeltdown.SetEffectNumber(0);
		MeltdownLogger.Debug(Serial, "Initializing Effect...");
	}

	void nextPattern()
	{
		// Reset the mode.
		initMode();

		// Set to black.
		//MobileMeltdown.SetAllColor(leds, CRGB::Black); // <-- TODO - Does this look okay without the set to black?
		MobileMeltdown.IncrementPatternNumber();
		MeltdownLogger.Debug(Serial, "Next Pattern...");
	}

	void nextMode()
	{
		int modeNumber = MobileMeltdown.IncrementModeNumber();
		MeltdownLogger.Debug(Serial, "Next Mode", modeNumber);
	}

	void nextEffect()
	{
		int effectNumber = MobileMeltdown.IncrementEffectNumber();
		MeltdownLogger.Debug(Serial, "Next Effect...");
	}

	void toggleSolidColor()
	{
		MobileMeltdown.ToggleSolidColor();
		MeltdownLogger.Debug(Serial, "Toggling Solid Color...");
	}

	void setAnalogPattern()
	{
		int currVal = MobileMeltdown.GetAnalogPattern();
		int patternVal = MobileMeltdown.SetAnalogPattern(ANALOG_PIN);

		if (MeltdownSerial.HasChanged(currVal, patternVal))
		{
			MeltdownLogger.Debug(Serial, "Setting Analog Pattern", patternVal);
		}
	}

	void displayDebugColors()
	{
		int hueStep = 255 / NUM_STRIPS;

		CHSV hsv;
		hsv.hue = 0;
		hsv.val = 255;
		hsv.sat = 240;

		for (int i = 0; i < NUM_STRIPS; i++)
		{
			for (int j = 0; j < NUM_LEDS_PER_STRIP; j++)
			{
				int hue = (i * hueStep) % 255;
				hsv.hue = hue;

				leds[j + (i * NUM_LEDS_PER_STRIP)] = hsv;
			}
		}
	}

#pragma endregion PATTERNS

#pragma region AUTO MODE

	void deactivateAutoMode()
	{
		MeltdownLogger.Debug(Serial, "Deactivating Auto Mode...");
		MobileMeltdown.SetAutoModeActive(false);
	}

	void executeAutoMode()
	{
		if (MobileMeltdown.GetAutoModeActive())
		{
			EVERY_N_SECONDS(30)
			{
				// If we've reached the limit of modes for this pattern, get the next pattern.
				if (MobileMeltdown.GetModeNumber() >= MobileMeltdown.GetNumModes())
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

#pragma endregion AUTO MODE

#pragma region BUTTONS

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
	Button solidColorButton = { SOLID_COLOR_PIN };

	void checkButtonState(Button *button)
	{
		// Read the state of the button pin.
		button->state = digitalRead(button->pin);

		// Check if the button is pressed. If it is, the buttonState is LOW.
		if (button->state == LOW && button->previousState == HIGH)
		{
			// This is awkward, but for now if this is the first that the code button has been pressed, we must activate it here.
			if (!MobileMeltdown.GetAutoModeActive())
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
			if (!MobileMeltdown.GetAutoModeActive())
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
			checkButtonState(&solidColorButton);
		}
	}

	void checkModifiers()
	{
		EVERY_N_MILLISECONDS(20)
		{
			setAnalogPattern();
		}
	}

	void setupButtons()
	{
		patternButton.callback = nextPattern;
		patternButton.isToggle = true;
		modeButton.callback = nextMode;
		modeButton.isToggle = true;
		solidColorButton.callback = toggleSolidColor;
		solidColorButton.isToggle = false;
	}

#pragma endregion BUTTONS

	void executeSetup()
	{
		// initialize serial communication at 9600 bits per second:
		Serial.begin(9600);

		Serial.println("Serial port opened.");

		MeltdownLogger.InitSerial(DEBUG);

		MobileMeltdown.InitTimers();

		delay(3000);

		setupButtons();

		MobileMeltdown.SetAutoModeActive(true);

		pinMode(PATTERN_PIN, INPUT_PULLUP);
		pinMode(MODE_PIN, INPUT_PULLUP);
		pinMode(SOLID_COLOR_PIN, INPUT_PULLUP);
		pinMode(ANALOG_PIN, INPUT);

		initPattern();
		initMode();
		initEffect(); 

		FastLED.addLeds<LED_TYPE, gDataPin, gOrder>(leds, NUM_LEDS);

		LEDS.setBrightness(gBrightness);
	}

	void executeLoop()
	{
		checkButtonStates();
		checkModifiers();

		if (MobileMeltdown.GetAutoModeActive())
		{
			executeAutoMode();
		}
		else if (MobileMeltdown.GetSolidColor())
		{
			MobileMeltdown.SetAllColor(leds, CRGB::Black);
		}
		else
		{
			MobileMeltdown.ExecutePattern(leds);
			MobileMeltdown.ExecuteEffect(leds);

			//MobileMeltdown.IncrementFrame(1.5f);
		}

		if (MobileMeltdown.GetDelay() > 0)
		{
			delay(MobileMeltdown.GetDelay());
		}

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