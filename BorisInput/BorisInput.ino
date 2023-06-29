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

#define RING_PIN_1 46
#define RING_PIN_2 49
#define RING_PIN_3 48
#define RING_PIN_4 51
#define RING_PIN_5 53

#define BUTTON_PIN_1 0 // 1
#define BUTTON_PIN_2 4 // 2
#define BUTTON_PIN_3 8 // 3
#define BUTTON_PIN_4 12 // 4
#define BUTTON_PIN_5 17 // 5
#define BUTTON_PIN_6 25 // 6
#define BUTTON_PIN_7 29 // 8
#define BUTTON_PIN_8 33 // 7
#define BUTTON_PIN_9 37 // 9
#define BUTTON_PIN_10 41 // 10
#define BUTTON_PIN_11 45
#define ANALOG_PIN_1 A4
#define ANALOG_PIN_2 A0

#define PATTERN_PIN BUTTON_PIN_1
#define PAUSE_PIN BUTTON_PIN_2
#define MODE_PIN BUTTON_PIN_3
#define PURPLE_PIN BUTTON_PIN_4
#define YELLOW_PIN BUTTON_PIN_5
#define BLACK_PIN BUTTON_PIN_9
#define RAINBOW_PIN BUTTON_PIN_10
#define MIRROR_PIN BUTTON_PIN_11
#define EFFECT_PIN BUTTON_PIN_6
#define BOTTOM_PIN BUTTON_PIN_7
#define TOP_PIN BUTTON_PIN_8
#define ANALOG_PATTERN_PIN ANALOG_PIN_1
#define ANALOG_EFFECT_PIN ANALOG_PIN_2
#define PARTIAL_RING_PIN ANALOG_PIN_2

#define CODE_INIT_PIN PAUSE_PIN
#define CODE_PIN_1 BLACK_PIN
#define CODE_PIN_2 RAINBOW_PIN
#define CODE_PIN_3 YELLOW_PIN
#define CODE_PIN_4 PURPLE_PIN

