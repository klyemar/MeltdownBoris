#include <FastLED.h>
#include <MeltdownLED.h>

// Boris, short for Explora Borealis, is an interactive light display
// controlled by several individual user inputs.
//
// This project builds off of the FastLED library and some of its existing patterns
// and examples.

#define DEBUG true

#define DATA_PIN 22

#define BUTTON_PIN_1 2
#define BUTTON_PIN_2 3
#define BUTTON_PIN_3 4
#define BUTTON_PIN_4 5
#define BUTTON_PIN_5 6
#define BUTTON_PIN_6 7
#define BUTTON_PIN_7 8
#define BUTTON_PIN_8 9
#define BUTTON_PIN_9 10
#define BUTTON_PIN_10 11
#define BUTTON_PIN_11 12
#define BUTTON_PIN_12 13
#define BUTTON_PIN_13 31
#define ANALOG_PIN_1 A0
#define ANALOG_PIN_2 A1
#define ANALOG_PIN_3 A1

#define PATTERN_PIN 33//BUTTON_PIN_1
#define BOTH_PIN 32//BUTTON_PIN_1
#define SPOKES_ONLY_PIN 34//BUTTON_PIN_3
#define WHEELS_ONLY_PIN 35//BUTTON_PIN_4
#define HUE1_PIN 36//BUTTON_PIN_1
#define HUE2_PIN BUTTON_PIN_6
#define HUE3_PIN BUTTON_PIN_7
#define HUE4_PIN BUTTON_PIN_8
#define HUE5_PIN BUTTON_PIN_9
#define INVERSE_PIN BUTTON_PIN_1
#define FADE_PIN ANALOG_PIN_2
#define POS_PIN ANALOG_PIN_1
//#define FPS_PIN ANALOG_PIN_3

#define NUM_MED_RINGS 0
#define NUM_LARGE_RINGS 0

#define NUM_LEDS_PER_MED_RING 16
#define NUM_LEDS_PER_LARGE_RING 24
#define NUM_MED_RING_LEDS (NUM_LEDS_PER_MED_RING + NUM_MED_RINGS)
#define NUM_LARGE_RING_LEDS (NUM_LEDS_PER_LARGE_RING + NUM_LARGE_RINGS)
#define NUM_RING_LEDS (NUM_LARGE_RING_LEDS + NUM_MED_RING_LEDS)
#define NUM_LEDS (NUM_RING_LEDS)

CRGB leds[NUM_LEDS];

CRGB *ledLargeRingSets[NUM_LARGE_RINGS];
CRGB *ledMedRingSets[NUM_MED_RINGS];

// Global LED values.
bool gActiveSpokes = false;
bool gSpokesOnly = false;
bool gWheelsOnly = false;

// Buttons.
struct Button
{
    uint8_t pin;
    uint8_t state;
    bool canChangeState;
    void (*callback)();
    
    Button(uint8_t pin) : pin (pin), state {0}, canChangeState {true}
    {}
};

Button patternButton = { PATTERN_PIN };
Button inverseButton = { INVERSE_PIN };
Button bothButton = { BOTH_PIN };
Button spokesOnlyButton = { SPOKES_ONLY_PIN };
Button wheelsOnlyButton = { WHEELS_ONLY_PIN };
Button hue1Button = { HUE1_PIN };
Button hue2Button = { HUE2_PIN };
Button hue3Button = { HUE3_PIN };
Button hue4Button = { HUE4_PIN };
Button hue5Button = { HUE5_PIN };

// Ring options.
enum RingPosition { Top, Bottom, Full, Empty };
enum RingSize { Large, Medium };

void setup()
{
    // initialize serial communication at 9600 bits per second:
    Serial.begin(9600);
    Serial2.begin(9600);

    Serial.println("Serial port opened.");
    
    delay(3000); // 3 second delay for recovery
    //FastLED
    FastLED.addLeds<WS2812B, DATA_PIN, RGB>(leds, NUM_LEDS);  

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
    pinMode(BUTTON_PIN_7, INPUT);
    pinMode(BUTTON_PIN_8, INPUT);
    pinMode(BUTTON_PIN_9, INPUT);
    pinMode(BUTTON_PIN_10, INPUT);
    pinMode(BUTTON_PIN_11, INPUT);
    pinMode(BUTTON_PIN_12, INPUT);
    pinMode(ANALOG_PIN_1, INPUT);
    pinMode(ANALOG_PIN_2, INPUT);
}

