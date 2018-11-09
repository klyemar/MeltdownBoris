#include <FastLED.h>
#include <MeltdownLED.h>

// Boris, short for Explora Borealis, is an interactive light display
// controlled by several individual user inputs.
//
// This project builds off of the FastLED library and some of its existing patterns
// and examples.

#define DEBUG true

#define RING_PIN_1 8
#define RING_PIN_2 9

#define BUTTON_PIN_1 2
#define BUTTON_PIN_2 3
#define BUTTON_PIN_3 4
#define BUTTON_PIN_4 5
#define BUTTON_PIN_5 6
#define BUTTON_PIN_6 7
#define ANALOG_PIN_1 A0
#define ANALOG_PIN_2 A1

#define PATTERN_PIN BUTTON_PIN_1
#define HUE1_PIN BUTTON_PIN_2
#define HUE2_PIN BUTTON_PIN_3
#define HUE3_PIN BUTTON_PIN_4
//#define HUE4_PIN BUTTON_PIN_8
//#define HUE5_PIN BUTTON_PIN_9
#define PAUSE_PIN BUTTON_PIN_5
#define MODE_PIN BUTTON_PIN_6
// #define BOTH_PIN BUTTON_PIN_2
// #define SPOKES_ONLY_PIN BUTTON_PIN_3
// #define WHEELS_ONLY_PIN BUTTON_PIN_4
#define ANALOG_PATTERN_PIN ANALOG_PIN_1
#define ANALOG_MODE_PIN ANALOG_PIN_2

#define NUM_MED_RINGS 1
#define NUM_LARGE_RINGS 1

#define NUM_LEDS_PER_MED_RING 16
#define NUM_LEDS_PER_LARGE_RING 24
#define NUM_MED_RING_LEDS (NUM_LEDS_PER_MED_RING + NUM_MED_RINGS)
#define NUM_LARGE_RING_LEDS (NUM_LEDS_PER_LARGE_RING + NUM_LARGE_RINGS)
#define NUM_RING_LEDS (NUM_LARGE_RING_LEDS + NUM_MED_RING_LEDS)
#define NUM_LEDS (NUM_RING_LEDS)

CRGB ringLeds1[NUM_LEDS_PER_LARGE_RING];
CRGB ringLeds2[NUM_LEDS_PER_MED_RING];

CRGB *ring1[NUM_LEDS_PER_LARGE_RING];
CRGB *ring2[NUM_LEDS_PER_MED_RING];

// Global LED values.
bool gActiveSpokes = false;
bool gSpokesOnly = false;
bool gWheelsOnly = false;

// Buttons.
struct Button
{
    int pin;
    int state;
    bool canChangeState;
    void (*callback)();
    
    Button(int pin) : pin (pin), state {0}, canChangeState {false}
    {}
};

Button patternButton = { PATTERN_PIN };
Button modeButton = { MODE_PIN };
// Button bothButton = { BOTH_PIN };
// Button spokesOnlyButton = { SPOKES_ONLY_PIN };
// Button wheelsOnlyButton = { WHEELS_ONLY_PIN };
Button hue1Button = { HUE1_PIN };
Button hue2Button = { HUE2_PIN };
Button hue3Button = { HUE3_PIN };
Button pauseButton = { PAUSE_PIN };

// Ring options.
enum RingPosition { Top, Bottom, Full, Empty };
enum RingSize { Large, Medium };