#define NUM_LEDS_PER_MED_RING 16
#define NUM_LEDS_PER_LARGE_RING 24

	CRGB patternRingLeds[NUM_LEDS_PER_LARGE_RING];
	CRGB effectValRingLeds[NUM_LEDS_PER_LARGE_RING];
	CRGB positionRingLeds[NUM_LEDS_PER_LARGE_RING];
	CRGB modeRingLeds[NUM_LEDS_PER_MED_RING];
	CRGB effectRingLeds[NUM_LEDS_PER_MED_RING];

	uint16_t medRingIndexes[NUM_LEDS_PER_MED_RING];
	uint16_t largeRingIndexes[NUM_LEDS_PER_LARGE_RING];

	int gRingHue = 0;

	const int gButtonCodeLength = 5;
	int gButtonCode[gButtonCodeLength] = { 0, 0, 0, 0, 0 };

	// Ring options.
	enum RingPosition { Top, Bottom, Full, Empty, Partial };

	struct Button
	{
		int pin;
		int state;
		int previousState;
		bool isToggle;
		void(*pressedCallback)();
		void(*depressedCallback)();

		Button(int pin) : pin(pin), state{ HIGH }, previousState{ HIGH }, isToggle{ false }, depressedCallback{NULL}
		{}
	};

	Button patternButton = { PATTERN_PIN };
	Button effectButton = { EFFECT_PIN };
	Button modeButton = { MODE_PIN };
	Button topButton = { TOP_PIN };
	Button bottomButton = { BOTTOM_PIN };
	Button purpleButton = { PURPLE_PIN };
	Button yellowButton = { YELLOW_PIN };
	Button rainbowButton = { RAINBOW_PIN };
	Button blackButton = { BLACK_PIN };
	Button mirrorButton = { MIRROR_PIN };
	Button pauseButton = { PAUSE_PIN };

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
		setColor(patternRingLeds, NUM_LEDS_PER_LARGE_RING, color);
		setColor(effectValRingLeds, NUM_LEDS_PER_LARGE_RING, color);
		setColor(positionRingLeds, NUM_LEDS_PER_LARGE_RING, color);
		setColor(modeRingLeds, NUM_LEDS_PER_MED_RING, color);
		setColor(effectRingLeds, NUM_LEDS_PER_MED_RING, color);
	}

	int getRingLedCount(int numLeds)
	{
		static int ledCount = 0;
		int newVal = MeltdownSerial.GetAnalogValue(PARTIAL_RING_PIN, ledCount);

		if (MeltdownSerial.HasChanged(ledCount, newVal))
		{
			ledCount = map(newVal, 0, 1023, 0, numLeds + 1);
			gRingHue = map(newVal, 0, 1023, 0, 255);
		}

		return ledCount;
	}

	bool canColorRingLed(int index, int numLeds, RingPosition position)
	{
		if (position == Full)
			return true;
		if (position == Empty)
			return false;
		if (position == Top)
			return index > (numLeds / 2);
		if (position == Bottom)
			return index <= (numLeds / 2);
		if (position == Partial)
			return index <= getRingLedCount(numLeds);

		return false;
	}

	void setRingColor(CRGB ringLeds[], int numLeds, RingPosition position, int hueOffset)
	{
		setColor(ringLeds, numLeds, CRGB::Black);

		for (int i = 0; i < numLeds; i++)
		{
			if (canColorRingLed(i + 1, numLeds, position))
			{
				ringLeds[i] = CHSV(gRingHue + hueOffset, 255, 192);
			}
		}
	}

	void executeAutoPattern()
	{
		static int hue = 0;

		setAllColor(CRGB::Black);
		setRingColor(patternRingLeds, NUM_LEDS_PER_LARGE_RING, Full, hue);

		EVERY_N_MILLIS(50)
		{
			hue++;
		}
	}

	void initPattern()
	{
		int patternNumber = MeltdownLED.SetPatternNumber(0);

		MeltdownLogger.Debug(Serial, "Initializing Pattern...");
		MeltdownSerial.SendCommand(Serial, Serial1, MeltdownSerial.PATTERN, patternNumber);
	}

	void initEffect()
	{
		int effectNumber = MeltdownLED.SetEffectNumber(0);

		MeltdownLogger.Debug(Serial, "Initializing Effect...");
		MeltdownSerial.SendCommand(Serial, Serial1, MeltdownSerial.EFFECT, effectNumber);
	}

	void initMode()
	{
		int modeNumber = MeltdownLED.SetModeNumber(0);

		MeltdownLogger.Debug(Serial, "Initializing Mode...");
		MeltdownSerial.SendCommand(Serial, Serial1, MeltdownSerial.MODE, modeNumber);
	}

	void initAuto()
	{
		MeltdownLED.SetAutoMode(MeltdownLED.PatternMode);

		MeltdownLogger.Debug(Serial, "Initializing Auto Mode...");
		MeltdownSerial.SendCommand(Serial, Serial1, MeltdownSerial.AUTO_PATTERN_MODE, 1);
	}

	void nextPattern()
	{
		// Reset the mode.
		initMode();

		// Set to black.
		MeltdownLED.SetAllColor(patternRingLeds, largeRingIndexes, NUM_LEDS_PER_LARGE_RING, CRGB::Black);
		int patternNumber = MeltdownLED.IncrementPatternNumber();

		MeltdownLogger.Debug(Serial, "Next Pattern...");
		MeltdownSerial.SendCommand(Serial, Serial1, MeltdownSerial.PATTERN, patternNumber);
	}

	void nextEffect()
	{
		int effectNumber = MeltdownLED.IncrementEffectNumber();

		MeltdownLogger.Debug(Serial, "Next Effect...");
		MeltdownSerial.SendCommand(Serial, Serial1, MeltdownSerial.EFFECT, effectNumber);
	}

	void nextMode()
	{
		int modeNumber = MeltdownLED.IncrementModeNumber();

		MeltdownLogger.Debug(Serial, "Next Mode", modeNumber);
		MeltdownSerial.SendCommand(Serial, Serial1, MeltdownSerial.MODE, modeNumber);
	}

	void executeAutoMode()
	{
		if (!MeltdownLED.GetAutoModeActive()) return;

		if (MeltdownLED.IsAutoPattern())
		{
			EVERY_N_SECONDS(120)
			{
				nextPattern();
			}
		}
		else if (MeltdownLED.IsAutoMode())
		{
			EVERY_N_SECONDS(30)
			{
				nextMode();
			}
		}
		else if (MeltdownLED.IsAutoPatternMode())
		{
			EVERY_N_SECONDS(15)
			{
				Serial.print("Mode Number: ");
				Serial.println(MeltdownLED.GetModeNumber());
				Serial.print("Num Modes: ");
				Serial.println(MeltdownLED.GetNumModes());
				// If we've reached the limit of modes for this pattern, get the next pattern.
				if (MeltdownLED.GetModeNumber() >= MeltdownLED.GetNumModes())
				{
					Serial.println("Auto changing pattern...");
					nextPattern();
				}
				else
				{
					Serial.println("Auto changing mode...");
					nextMode();
				}
			}
		}

		executeAutoPattern();
	}