void setupLedArrays()
{
    for (int i = 0; i < NUM_LARGE_RINGS; i++)
    {
        for (int j = 0; j < NUM_LEDS_PER_LARGE_RING; j++)
        {
            ledLargeRingSets[i] = &leds[(i * NUM_LEDS_PER_LARGE_RING) + j];
        }
    }
    for (int i = 0; i < NUM_MED_RINGS; i++)
    {
        for (int j = 0; j < NUM_LEDS_PER_MED_RING; j++)
        {
            ledMedRingSets[i] = &leds[(i * NUM_LEDS_PER_MED_RING) + j];
        }
    }
}

void setupButtons()
{  
    patternButton.callback = nextPattern;
    bothButton.callback = setBoth;
    spokesOnlyButton.callback = setSpokesOnly;
    wheelsOnlyButton.callback = setWheelsOnly;
    hue1Button.callback = toggleHue1;
    hue2Button.callback = toggleHue2;
    hue3Button.callback = toggleHue3;
    hue4Button.callback = toggleHue4;
    hue5Button.callback = toggleHue5;
    inverseButton.callback = toggleInverse;
}

void loop()
{
    checkButtonStates();

    checkModifiers();

    if (!MeltdownLED.GetPause())
    {
        // Wheel pattern.
        // Call the current pattern function once, updating the 'leds' array
        //gPatterns[gCurrentPatternNumber](ledLargeRingSets, NUM_LARGE_RING_LEDS);

        //if (gInverse) invert(leds, NUM_LEDS);
        
        // Spoke pattern.
        // Call the current pattern function once, updating the 'leds' array
        //gPatterns[getNextPatternNumber()](ledMedRingSets, NUM_MED_RING_LEDS);

        // send the 'leds' array out to the actual LED strip
        //LEDS.show();
    }

    // insert a delay to keep the framerate modest
    //LEDS.delay(1000 / gFps);
 
    //gutCheck();
}

void gutCheck()
{   
   
}

void nextPattern()
{
    // Set to black.
    setColor(leds, NUM_LEDS, CRGB::Black);
    
    MeltdownLED.NextPattern();

    #if DEBUG
        Serial.println("Next Pattern...");
    #endif
    sendBoolCommand("PTN1", true);
}

#pragma region SET MODIFIERS

void checkModifiers()
{
    //setBrightness();
    // setFps();
    setPosition();
    //setFade();
}

void setBrightness()
{
    // int32_t minVal = 0;
    // int32_t maxVal = 100;
    // float currVal = gBrightness;

    // gBrightness = MeltdownLED.GetAnalogValue(BRIGHTNESS_PIN, currVal, minVal, maxVal);
    // LEDS.setBrightness(gBrightness);

    // if (MeltdownLED.HasChanged(currVal, gBrightness))
    // {
    // #if DEBUG
    //     Serial.print("Setting Brightness: ");
    //     Serial.println(gBrightness);
    // #endif
    //     sendCommand("BRIT", gBrightness);
    // }
}

void toggleInverse()
{
    bool inverseVal = MeltdownLED.ToggleInverse();
    #if DEBUG
        Serial.print("Setting Inverse: ");
        Serial.println(inverseVal);
    #endif
    sendCommand("INVR", inverseVal);
}

void setFps()
{
    // int32_t minVal = 500;
    // int32_t maxVal = 5000;
    // float currVal = gFps;

    // gFps = MeltdownLED.GetAnalogValue(FPS_PIN, currVal, minVal, maxVal);

    // if (MeltdownLED.HasChanged(currVal, gFps))
    // {
    // #if DEBUG
    //     Serial.print("Setting FPS: ");
    //     Serial.println(gFps);
    // #endif
    //     sendCommand("SPED", gFps);
    // }
}

