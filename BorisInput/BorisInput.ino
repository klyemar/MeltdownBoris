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
#define HUE1_PIN BUTTON_PIN_4
#define HUE2_PIN BUTTON_PIN_5
#define HUE3_PIN BUTTON_PIN_9
#define HUE4_PIN BUTTON_PIN_10
#define FULL_BRIGHT_PIN BUTTON_PIN_11
#define EFFECT_PIN BUTTON_PIN_6
#define BOTTOM_PIN BUTTON_PIN_7
#define TOP_PIN BUTTON_PIN_8
#define ANALOG_PATTERN_PIN ANALOG_PIN_1
#define ANALOG_EFFECT_PIN ANALOG_PIN_2
#define PARTIAL_RING_PIN ANALOG_PIN_2

#define CODE_INIT_PIN PAUSE_PIN
#define CODE_PIN_1 HUE3_PIN
#define CODE_PIN_2 HUE4_PIN
#define CODE_PIN_3 HUE2_PIN
#define CODE_PIN_4 HUE1_PIN

#define NUM_LEDS_PER_MED_RING 16
#define NUM_LEDS_PER_LARGE_RING 24

	CRGB ringLeds1[NUM_LEDS_PER_LARGE_RING];
	CRGB ringLeds2[NUM_LEDS_PER_LARGE_RING];
	CRGB ringLeds3[NUM_LEDS_PER_LARGE_RING];
	CRGB ringLeds4[NUM_LEDS_PER_MED_RING];
	CRGB ringLeds5[NUM_LEDS_PER_MED_RING];

	CRGB *patternRing[NUM_LEDS_PER_LARGE_RING];
	CRGB *effectValRing[NUM_LEDS_PER_LARGE_RING];
	CRGB *positionRing[NUM_LEDS_PER_LARGE_RING];
	CRGB *modeRing[NUM_LEDS_PER_MED_RING];
	CRGB *effectRing[NUM_LEDS_PER_MED_RING];

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
		void(*callback)();

		Button(int pin) : pin(pin), state{ HIGH }, previousState{ HIGH }, isToggle{ false }
		{}
	};

	Button patternButton = { PATTERN_PIN };
	Button effectButton = { EFFECT_PIN };
	Button modeButton = { MODE_PIN };
	Button topButton = { TOP_PIN };
	Button bottomButton = { BOTTOM_PIN };
	Button hue1Button = { HUE1_PIN };
	Button hue2Button = { HUE2_PIN };
	Button hue3Button = { HUE3_PIN };
	Button hue4Button = { HUE4_PIN };
	Button fullBrightButton = { FULL_BRIGHT_PIN };
	Button pauseButton = { PAUSE_PIN };

#pragma region PATTERNS

	void setColor(CRGB *ledSets[], int numLeds, CRGB::HTMLColorCode color)
	{
		for (int i = 0; i < numLeds; i++)
		{
			*ledSets[i] = color;
		}
	}

	void setAllColor(CRGB::HTMLColorCode color)
	{
		setColor(patternRing, NUM_LEDS_PER_LARGE_RING, color);
		setColor(effectValRing, NUM_LEDS_PER_LARGE_RING, color);
		setColor(positionRing, NUM_LEDS_PER_LARGE_RING, color);
		setColor(modeRing, NUM_LEDS_PER_MED_RING, color);
		setColor(effectRing, NUM_LEDS_PER_MED_RING, color);
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

	void setRingColor(CRGB *ringLeds[], int numLeds, RingPosition position, int hueOffset)
	{
		setColor(ringLeds, numLeds, CRGB::Black);

		for (int i = 0; i < numLeds; i++)
		{
			if (canColorRingLed(i + 1, numLeds, position))
			{
				*ringLeds[i] = CHSV(gRingHue + hueOffset, 255, 192);
			}
		}
	}

	void executeAutoPattern()
	{
		static int hue = 0;

		setAllColor(CRGB::Black);
		setRingColor(patternRing, NUM_LEDS_PER_LARGE_RING, Full, hue);

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

	void nextPattern()
	{
		// Reset the mode.
		initMode();

		// Set to black.
		MeltdownLED.SetAllColor(patternRing, NUM_LEDS_PER_LARGE_RING, CRGB::Black);
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
				// If we've reached the limit of modes for this pattern, get the next pattern.
				if (MeltdownLED.GetModeNumber() >= MeltdownLED.GetNumModes())
				{
					nextPattern();
					MeltdownLED.ExecutePattern(patternRing, NUM_LEDS_PER_LARGE_RING, 1, -1);
				}
				else
				{
					nextMode();
				}
			}
		}

		executeAutoPattern();
	}