void setup()
{
    // initialize serial communication at 9600 bits per second:
    Serial.begin(9600);
    // Serial2.begin(9600);

    Serial.println("Serial port opened.");
    
    delay(3000); // 3 second delay for recovery
    //FastLED
    FastLED.addLeds<WS2812, RING_PIN_1, GRB>(ringLeds1, NUM_LEDS_PER_LARGE_RING);  
    FastLED.addLeds<WS2812, RING_PIN_2, GRB>(ringLeds2, NUM_LEDS_PER_MED_RING);  

    // set master brightness control
    LEDS.setBrightness(MeltdownLED.GetBrightness());

    setupLedArrays();

    setupButtons();

    pinMode(BUTTON_PIN_1, INPUT);
    pinMode(BUTTON_PIN_2, INPUT);
    pinMode(BUTTON_PIN_3, INPUT);
    pinMode(BUTTON_PIN_4, INPUT);
    pinMode(BUTTON_PIN_5, INPUT);
    pinMode(BUTTON_PIN_6, INPUT);
    pinMode(ANALOG_PIN_1, INPUT);
    pinMode(ANALOG_PIN_2, INPUT);
}

void setupButtons()
{  
    patternButton.callback = nextPattern;
    modeButton.callback = nextMode;
    // bothButton.callback = setBoth;
    // spokesOnlyButton.callback = setSpokesOnly;
    // wheelsOnlyButton.callback = setWheelsOnly;
    hue1Button.callback = toggleHue1;
    hue2Button.callback = toggleHue2;
    hue3Button.callback = toggleHue3;
    // //hue4Button.callback = toggleHue4;
    // // hue5Button.callback = toggleHue5;
    // inverseButton.callback = toggleInverse;
    pauseButton.callback = togglePause;
}

void loop()
{
    checkButtonStates();

    checkModifiers();

    if (!MeltdownLED.GetPause())
    {
        // Wheel pattern.
        // Call the current pattern function once, updating the 'leds' array
        MeltdownLED.ExecutePattern(ring1, NUM_LEDS_PER_LARGE_RING);
        MeltdownLED.ExecutePattern(ring2, NUM_LEDS_PER_MED_RING);
        
        MeltdownLED.ExecuteMode(ring1, NUM_LEDS_PER_LARGE_RING);
        MeltdownLED.ExecuteMode(ring2, NUM_LEDS_PER_MED_RING);

        // send the 'leds' array out to the actual LED strip
        LEDS.show();
    }

    // insert a delay to keep the framerate modest
    LEDS.delay(1000 / MeltdownLED.GetFps());
    
    // EVERY_N_SECONDS( 20 ) { nextPattern(); } // change patterns periodically

    //gutCheck();
}

void gutCheck()
{   
    for (int i = 0; i < NUM_LEDS_PER_LARGE_RING; i++)
    {
        *ring1[i] = CRGB::Blue;
    }
    // for (int i = 0; i < NUM_LEDS_PER_LARGE_RING; i++)
    // {
    //     *ring2[i] = CRGB::Blue;
    // }
    LEDS.show();
}

void setupLedArrays()
{   
    for (int i = 0; i < NUM_LEDS_PER_LARGE_RING; i++)
    {
        ring1[i] = &ringLeds1[i];
    }
    for (int i = 0; i < NUM_LEDS_PER_MED_RING; i++)
    {
        ring2[i] = &ringLeds2[i];
    }
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
    setColor(ringLeds1, NUM_LEDS_PER_LARGE_RING, CRGB::Black);
    
    MeltdownLED.NextPattern();

    #if DEBUG
        Serial.println("Next Pattern...");
    #endif
    sendBoolCommand("PTN1", true);
}

void nextMode()
{
    MeltdownLED.NextMode();

    #if DEBUG
        Serial.println("Next Mode...");
    #endif
    sendBoolCommand("MODE", true);
}

#pragma region SET MODIFIERS

void checkModifiers()
{
    EVERY_N_MILLISECONDS(20) 
    { 
        setAnalogPattern();
        setAnalogMode();
    }
}

void setBrightness()
{
    int currVal = MeltdownLED.GetBrightness();
    int brightVal = MeltdownLED.SetBrightness(-1);

    if (MeltdownLED.HasChanged(currVal, brightVal))
    {
    #if DEBUG
        Serial.print("Setting Brightness: ");
        Serial.println(brightVal);
    #endif
        sendCommand("BRIT", brightVal);
    }
}