void toggleHue1() { toggleHue(1); }
void toggleHue2() { toggleHue(2); }
void toggleHue3() { toggleHue(3); }
void toggleHue4() { toggleHue(4); }
void toggleHue5() { toggleHue(5); }

void toggleHue(uint8_t index)
{
    bool hueValue = MeltdownLED.ToggleHue(index);
    #if DEBUG
        Serial.print("Toggling Hue...");
    #endif
    sendCommand("HUE" + index, hueValue);
}

void setFade()
{
    float currVal = MeltdownLED.GetFade();
    float fadeVal = MeltdownLED.SetFade(FADE_PIN);

    if (MeltdownLED.HasChanged(currVal, fadeVal))
    {
    #if DEBUG
        Serial.print("Setting Fade: ");
        Serial.println(fadeVal);
    #endif
        sendCommand("FADE", fadeVal);
    }
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

void setPosition()
{
    float currVal = MeltdownLED.GetPosition();
    float posVal = MeltdownLED.SetPosition(POS_PIN);

    if (MeltdownLED.HasChanged(currVal, posVal))
    {
        #if DEBUG
            Serial.print("Setting Position: ");
            Serial.println(posVal);
        #endif
        sendCommand("POSN", posVal);
    }
}

void setPause(bool isPaused)
{
    MeltdownLED.SetPause(isPaused);

    #if DEBUG
        Serial.print("Setting Pause: ");
        Serial.println(isPaused);
    #endif
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

void setRingColor(CRGB *ringLeds[], RingPosition position, RingSize size, CRGB::HTMLColorCode color)
{
    uint8_t numLeds = getNumRingLeds(position, size);

    CRGB** leds = getRingLeds(ringLeds, position, size);

    if (position == Empty) color = CRGB::Black;

    for (int i = 0; i < numLeds; i++)
    {
        *leds[i] = color;
    }
}

CRGB** getRingLeds(CRGB *ringLeds[], RingPosition position, RingSize size)
{
    // // If the position is full or empty, we'll want all of our lights.
    // if (position == Full || position == Empty) return ringLeds;

    // uint8_t numLeds = getNumRingLeds(position, size);

    // CRGB *halfLeds[numLeds];

    // for (int i = 0; i < numLeds; i++)
    // {
    //     if (position == Top)
    //     {
    //         halfLeds[i] = ringLeds[i];
    //     }
    //     else
    //     {
    //         halfLeds[i] = ringLeds[i + numLeds];
    //     }
    // }

    // return halfLeds;
}

uint8_t getNumRingLeds(RingPosition position, RingSize size)
{
    uint8_t numLeds = NUM_LEDS_PER_LARGE_RING;
    if (size == Medium)
    {
        numLeds = NUM_LEDS_PER_MED_RING;
    }

    if (position == Top || position == Bottom)
    {
        return numLeds / 2;
    }
    else
    {
        return numLeds;
    }
}

#pragma endregion PATTERNS

#pragma region INPUTS

void checkButtonStates()
{
    checkButtonState(&patternButton);
    checkButtonState(&bothButton);
    checkButtonState(&spokesOnlyButton);
    checkButtonState(&wheelsOnlyButton);
    checkButtonState(&hue1Button);
    checkButtonState(&hue2Button);
    checkButtonState(&hue3Button);
    checkButtonState(&hue4Button);
    checkButtonState(&hue5Button);
    checkButtonState(&inverseButton);
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

void tryExecuteCommand()
{
}

void sendBoolCommand(String command, bool value)
{
    sendCommand(command, value ? 1 : 0);
}

void sendCommand(String command, float value)
{
    String serialCommand = MeltdownLED.PrepareCommand(command, value);
    
    if (Serial2.available())
    {
        Serial.print("Sending command: " + serialCommand);
        for (int i = 0; i < serialCommand.length(); i++)
        {
            Serial2.write(serialCommand[i]);   // Push each char 1 by 1 on each loop pass
        }
    }
}

#pragma endregion COMMANDS