#pragma endregion PATTERNS

#pragma region BUTTON CODE

	int gAutoPatternModeCode[5] = { CODE_PIN_1, CODE_PIN_1, CODE_PIN_1, CODE_PIN_1, CODE_PIN_1 }; // 9, 9, 9, 9, 9
	int gAutoModeCode[5] = { CODE_PIN_2, CODE_PIN_2, CODE_PIN_2, CODE_PIN_2, CODE_PIN_2 }; // 10, 10, 10, 10, 10
	int gAutoPatternCode[5] = { CODE_PIN_3, CODE_PIN_3, CODE_PIN_3, CODE_PIN_3, CODE_PIN_3 }; // 5, 5, 5, 5, 5
	int gAutoSleepCode[5] = { CODE_PIN_4, CODE_PIN_4, CODE_PIN_4, CODE_PIN_4, CODE_PIN_4 }; // 4, 4, 4, 4, 4

	void disableAutoMode()
	{
		MeltdownLogger.Debug(Serial, "Disabling Auto Mode...");
		MeltdownLED.SetAutoMode(MeltdownLED.None);
	}

	void recordButtonPress(int buttonPin)
	{
		// Insert the button pin to the first position and shift each portion of the code.
		gButtonCode[4] = gButtonCode[3];
		gButtonCode[3] = gButtonCode[2];
		gButtonCode[2] = gButtonCode[1];
		gButtonCode[1] = gButtonCode[0];
		gButtonCode[0] = buttonPin;
	}

	bool buttonCodeMatches(int code[])
	{
		// Check each portion of the code, if it doesn't match the desired code return false.
		for (int i = 0; i < gButtonCodeLength; i++)
		{
			if (code[i] != gButtonCode[i])
			{
				return false;
			}
		}

		return true;
	}

	void resetButtonCode()
	{
		// Replace code with all zeros.
		for (int i = 0; i < gButtonCodeLength; i++)
		{
			gButtonCode[i] = 0;
		}
	}

	void readButtonCode()
	{
		// Auto Pattern
		if (buttonCodeMatches(gAutoPatternCode))
		{
			if (!MeltdownLED.IsAutoPattern())
			{
				MeltdownLogger.Debug(Serial, "Enabling Auto Pattern Mode...");

				MeltdownLED.SetAutoMode(MeltdownLED.Pattern);
				MeltdownSerial.SendCommand(Serial, Serial1, MeltdownSerial.AUTO_PATTERN, 1);
			}
			else
			{
				disableAutoMode();
			}

			resetButtonCode();
		}
		// Auto Mode
		else if (buttonCodeMatches(gAutoModeCode))
		{
			if (!MeltdownLED.IsAutoMode())
			{
				MeltdownLogger.Debug(Serial, "Enabling Auto Mode Mode...");

				MeltdownLED.SetAutoMode(MeltdownLED.Mode);
				MeltdownSerial.SendCommand(Serial, Serial1, MeltdownSerial.AUTO_MODE, 1);
			}
			else
			{
				disableAutoMode();
			}

			resetButtonCode();
		}
		// Auto Pattern Mode
		else if (buttonCodeMatches(gAutoPatternModeCode))
		{
			if (!MeltdownLED.IsAutoPatternMode())
			{
				MeltdownLogger.Debug(Serial, "Enabling Auto Pattern Mode Mode...");

				MeltdownLED.SetAutoMode(MeltdownLED.PatternMode);
				MeltdownSerial.SendCommand(Serial, Serial1, MeltdownSerial.AUTO_PATTERN_MODE, 1);
			}
			else
			{
				disableAutoMode();
			}

			resetButtonCode();
		}
		// Auto Sleep
		else if (buttonCodeMatches(gAutoSleepCode))
		{
			if (!MeltdownLED.IsAutoSleep())
			{
				MeltdownLogger.Debug(Serial, "Enabling Auto Sleep Mode...");

				MeltdownLED.SetAutoMode(MeltdownLED.Sleep);
				MeltdownSerial.SendCommand(Serial, Serial1, MeltdownSerial.AUTO_SLEEP, 1);
			}
			else
			{
				disableAutoMode();
			}

			resetButtonCode();
		}
	}