#pragma endregion PATTERNS

#pragma region BUTTON CODE

	int gAutoPatternCode[5] = { CODE_PIN_1, CODE_PIN_1, CODE_PIN_1, CODE_PIN_1, CODE_PIN_1 }; // 9, 9, 9, 9, 9
	int gAutoModeCode[5] = { CODE_PIN_2, CODE_PIN_2, CODE_PIN_2, CODE_PIN_2, CODE_PIN_2 }; // 10, 10, 10, 10, 10
	int gAutoPatternModeCode[5] = { CODE_PIN_3, CODE_PIN_3, CODE_PIN_3, CODE_PIN_3, CODE_PIN_3 }; // 5, 5, 5, 5, 5
	int gAutoSleepCode[5] = { CODE_PIN_4, CODE_PIN_4, CODE_PIN_4, CODE_PIN_4, CODE_PIN_4 }; // 4, 4, 4, 4, 4

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
		if (buttonCodeMatches(gAutoPatternCode))
		{
			MeltdownLogger.Debug(Serial, "Enabling Auto Pattern Mode...");

			MeltdownLED.SetAutoMode(MeltdownLED.Pattern);
			MeltdownSerial.SendCommand(Serial, Serial1, MeltdownSerial.AUTO_PATTERN, 1);

			resetButtonCode();
		}
		else if (buttonCodeMatches(gAutoModeCode))
		{
			MeltdownLogger.Debug(Serial, "Enabling Auto Mode Mode...");

			MeltdownLED.SetAutoMode(MeltdownLED.Mode);
			MeltdownSerial.SendCommand(Serial, Serial1, MeltdownSerial.AUTO_MODE, 1);

			resetButtonCode();
		}
		else if (buttonCodeMatches(gAutoPatternModeCode))
		{
			MeltdownLogger.Debug(Serial, "Enabling Auto Pattern Mode Mode...");

			MeltdownLED.SetAutoMode(MeltdownLED.PatternMode);
			MeltdownSerial.SendCommand(Serial, Serial1, MeltdownSerial.AUTO_PATTERN_MODE, 1);

			resetButtonCode();
		}
		else if (buttonCodeMatches(gAutoSleepCode))
		{
			MeltdownLogger.Debug(Serial, "Enabling Auto Sleep Mode...");

			MeltdownLED.SetAutoMode(MeltdownLED.Sleep);
			MeltdownSerial.SendCommand(Serial, Serial1, MeltdownSerial.AUTO_SLEEP, 1);

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

	void toggleHue(int index, String command)
	{
		bool hueValue = MeltdownLED.ToggleHue(index);

		MeltdownLogger.Debug(Serial, "Toggling Hue", command);
		MeltdownSerial.SendCommand(Serial, Serial1, command, hueValue);
	}

	void toggleHue1() { toggleHue(1, MeltdownSerial.HUE1); }
	void toggleHue2() { toggleHue(2, MeltdownSerial.HUE2); }
	void toggleHue3() { toggleHue(3, MeltdownSerial.HUE3); }
	void toggleHue4() { toggleHue(4, MeltdownSerial.HUE4); }

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

	void toggleFullBright()
	{
		bool fullBrightVal = MeltdownLED.ToggleFullBright();

		MeltdownLogger.Debug(Serial, "Setting Full Bright", fullBrightVal);
		MeltdownSerial.SendCommand(Serial, Serial1, MeltdownSerial.FULL_BRIGHT, fullBrightVal);
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

	void disableAutoMode()
	{
		MeltdownLogger.Debug(Serial, "Deactivating Auto Mode...");
		MeltdownLED.SetAutoMode(MeltdownLED.None);
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
						button->callback();
					}
					else
					{
						disableAutoMode();
					}
				}
			}
			else
			{
				// If auto mode is not active, call the button callback. Otherwise, deactivate auto mode.
				if (!MeltdownLED.GetAutoModeActive())
				{
					button->callback();
				}
				else
				{
					disableAutoMode();
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

					button->callback();
				}
				else
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
						disableAutoMode();
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
			checkButtonState(&hue1Button);
			checkButtonState(&hue2Button);
			checkButtonState(&hue3Button);
			checkButtonState(&hue4Button);
			checkButtonState(&fullBrightButton);
		}
	}

