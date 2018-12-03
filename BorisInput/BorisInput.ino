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
#define SPOKES_ONLY_PIN BUTTON_PIN_7
#define WHEELS_ONLY_PIN BUTTON_PIN_8
#define ANALOG_PATTERN_PIN ANALOG_PIN_1
#define ANALOG_EFFECT_PIN ANALOG_PIN_2
#define PARTIAL_RING_PIN ANALOG_PIN_2

#define NUM_MED_RINGS 0
#define NUM_LARGE_RINGS 2

#define NUM_LEDS_PER_MED_RING 16
#define NUM_LEDS_PER_LARGE_RING 24
#define NUM_MED_RING_LEDS (NUM_LEDS_PER_MED_RING + NUM_MED_RINGS)
#define NUM_LARGE_RING_LEDS (NUM_LEDS_PER_LARGE_RING + NUM_LARGE_RINGS)
#define NUM_RING_LEDS (NUM_LARGE_RING_LEDS + NUM_MED_RING_LEDS)
#define NUM_LEDS (NUM_RING_LEDS)

CRGB patternRingLeds[NUM_LEDS_PER_LARGE_RING];
CRGB analogRingLeds[NUM_LEDS_PER_LARGE_RING];
// CRGB analogRingLeds[NUM_LEDS_PER_MED_RING];

int gRingLedCount = 0; // determines the number of lit LEDs for the parially lit ring

// Buttons.
struct Button
{
    int pin;
    int state;
    bool canChangeState;
    bool isToggle;
    void (*callback)();
    
    Button(int pin) : pin (pin), state {0}, canChangeState {false}, isToggle {true}
    {}
};

Button patternButton = { PATTERN_PIN };
Button effectButton = { EFFECT_PIN };
Button modeButton = { MODE_PIN };
Button spokesOnlyButton = { SPOKES_ONLY_PIN };
Button wheelsOnlyButton = { WHEELS_ONLY_PIN };
Button hue1Button = { HUE1_PIN };
Button hue2Button = { HUE2_PIN };
Button hue3Button = { HUE3_PIN };
Button hue4Button = { HUE4_PIN };
Button pauseButton = { PAUSE_PIN };

// Ring options.
enum RingPosition { Top, Bottom, Full, Empty, Partial };
enum RingSize { Large, Medium };

void setup()
{
    // initialize serial communication at 9600 bits per second:
    Serial.begin(9600);
    Serial1.begin(9600);

    Serial.println("Serial port opened.");

    MeltdownLogger.InitSerial(DEBUG);
    
    delay(3000); // 3 second delay for recovery
    //FastLED
    FastLED.addLeds<WS2812, RING_PIN_1, GRB>(patternRingLeds, NUM_LEDS_PER_LARGE_RING);  
    // FastLED.addLeds<WS2812, RING_PIN_2, GRB>(analogRingLeds, NUM_LEDS_PER_MED_RING);  
    FastLED.addLeds<WS2812, RING_PIN_2, GRB>(analogRingLeds, NUM_LEDS_PER_LARGE_RING);  

    // set master brightness control
    LEDS.setBrightness(MeltdownLED.GetBrightness());

    // setupLedArrays();

    setupButtons();

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
    pinMode(ANALOG_PIN_1, INPUT);
    pinMode(ANALOG_PIN_2, INPUT);
}

void setupButtons()
{  
    patternButton.callback = nextPattern;
    effectButton.callback = nextEffect;
    modeButton.callback = nextMode;
    spokesOnlyButton.callback = setSpokesOnly;
    wheelsOnlyButton.callback = setWheelsOnly;
    hue1Button.callback = toggleHue1;
    hue1Button.isToggle = false;
    hue2Button.callback = toggleHue2;
    hue2Button.isToggle = false;
    hue3Button.callback = toggleHue3;
    hue3Button.isToggle = false;
    hue4Button.callback = toggleHue4;
    hue4Button.isToggle = false;
    pauseButton.callback = togglePause;
}

void loop()
{
    checkButtonStates();
    checkModifiers();

    if (!MeltdownLED.GetPause())
    {
        MeltdownLED.ExecutePattern(patternRingLeds, NUM_LEDS_PER_LARGE_RING);
        MeltdownLED.ExecuteEffect(patternRingLeds, NUM_LEDS_PER_LARGE_RING);
    }

    MeltdownLED.ExecutePattern(analogRingLeds, NUM_LEDS_PER_LARGE_RING);
    MeltdownLED.ExecuteEffect(analogRingLeds, NUM_LEDS_PER_LARGE_RING);
    // MeltdownLED.ExecutePattern(analogRingLeds, NUM_LEDS_PER_MED_RING);
    // MeltdownLED.ExecuteEffect(analogRingLeds, NUM_LEDS_PER_MED_RING);

    LEDS.delay(1000 / MeltdownLED.GetFps());
    
    LEDS.show();
}