#pragma endregion BUTTON CODE

#pragma region SET MODIFIERS

	void setBrightness()
	{
		int currVal = MeltdownLED.GetBrightness();
		int brightVal = MeltdownLED.SetBrightness(-1);

		if (MeltdownSerial.HasChanged(currVal, brightVal))
		{
			MeltdownLogger.Debug(Serial, "Setting Brightness", brightVal);
			MeltdownSerial.SendCommand(Serial, Serial1, MeltdownSerial.BRIGHTNESS, brightVal);
		}
	}

	void enableFullPurple()
	{
		bool val = MeltdownLED.EnableFullPurple();

		MeltdownLogger.Debug(Serial, "Enabling Full Purple", val);
		MeltdownSerial.SendCommand(Serial, Serial1, MeltdownSerial.ENABLE_FULL_PURPLE, val);
	}

	void disableFullPurple()
	{
		bool val = MeltdownLED.DisableFullPurple();

		MeltdownLogger.Debug(Serial, "Disabling Full Purple", val);
		MeltdownSerial.SendCommand(Serial, Serial1, MeltdownSerial.DISABLE_FULL_PURPLE, val);
	}

	void enableFullYellow()
	{
		bool val = MeltdownLED.EnableFullYellow();

		MeltdownLogger.Debug(Serial, "Enabling Full Yellow", val);
		MeltdownSerial.SendCommand(Serial, Serial1, MeltdownSerial.ENABLE_FULL_YELLOW, val);
	}

	void disableFullYellow()
	{
		bool val = MeltdownLED.DisableFullYellow();

		MeltdownLogger.Debug(Serial, "Disabling Full Yellow", val);
		MeltdownSerial.SendCommand(Serial, Serial1, MeltdownSerial.DISABLE_FULL_YELLOW, val);
	}

	void enableFullRainbow()
	{
		bool val = MeltdownLED.EnableFullRainbow();

		MeltdownLogger.Debug(Serial, "Enabling Full Rainbow", val);
		MeltdownSerial.SendCommand(Serial, Serial1, MeltdownSerial.ENABLE_FULL_RAINBOW, val);
	}

	void disableFullRainbow()
	{
		bool val = MeltdownLED.DisableFullRainbow();

		MeltdownLogger.Debug(Serial, "Disabling Full Rainbow", val);
		MeltdownSerial.SendCommand(Serial, Serial1, MeltdownSerial.DISABLE_FULL_RAINBOW, val);
	}

	void enableFullGreen()
	{
		bool val = MeltdownLED.EnableFullGreen();

		MeltdownLogger.Debug(Serial, "Enabling Full Green", val);
		MeltdownSerial.SendCommand(Serial, Serial1, MeltdownSerial.ENABLE_FULL_GREEN, val);
	}

	void disableFullGreen()
	{
		bool val = MeltdownLED.DisableFullGreen();

		MeltdownLogger.Debug(Serial, "Disabling Full Green", val);
		MeltdownSerial.SendCommand(Serial, Serial1, MeltdownSerial.DISABLE_FULL_GREEN, val);
	}

	void toggleBlack() 
	{
		bool blackVal = MeltdownLED.ToggleBlack();

		MeltdownLogger.Debug(Serial, "Toggling Black", blackVal);
		MeltdownSerial.SendCommand(Serial, Serial1, MeltdownSerial.BLACK, blackVal);
	}

	void setTopPosition()
	{
		bool topVal = MeltdownLED.ToggleTop();

		MeltdownLogger.Debug(Serial, "Setting top position", topVal);
		MeltdownSerial.SendBoolCommand(Serial, Serial1, MeltdownSerial.TOP, topVal);
	}

	void setBottomPosition()
	{
		bool bottomVal = MeltdownLED.ToggleBottom();

		MeltdownLogger.Debug(Serial, "Setting bottom position", bottomVal);
		MeltdownSerial.SendBoolCommand(Serial, Serial1, MeltdownSerial.BOTTOM, bottomVal);
	}

	void setAnalogPattern()
	{
		int currVal = MeltdownLED.GetAnalogPattern();
		int patternVal = MeltdownLED.SetAnalogPattern(ANALOG_PATTERN_PIN);

		if (MeltdownSerial.HasChanged(currVal, patternVal))
		{
			MeltdownLogger.Debug(Serial, "Setting Analog Pattern", patternVal);
			MeltdownSerial.SendCommand(Serial, Serial1, MeltdownSerial.ANALOG_PATTERN, patternVal);
		}
	}

	void setAnalogEffect()
	{
		int currVal = MeltdownLED.GetAnalogEffect();
		int modeVal = MeltdownLED.SetAnalogEffect(ANALOG_EFFECT_PIN);

		if (MeltdownSerial.HasChanged(currVal, modeVal))
		{
			MeltdownLogger.Debug(Serial, "Setting Analog Effect", modeVal);
			MeltdownSerial.SendCommand(Serial, Serial1, MeltdownSerial.ANALOG_EFFECT, modeVal);
		}
	}

	void togglePause()
	{
		bool pauseVal = MeltdownLED.TogglePause();

		MeltdownLogger.Debug(Serial, "Setting Pause", pauseVal);
		MeltdownSerial.SendCommand(Serial, Serial1, MeltdownSerial.PAUSE, pauseVal);
	}

	void toggleMirror()
	{
		bool mirrorVal = MeltdownLED.ToggleMirror();

		MeltdownLogger.Debug(Serial, "Toggling Mirror", mirrorVal);
		MeltdownSerial.SendCommand(Serial, Serial1, MeltdownSerial.MIRROR, mirrorVal);
	}

	void tryAutoMode()
	{
		if (!MeltdownLED.GetAutoModeEnabled()) return;

		// Check if sleep mode has been enabled. If so, send the serial command.
		if (MeltdownLED.CheckAutoTimer())
		{
			MeltdownLogger.Debug(Serial, "Activating Auto Mode...");
			MeltdownSerial.SendBoolCommand(Serial, Serial1, MeltdownSerial.ACTIVATE_AUTO, true);
		}
	}

	void deactivateAutoMode()
	{
		MeltdownLogger.Debug(Serial, "Deactivating Auto Mode...");
		MeltdownLED.SetAutoModeActive(false);
	}

	void checkModifiers()
	{
		EVERY_N_MILLISECONDS(20)
		{
			setAnalogPattern();
			setAnalogEffect();
		}
	}

