#include <FastLED.h>
#include <MeltdownSerial.h>
#include <MeltdownLogger.h>
#include <MobileMeltdown.h>

// Boris, short for Explora Borealis, is an interactive light display
// controlled by several individual user inputs.
//
// This project builds off of the FastLED library and some of its existing patterns
// and examples.

namespace Meltdown
{
#define DEBUG true

	// Circle Lamp
//#define LED_TYPE WS2812B 
	//const uint8_t gDataPin = 0; const int gNumLedsPerStrip = 93; const int gNumStrips = 1; const int gBrightness = 50; const EOrder gOrder = GRB; 

	// Neon Light
//#define LED_TYPE WS2811 
	//const uint8_t gDataPin = 11; const int gNumLedsPerStrip = 20; const int gNumStrips = 1; const int gBrightness = 110; const EOrder gOrdergOrder = RGB; 

	// Christmas Bulbs
#define LED_TYPE WS2812B 
	const uint8_t gDataPin = 0; const int gNumLedsPerStrip = 50; const int gNumStrips = 1; const int gBrightness = 110; const EOrder gOrder = RGB;

	// Christmas Bulbs XL
//#define LED_TYPE WS2812B 
	//const uint8_t gDataPin = 0; const int gNumLedsPerStrip = 100; const int gNumStrips = 1; const int gBrightness = 110; const EOrder gOrder = RGB;

	// Umbrella
//#define LED_TYPE WS2812B 
	//const uint8_t gDataPin = 0; const int gNumLedsPerStrip = 17; const int gNumStrips = 8; const int gBrightness = 110; const EOrder gOrder = GRB; 

	// Umbrella XL
//#define LED_TYPE WS2812B 
	//const uint8_t gDataPin = 0; const int gNumLedsPerStrip = 34; const int gNumStrips = 8; const int gBrightness = 110; const EOrder gOrder = GRB; 

	// Arbitrary Config
//#define LED_TYPE WS2812B 
	//const uint8_t gDataPin = 0; const int gNumLedsPerStrip = 425; const int gNumStrips = 1; const int gBrightness = 110; const EOrder gOrder = GRB; 

#define BUTTON_PIN_1 2
#define BUTTON_PIN_2 3
#define BUTTON_PIN_3 4

#define PATTERN_PIN BUTTON_PIN_1
#define MODE_PIN BUTTON_PIN_2
#define EFFECT_PIN BUTTON_PIN_3

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

	CRGB leds[gNumLedsPerStrip * gNumStrips];

#pragma region PATTERNS

	void initLeds()
	{
		MobileMeltdown.SetNumLeds(gNumLedsPerStrip * gNumStrips);
		MeltdownLogger.Debug(Serial, "Initializing LEDs...");
	}

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
		MobileMeltdown.SetAllColor(leds, CRGB::Black);
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

	void executeAutoMode()
	{
		if (!MobileMeltdown.GetAutoModeActive()) return;

		if (MobileMeltdown.IsAutoPattern())
		{
			EVERY_N_SECONDS(15) { nextPattern(); }
		}
		else if (MobileMeltdown.IsAutoMode())
		{
			EVERY_N_SECONDS(10) { nextMode(); }
		}
		else if (MobileMeltdown.IsAutoPatternMode())
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

#pragma endregion PATTERNS

#pragma region SET MODIFIERS

	void deactivateAutoMode()
	{
		MeltdownLogger.Debug(Serial, "Deactivating Auto Mode...");
		MobileMeltdown.SetAutoModeActive(false);
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

	void executeSetup()
	{
		// initialize serial communication at 9600 bits per second:
		Serial.begin(9600);
		Serial1.begin(9600);

		Serial.println("Serial port opened.");

		MeltdownLogger.InitSerial(DEBUG);

		MobileMeltdown.InitTimers();

		delay(3000);

		setupButtons();

		MobileMeltdown.SetAutoMode(MobileMeltdown.PatternMode);

		pinMode(BUTTON_PIN_1, INPUT_PULLUP);
		pinMode(BUTTON_PIN_2, INPUT_PULLUP);
		pinMode(BUTTON_PIN_3, INPUT_PULLUP);

		initLeds();
		initPattern();
		initMode();
		initEffect();

		FastLED.addLeds<LED_TYPE, gDataPin, gOrder>(leds, gNumLedsPerStrip * gNumStrips);

		LEDS.setBrightness(gBrightness);
	}

	void executeLoop()
	{
		checkButtonStates();

		if (MobileMeltdown.GetAutoModeActive())
		{
			executeAutoMode();
		}

		MobileMeltdown.ExecutePattern(leds);
		MobileMeltdown.ExecuteEffect(leds);

		MobileMeltdown.IncrementFrame();

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