#pragma endregion INPUTS

	void setupButtons()
	{
		patternButton.callback = nextPattern;
		patternButton.isToggle = true;
		effectButton.callback = nextEffect;
		effectButton.isToggle = true;
		modeButton.callback = nextMode;
		modeButton.isToggle = true;
		hue1Button.callback = toggleHue1;
		hue1Button.isToggle = true;
		hue2Button.callback = toggleHue2;
		hue2Button.isToggle = true;

		topButton.callback = setTopPosition;
		bottomButton.callback = setBottomPosition;
		fullBrightButton.callback = toggleFullBright;
		hue3Button.callback = toggleHue3;
		hue4Button.callback = toggleHue4;
		pauseButton.callback = togglePause;
	}

	void setupLedArrays()
	{
		for (int i = 0; i < NUM_LEDS_PER_LARGE_RING; i++)
		{
			patternRing[i] = &ringLeds1[i];
			effectValRing[i] = &ringLeds2[i];
			positionRing[i] = &ringLeds3[i];
		}

		for (int i = 0; i < NUM_LEDS_PER_MED_RING; i++)
		{
			modeRing[i] = &ringLeds4[i];
			effectRing[i] = &ringLeds5[i];
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

		FastLED.addLeds<WS2812, RING_PIN_1, GRB>(ringLeds1, NUM_LEDS_PER_LARGE_RING);
		FastLED.addLeds<WS2812, RING_PIN_2, GRB>(ringLeds2, NUM_LEDS_PER_LARGE_RING);
		FastLED.addLeds<WS2812, RING_PIN_3, GRB>(ringLeds3, NUM_LEDS_PER_LARGE_RING);
		FastLED.addLeds<WS2812, RING_PIN_4, GRB>(ringLeds4, NUM_LEDS_PER_MED_RING);
		FastLED.addLeds<WS2812, RING_PIN_5, GRB>(ringLeds5, NUM_LEDS_PER_MED_RING);

		LEDS.setBrightness(MeltdownLED.GetBrightness());

		setupButtons();

		setupLedArrays();

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
				MeltdownLED.ExecutePattern(patternRing, NUM_LEDS_PER_LARGE_RING, 1, -1);
				MeltdownLED.ExecuteEffect(patternRing, NUM_LEDS_PER_LARGE_RING);

				MeltdownLED.ExecutePattern(modeRing, NUM_LEDS_PER_MED_RING, 0, 1);
				MeltdownLED.ExecuteEffect(modeRing, NUM_LEDS_PER_MED_RING);
			}

			if (MeltdownLED.GetTop())
			{
				setRingColor(positionRing, NUM_LEDS_PER_LARGE_RING, Top, 64);
			}
			else if (MeltdownLED.GetBottom())
			{
				setRingColor(positionRing, NUM_LEDS_PER_LARGE_RING, Bottom, 128);
			}
			else
			{
				setRingColor(positionRing, NUM_LEDS_PER_LARGE_RING, Full, 192);
			}

			setRingColor(effectValRing, NUM_LEDS_PER_LARGE_RING, Partial, 0);
			MeltdownLED.ExecuteEffect(effectValRing, NUM_LEDS_PER_LARGE_RING);

			setColor(effectRing, NUM_LEDS_PER_MED_RING, CRGB::Black);
			MeltdownLED.ExecuteEffect(effectRing, NUM_LEDS_PER_MED_RING, 1);

			if (MeltdownLED.GetFullBright())
			{
				MeltdownLED.MaximizeBrightness(patternRing, NUM_LEDS_PER_LARGE_RING);
				MeltdownLED.MaximizeBrightness(effectValRing, NUM_LEDS_PER_LARGE_RING);
				MeltdownLED.MaximizeBrightness(modeRing, NUM_LEDS_PER_MED_RING);
			}

			tryAutoMode();
		}

		LEDS.delay(1000 / MeltdownLED.GetFps());

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