#pragma endregion SET MODIFIERS

#pragma region INPUTS

	bool isCodeButtonPressed()
	{
		return digitalRead(CODE_INIT_PIN) == LOW;
	}

	void checkButtonState(Button *button)
	{
		// Read the state of the button pin.
		button->state = digitalRead(button->pin);

		// Check if the button is pressed. If it is, the buttonState is LOW.
		if (button->state == LOW && button->previousState == HIGH)
		{
			// Are we holding down the code button?
			if (isCodeButtonPressed())
			{
				// If we're pressing another button other than the code button, record the button press and read the code.
				if (button->pin != CODE_INIT_PIN)
				{
					Serial.println(button->pin);

					recordButtonPress(button->pin);

					readButtonCode();
				}
				else
				{
					// This is awkward, but for now if this is the first that the code button has been pressed, we must activate it here.
					if (!MeltdownLED.GetAutoModeActive())
					{
						button->pressedCallback();
					}
					else
					{
						deactivateAutoMode();
					}
				}
			}
			else
			{
				// If auto mode is not active, call the button callback. Otherwise, deactivate auto mode.
				if (!MeltdownLED.GetAutoModeActive())
				{
					button->pressedCallback();
				}
				else
				{
					deactivateAutoMode();
				}
			}

			button->previousState = LOW;
		}
		// Check if the button is depressed. If it is, the buttonState is HIGH.
		else if (button->state == HIGH && button->previousState == LOW)
		{
			if (!isCodeButtonPressed())
			{
				// Are we depressing the code button?
				if (button->pin == CODE_INIT_PIN)
				{
					resetButtonCode();

					button->pressedCallback();
				}
				else
				{
					// If auto mode is not active, call the button callback. Otherwise, deactivate auto mode.
					if (!MeltdownLED.GetAutoModeActive())
					{
						if (!button->isToggle)
						{
							if (button->depressedCallback != NULL)
							{
								button->depressedCallback();
							}
							else
							{
								button->pressedCallback();
							}
						}
					}
					else
					{
						deactivateAutoMode();
					}
				}
			}

			button->previousState = HIGH;
		}
	}

	void checkButtonStates()
	{
		EVERY_N_MILLISECONDS(50)
		{
			checkButtonState(&patternButton);
			checkButtonState(&effectButton);
			checkButtonState(&modeButton);
			checkButtonState(&pauseButton);
			checkButtonState(&topButton);
			checkButtonState(&bottomButton);
			checkButtonState(&purpleButton);
			checkButtonState(&yellowButton);
			checkButtonState(&blackButton);
			checkButtonState(&rainbowButton);
			checkButtonState(&mirrorButton);
		}
	}

