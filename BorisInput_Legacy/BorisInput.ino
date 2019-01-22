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

#define RING_PIN_1 8
#define RING_PIN_2 9
#define RING_PIN_3 10
#define RING_PIN_4 11
#define RING_PIN_5 12

#define BUTTON_PIN_1 27 // 2
#define BUTTON_PIN_2 28 // 3
#define BUTTON_PIN_3 29 // 4
#define BUTTON_PIN_4 30 // 5
#define BUTTON_PIN_5 31 // 6
#define BUTTON_PIN_6 32 // 7
#define BUTTON_PIN_7 33 // 10
#define BUTTON_PIN_8 34 // 11
#define BUTTON_PIN_9 35 // 12
#define BUTTON_PIN_10 36 // 13
#define BUTTON_PIN_11 37
#define ANALOG_PIN_1 A0
#define ANALOG_PIN_2 A1

#define PATTERN_PIN BUTTON_PIN_1
#define PAUSE_PIN BUTTON_PIN_2
#define MODE_PIN BUTTON_PIN_3
#define HUE1_PIN BUTTON_PIN_4
#define HUE2_PIN BUTTON_PIN_5
#define HUE3_PIN BUTTON_PIN_9
#define HUE4_PIN BUTTON_PIN_10
#define EFFECT_PIN BUTTON_PIN_6
#define BOTTOM_PIN BUTTON_PIN_7
#define TOP_PIN BUTTON_PIN_8
#define ANALOG_PATTERN_PIN ANALOG_PIN_1
#define ANALOG_EFFECT_PIN ANALOG_PIN_2
#define PARTIAL_RING_PIN ANALOG_PIN_2

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

struct Button
{
    int pin;
    int state;
    int previousState;
    bool isToggle;
    void (*callback)();
    
    Button(int pin) : pin (pin), state {HIGH}, previousState {HIGH}, isToggle {false}
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
Button pauseButton = { PAUSE_PIN };

// Ring options.
enum RingPosition { Top, Bottom, Full, Empty, Partial };

void setup()
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
}

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

void loop()
{
    checkButtonStates();

    checkModifiers();

    if (!MeltdownLED.GetSleeping())
    {
        if (!MeltdownLED.GetPause())
        {
            MeltdownLED.ExecutePattern(patternRing, NUM_LEDS_PER_LARGE_RING, 1);
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

        trySleep();
    }
    else
    {
        executeSleepPattern();
    }

    LEDS.delay(1000 / MeltdownLED.GetFps());
    
    LEDS.show();
}

void executeSleepPattern()
{
    static int hue = 0;

    setAllColor(CRGB::Black);
    setRingColor(patternRing, NUM_LEDS_PER_LARGE_RING, Full, hue);

    EVERY_N_MILLIS(50)
    {
        hue++;
    }
}

void nextPattern()
{
    // Set to black.
    MeltdownLED.SetAllColor(patternRing, NUM_LEDS_PER_LARGE_RING, CRGB::Black);  
    MeltdownLED.NextPattern();

    MeltdownLogger.Debug(Serial, "Next Pattern...");  
    MeltdownSerial.SendBoolCommand(Serial, Serial1, MeltdownSerial.PATTERN, true);
}

void nextEffect()
{
    MeltdownLED.NextEffect();

    MeltdownLogger.Debug(Serial, "Next Effect...");  
    MeltdownSerial.SendBoolCommand(Serial, Serial1, MeltdownSerial.EFFECT, true);
}

void nextMode()
{
    int modeNumber = MeltdownLED.NextMode();

    MeltdownLogger.Debug(Serial, "Next Mode", modeNumber);
    MeltdownSerial.SendCommand(Serial, Serial1, MeltdownSerial.MODE, true);
}

#pragma region SET MODIFIERS

void checkModifiers()
{
    EVERY_N_MILLISECONDS(20) 
    { 
        setAnalogPattern();
        setAnalogEffect();
    }
}

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

void toggleHue1() { toggleHue(1, MeltdownSerial.HUE1); }
void toggleHue2() { toggleHue(2, MeltdownSerial.HUE2); }
void toggleHue3() { toggleHue(3, MeltdownSerial.HUE3); }
void toggleHue4() { toggleHue(4, MeltdownSerial.HUE4); }

void toggleHue(int index, String command)
{
    bool hueValue = MeltdownLED.ToggleHue(index);
    
    MeltdownLogger.Debug(Serial, "Toggling Hue", command);   
    MeltdownSerial.SendCommand(Serial, Serial1, command, hueValue);
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

void trySleep()
{
    // Check if sleep mode has been enabled. If so, send the serial command.
    if (MeltdownLED.CheckSleepTimer())
    {
        MeltdownLogger.Debug(Serial, "Entering Sleep Mode...");  
        MeltdownSerial.SendBoolCommand(Serial, Serial1, MeltdownSerial.SLEEP, true);
    }
}

void tryWakeUp()
{
    if (MeltdownLED.GetSleeping())
    {
        MeltdownLogger.Debug(Serial, "Waking Up...");   
        MeltdownSerial.SendCommand(Serial, Serial1, MeltdownSerial.SLEEP, false);
    }
    MeltdownLED.SetSleeping(false);
}

#pragma endregion SET MODIFIERS

#pragma region PATTERNS

void setAllColor(CRGB::HTMLColorCode color)
{
    setColor(patternRing, NUM_LEDS_PER_LARGE_RING, color);
    setColor(effectValRing, NUM_LEDS_PER_LARGE_RING, color);
    setColor(positionRing, NUM_LEDS_PER_LARGE_RING, color);
    setColor(modeRing, NUM_LEDS_PER_MED_RING, color);
    setColor(effectRing, NUM_LEDS_PER_MED_RING, color);
}

void setColor(CRGB *ledSets[], int numLeds, CRGB::HTMLColorCode color)
{
    for (int i = 0; i < numLeds; i++)
    {
        *ledSets[i] = color;
    }
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

#pragma endregion PATTERNS

#pragma region INPUTS

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
    }
}

void checkButtonState(Button *button)
{
    // Read the state of the button pin.
    button->state = digitalRead(button->pin);

    // Check if the button is depressed. If it is, the buttonState is LOW.
    if (button->state == LOW && button->previousState == HIGH) 
    {
        if (!MeltdownLED.GetSleeping())
        {
            button->callback();
        }
        tryWakeUp();

        button->previousState = LOW;
    }
    // Check if the button is pressed. If it is, the buttonState is HIGH.
    else if (button->state == HIGH && button->previousState == LOW)
    {
        if (!MeltdownLED.GetSleeping())
        {
            if (!button->isToggle)
            {
                button->callback();
            }
        }
        tryWakeUp();

        button->previousState = HIGH;
    }
}

#pragma endregion INPUTS