void setupLedArrays()
{   
    // for (int i = 0; i < NUM_LARGE_RINGS; i++)
    // {
    //     for (int j = 0; j < NUM_LEDS_PER_LARGE_RING; j++)
    //     {
    //         ledLargeRingSets[i] = &leds[(i * NUM_LEDS_PER_LARGE_RING) + j];
    //     }
    // }
    // for (int i = 0; i < NUM_MED_RINGS; i++)
    // {
    //     for (int j = 0; j < NUM_LEDS_PER_MED_RING; j++)
    //     {
    //         ledMedRingSets[i] = &leds[(i * NUM_LEDS_PER_MED_RING) + j];
    //     }
    // }
}

void nextPattern()
{
    // Set to black.
    MeltdownLED.SetAllColor(patternRingLeds, NUM_LEDS_PER_LARGE_RING, CRGB::Black);  
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

    MeltdownLogger.Debug(Serial, "Next Mode: ", modeNumber);
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
        MeltdownLogger.Debug(Serial, "Setting Brightness: ", brightVal);   
        MeltdownSerial.SendCommand(Serial, Serial1, MeltdownSerial.BRIGHTNESS, brightVal);
    }
}

void toggleHue1() { toggleHue(1); }
void toggleHue2() { toggleHue(2); }
void toggleHue3() { toggleHue(3); }
void toggleHue4() { toggleHue(4); }

void toggleHue(int index)
{
    bool hueValue = MeltdownLED.ToggleHue(index);
    
    MeltdownLogger.Debug(Serial, "Toggling Hue...", hueValue);   
    MeltdownSerial.SendCommand(Serial, Serial1, "HUE" + index, hueValue);
}

void setBoth()
{
    // gSpokesOnly = false;
    // gWheelsOnly = false;

    // MeltdownLogger.Debug(Serial, "Setting both...");   
    // MeltdownSerial.SendBoolCommand(Serial, Serial1, MeltdownSerial.SPOKE, false);
    // MeltdownSerial.SendBoolCommand(Serial, Serial1, MeltdownSerial.WHEEL, false);
}

void setSpokesOnly()
{
    // gSpokesOnly = !gSpokesOnly;

    // MeltdownLogger.Debug(Serial, "Setting spokes only: ", gSpokesOnly);   
    // MeltdownSerial.SendBoolCommand(Serial, Serial1, MeltdownSerial.SPOKE, gSpokesOnly);
}

void setWheelsOnly()
{
    // gWheelsOnly = !gWheelsOnly;

    // MeltdownLogger.Debug(Serial, "Setting wheels only: ", gWheelsOnly);   
    // MeltdownSerial.SendBoolCommand(Serial, Serial1, MeltdownSerial.WHEEL, gWheelsOnly);
}

void setAnalogPattern()
{
    int currVal = MeltdownLED.GetAnalogPattern();
    int patternVal = MeltdownLED.SetAnalogPattern(ANALOG_PATTERN_PIN);

    if (MeltdownSerial.HasChanged(currVal, patternVal))
    {
        MeltdownLogger.Debug(Serial, "Setting Analog Pattern: ", patternVal);   
        MeltdownSerial.SendCommand(Serial, Serial1, MeltdownSerial.ANALOG_PATTERN, patternVal);
    }
}

void setAnalogEffect()
{
    int currVal = MeltdownLED.GetAnalogEffect();
    int modeVal = MeltdownLED.SetAnalogEffect(ANALOG_EFFECT_PIN);

    if (MeltdownSerial.HasChanged(currVal, modeVal))
    {
        MeltdownLogger.Debug(Serial, "Setting Analog Effect: ", modeVal);   
        MeltdownSerial.SendCommand(Serial, Serial1, MeltdownSerial.ANALOG_EFFECT, modeVal);
    }
}

void togglePause()
{
    bool pauseVal = MeltdownLED.TogglePause();

    MeltdownLogger.Debug(Serial, "Setting Pause: ", pauseVal);   
    MeltdownSerial.SendCommand(Serial, Serial1, MeltdownSerial.PAUSE, pauseVal);
}

#pragma endregion SET MODIFIERS

#pragma region PATTERNS

void setColor(CRGB ledSets[], int numLeds, CRGB::HTMLColorCode color)
{
    for (int i = 0; i < numLeds; i++)
    {
        ledSets[i] = color;
    }
}

void setRingColor(CRGB *ringLeds[], int numLeds, RingPosition position, CRGB::HTMLColorCode color)
{
    for (int i = 0; i < numLeds; i++)
    {
        if (canColorRingLed(i + 1, numLeds, position))
        {
            *ringLeds[i] = color;
        }
        else
        {
            *ringLeds[i] = CRGB::Black;
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
    int currVal = gRingLedCount;
    int newVal = MeltdownSerial.GetAnalogValue(PARTIAL_RING_PIN, currVal);

    if (MeltdownSerial.HasChanged(currVal, newVal))
    {
        gRingLedCount = 0; // TODO MAP THIS
    }

    return gRingLedCount;
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
        checkButtonState(&spokesOnlyButton);
        checkButtonState(&wheelsOnlyButton);
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
    // Check if the button is pressed. If it is, the buttonState is HIGH.
    if (button->state == LOW && button->canChangeState) 
    {
        button->callback();
        button->canChangeState = false;
    }
    else if (button->state == HIGH && !(button->canChangeState))
    {
        if (!button->isToggle)
        {
            button->callback();
        }
        button->canChangeState = true;
    }
}

#pragma endregion INPUTS