#pragma endregion INPUTS

	void setupButtons()
	{
		patternButton.pressedCallback = nextPattern;
		patternButton.isToggle = true;
		effectButton.pressedCallback = nextEffect;
		effectButton.isToggle = true;
		modeButton.pressedCallback = nextMode;
		modeButton.isToggle = true;

		topButton.pressedCallback = setTopPosition;
		bottomButton.pressedCallback = setBottomPosition;
		bottomButton.pressedCallback = setBottomPosition;
		mirrorButton.pressedCallback = toggleMirror;
		purpleButton.pressedCallback = enableFullPurple;
		purpleButton.depressedCallback = disableFullPurple;
		yellowButton.pressedCallback = enableFullYellow;
		yellowButton.depressedCallback = disableFullYellow;
		rainbowButton.pressedCallback = enableFullRainbow;
		rainbowButton.depressedCallback = disableFullRainbow;
		blackButton.pressedCallback = toggleBlack;
		pauseButton.pressedCallback = togglePause;
	}

	void setupLedIndexes()
	{
		// Now, this function might *seem* useless, and that's because it is. We need to provide an array of indexes to the library that is shared
		// with the primary Boris code. That code actually uses some fairly complex index arrangements, this code doesn't.

		for (int i = 0; i < NUM_LEDS_PER_LARGE_RING; i++)
		{
			largeRingIndexes[i] = i;
		}

		for (int i = 0; i < NUM_LEDS_PER_MED_RING; i++)
		{
			medRingIndexes[i] = i;
		}
	}

	void executeSetup()
	{
		// initialize serial communication at 9600 bits per second:
		Serial.begin(9600);
		Serial1.begin(9600);

		Serial.println("Serial port opened.");

		MeltdownLogger.InitSerial(DEBUG);

		MeltdownLED.InitTimers();

		delay(3000);

		FastLED.addLeds<WS2812, RING_PIN_1, GRB>(patternRingLeds, NUM_LEDS_PER_LARGE_RING);
		FastLED.addLeds<WS2812, RING_PIN_2, GRB>(effectValRingLeds, NUM_LEDS_PER_LARGE_RING);
		FastLED.addLeds<WS2812, RING_PIN_3, GRB>(positionRingLeds, NUM_LEDS_PER_LARGE_RING);
		FastLED.addLeds<WS2812, RING_PIN_4, GRB>(modeRingLeds, NUM_LEDS_PER_MED_RING);
		FastLED.addLeds<WS2812, RING_PIN_5, GRB>(effectRingLeds, NUM_LEDS_PER_MED_RING);

		LEDS.setBrightness(MeltdownLED.GetBrightness());

		setupButtons();

		setupLedIndexes();

		pinMode(BUTTON_PIN_1, INPUT_PULLUP);
		pinMode(BUTTON_PIN_2, INPUT_PULLUP);
		pinMode(BUTTON_PIN_3, INPUT_PULLUP);
		pinMode(BUTTON_PIN_4, INPUT_PULLUP);
		pinMode(BUTTON_PIN_5, INPUT_PULLUP);
		pinMode(BUTTON_PIN_6, INPUT_PULLUP);
		pinMode(BUTTON_PIN_7, INPUT_PULLUP);
		pinMode(BUTTON_PIN_8, INPUT_PULLUP);
		pinMode(BUTTON_PIN_9, INPUT_PULLUP);
		pinMode(BUTTON_PIN_10, INPUT_PULLUP);
		pinMode(BUTTON_PIN_11, INPUT_PULLUP);
		pinMode(ANALOG_PIN_1, INPUT);
		pinMode(ANALOG_PIN_2, INPUT);

		initPattern();
		initMode();
		initEffect();
		initAuto();
	}

	void executeLoop()
	{
		checkButtonStates();

		checkModifiers();

		if (MeltdownLED.GetAutoModeActive())
		{
			executeAutoMode();
		}
		else
		{
			if (!MeltdownLED.GetPause())
			{
				MeltdownLED.ExecutePattern(patternRingLeds, largeRingIndexes, NUM_LEDS_PER_LARGE_RING, 1, -1);
				MeltdownLED.ExecuteEffect(patternRingLeds, largeRingIndexes, NUM_LEDS_PER_LARGE_RING);

				MeltdownLED.ExecutePattern(modeRingLeds, medRingIndexes, NUM_LEDS_PER_MED_RING, 0, 1);
				MeltdownLED.ExecuteEffect(modeRingLeds, medRingIndexes, NUM_LEDS_PER_MED_RING);

				MeltdownLED.IncrementFrame();
			}

			if (MeltdownLED.GetTop())
			{
				setRingColor(positionRingLeds, NUM_LEDS_PER_LARGE_RING, Top, 64);
			}
			else if (MeltdownLED.GetBottom())
			{
				setRingColor(positionRingLeds, NUM_LEDS_PER_LARGE_RING, Bottom, 128);
			}
			else
			{
				setRingColor(positionRingLeds, NUM_LEDS_PER_LARGE_RING, Full, 192);
			}

			setRingColor(effectValRingLeds, NUM_LEDS_PER_LARGE_RING, Partial, 0);
			MeltdownLED.ExecuteEffect(effectValRingLeds, largeRingIndexes, NUM_LEDS_PER_LARGE_RING);

			setColor(effectRingLeds, NUM_LEDS_PER_MED_RING, CRGB::Black);
			MeltdownLED.ExecuteEffect(effectRingLeds, medRingIndexes, NUM_LEDS_PER_MED_RING, 1);

			tryAutoMode();
		}

		if (MeltdownLED.GetDelay() > 0)
		{
			delay(MeltdownLED.GetDelay());
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