void toggleHue1() { toggleHue(1); }
void toggleHue2() { toggleHue(2); }
void toggleHue3() { toggleHue(3); }
void toggleHue4() { toggleHue(4); }
void toggleHue5() { toggleHue(5); }

void toggleHue(int index)
{
    bool hueValue = MeltdownLED.ToggleHue(index);
    #if DEBUG
        Serial.println("Toggling Hue...");
    #endif
    String command = "HUE";
    sendCommand(command + index, hueValue);
}

void setBoth()
{
    gSpokesOnly = false;
    gWheelsOnly = false;

    #if DEBUG
        Serial.print("Setting both...");
    #endif
    sendBoolCommand("SPON", false);
    sendBoolCommand("WHON", false);
}

void setSpokesOnly()
{
    gSpokesOnly = !gSpokesOnly;

    #if DEBUG
        Serial.print("Setting spokes only: ");
        Serial.println(gSpokesOnly);
    #endif
    sendBoolCommand("SPON", gSpokesOnly);
}

void setWheelsOnly()
{
    gWheelsOnly = !gWheelsOnly;

    #if DEBUG
        Serial.print("Setting wheels only: ");
        Serial.println(gWheelsOnly);
    #endif
    sendBoolCommand("WHON", gWheelsOnly);
}

void setAnalogPattern()
{
    int currVal = MeltdownLED.GetAnalogPattern();
    int patternVal = MeltdownLED.SetAnalogPattern(ANALOG_PATTERN_PIN);

    if (MeltdownLED.HasChanged(currVal, patternVal))
    {
        #if DEBUG
            Serial.print("Setting Analog Pattern: ");
            Serial.println(patternVal);
        #endif
        sendCommand("ANPT", patternVal);
    }
}

void setAnalogMode()
{
    int currVal = MeltdownLED.GetAnalogMode();
    int modeVal = MeltdownLED.SetAnalogMode(ANALOG_MODE_PIN);

    if (MeltdownLED.HasChanged(currVal, modeVal))
    {
        #if DEBUG
            Serial.print("Setting Analog Mode: ");
            Serial.println(modeVal);
        #endif
        sendCommand("ANMD", modeVal);
    }
}

void togglePause()
{
    bool pauseVal = MeltdownLED.TogglePause();

    #if DEBUG
        Serial.print("Setting Pause: ");
        Serial.println(pauseVal);
    #endif
    sendCommand("PAUS", pauseVal);
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

    return false;
}

#pragma endregion PATTERNS

#pragma region INPUTS

void checkButtonStates()
{
    EVERY_N_MILLISECONDS(20) 
    { 
        checkButtonState(&patternButton);
        checkButtonState(&pauseButton);
        // checkButtonState(&bothButton);
        // checkButtonState(&spokesOnlyButton);
        // checkButtonState(&wheelsOnlyButton);
        checkButtonState(&hue1Button);
        checkButtonState(&hue2Button);
        checkButtonState(&hue3Button);
        //checkButtonState(&hue4Button);
        //checkButtonState(&hue5Button);
        // checkButtonState(&inverseButton);
        checkButtonState(&modeButton);
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
        button->canChangeState = true;
    }
}

#pragma endregion INPUTS

#pragma region COMMANDS

void sendBoolCommand(String command, bool value)
{
    sendCommand(command, value ? 1 : 0);
}

void sendCommand(String command, int value)
{
    String serialCommand = MeltdownLED.PrepareCommand(command, value);
    
    if (Serial.available())
    // if (Serial2.available())
    {
        Serial.print("Sending command: " + serialCommand);
        for (int i = 0; i < serialCommand.length(); i++)
        {
            // Serial2.write(serialCommand[i]);   // Push each char 1 by 1 on each loop pass
            Serial.write(serialCommand[i]);   // Push each char 1 by 1 on each loop pass
        }
    }
}

#pragma endregion COMMANDS
