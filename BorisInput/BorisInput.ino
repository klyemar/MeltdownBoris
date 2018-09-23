#include <FastLED.h>
#include <MeltdownLED.h>

// Boris, short for Explora Borealis, is an interactive light display
// controlled by several individual user inputs.
//
// This project builds off of the FastLED library and some of its existing patterns
// and examples.

#define DEBUG true

#define LARGE_RING_PIN_1 53
#define LARGE_RING_PIN_2 51

#define BUTTON_PIN_1 22
#define BUTTON_PIN_2 28
#define BUTTON_PIN_3 30
#define BUTTON_PIN_4 32
#define BUTTON_PIN_5 34
#define BUTTON_PIN_6 36
#define BUTTON_PIN_7 38
// #define BUTTON_PIN_8 40
// #define BUTTON_PIN_9 42
// #define BUTTON_PIN_10 11
// #define BUTTON_PIN_11 12
// #define BUTTON_PIN_12 13
// #define BUTTON_PIN_13 31
#define ANALOG_PIN_1 A0
#define ANALOG_PIN_2 A1
//#define ANALOG_PIN_3 A1

#define PATTERN_PIN BUTTON_PIN_2
#define PAUSE_PIN 50 // BUTTON_PIN_2
//#define BOTH_PIN BUTTON_PIN_2
#define SPOKES_ONLY_PIN BUTTON_PIN_3
#define WHEELS_ONLY_PIN BUTTON_PIN_4
#define HUE1_PIN BUTTON_PIN_5
#define HUE2_PIN BUTTON_PIN_6
#define HUE3_PIN BUTTON_PIN_7
//#define HUE4_PIN BUTTON_PIN_8
//#define HUE5_PIN BUTTON_PIN_9
//#define INVERSE_PIN BUTTON_PIN_1
#define FADE_PIN ANALOG_PIN_2
#define POS_PIN ANALOG_PIN_1
//#define FPS_PIN ANALOG_PIN_3

#define NUM_MED_RINGS 0
#define NUM_LARGE_RINGS 2

#define NUM_LEDS_PER_MED_RING 16
#define NUM_LEDS_PER_LARGE_RING 24
#define NUM_MED_RING_LEDS (NUM_LEDS_PER_MED_RING + NUM_MED_RINGS)
#define NUM_LARGE_RING_LEDS (NUM_LEDS_PER_LARGE_RING + NUM_LARGE_RINGS)
#define NUM_RING_LEDS (NUM_LARGE_RING_LEDS + NUM_MED_RING_LEDS)
#define NUM_LEDS (NUM_RING_LEDS)

CRGB largeRingLeds1[NUM_LEDS_PER_LARGE_RING];
CRGB largeRingLeds2[NUM_LEDS_PER_LARGE_RING];

CRGB *largeRing1[NUM_LEDS_PER_LARGE_RING];
CRGB *largeRing2[NUM_LEDS_PER_LARGE_RING];

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
//Button inverseButton = { INVERSE_PIN };
//Button bothButton = { BOTH_PIN };
Button spokesOnlyButton = { SPOKES_ONLY_PIN };
Button wheelsOnlyButton = { WHEELS_ONLY_PIN };
Button hue1Button = { HUE1_PIN };
Button hue2Button = { HUE2_PIN };
Button hue3Button = { HUE3_PIN };
//Button hue4Button = { HUE4_PIN };
//Button hue5Button = { HUE5_PIN };
Button pauseButton = { PAUSE_PIN };

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
    FastLED.addLeds<WS2812, LARGE_RING_PIN_1, GRB>(largeRingLeds1, NUM_LEDS_PER_LARGE_RING);  
    FastLED.addLeds<WS2812, LARGE_RING_PIN_2, GRB>(largeRingLeds2, NUM_LEDS_PER_LARGE_RING);  

    // set master brightness control
    LEDS.setBrightness(MeltdownLED.GetBrightness());

    setupLedArrays();

    setupButtons();

    //pinMode(BUTTON_PIN_1, INPUT);
    pinMode(BUTTON_PIN_2, INPUT);
    // pinMode(BUTTON_PIN_3, INPUT);
    // pinMode(BUTTON_PIN_4, INPUT);
    pinMode(BUTTON_PIN_5, INPUT);
    pinMode(BUTTON_PIN_6, INPUT);
    pinMode(BUTTON_PIN_7, INPUT);
    // pinMode(BUTTON_PIN_8, INPUT);
    // pinMode(BUTTON_PIN_9, INPUT);
    // pinMode(BUTTON_PIN_10, INPUT);
    // pinMode(BUTTON_PIN_11, INPUT);
    // pinMode(BUTTON_PIN_12, INPUT);
    pinMode(ANALOG_PIN_1, INPUT);
    pinMode(ANALOG_PIN_2, INPUT);

    MeltdownLED.NextPattern();
}

void setupButtons()
{  
    patternButton.callback = nextPattern;
    // bothButton.callback = setBoth;
    spokesOnlyButton.callback = setSpokesOnly;
    wheelsOnlyButton.callback = setWheelsOnly;
    hue1Button.callback = toggleHue1;
    hue2Button.callback = toggleHue2;
    hue3Button.callback = toggleHue3;
    //hue4Button.callback = toggleHue4;
    // hue5Button.callback = toggleHue5;
    //inverseButton.callback = toggleInverse;
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
        MeltdownLED.ExecutePattern(largeRing1, NUM_LEDS_PER_LARGE_RING);
        MeltdownLED.ExecutePattern(largeRing2, NUM_LEDS_PER_LARGE_RING);

        //if (gInverse) invert(leds, NUM_LEDS);

        // send the 'leds' array out to the actual LED strip
        LEDS.show();
    }

    //EVERY_N_SECONDS( 5. ) { MeltdownLED.NextPattern(); } // change patterns periodically

    // insert a delay to keep the framerate modest
    LEDS.delay(1000 / MeltdownLED.GetFps());
 
    MeltdownLED.IncrementHue(1);
    
    EVERY_N_SECONDS( 20 ) { nextPattern(); } // change patterns periodically

    //gutCheck();
}

void gutCheck()
{   
    for (int i = 0; i < NUM_LEDS_PER_LARGE_RING; i++)
    {
        *largeRing1[i] = CRGB::Blue;
    }
    for (int i = 0; i < NUM_LEDS_PER_LARGE_RING; i++)
    {
        *largeRing2[i] = CRGB::Blue;
    }
    LEDS.show();
}

void setupLedArrays()
{   
    for (int i = 0; i < NUM_LEDS_PER_LARGE_RING; i++)
    {
        largeRing1[i] = &largeRingLeds1[i];
    }
    for (int i = 0; i < NUM_LEDS_PER_LARGE_RING; i++)
    {
        largeRing2[i] = &largeRingLeds2[i];
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
    setColor(largeRingLeds1, NUM_LEDS_PER_LARGE_RING, CRGB::Black);
    
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
    setFade();
}

void setBrightness()
{
    uint8_t currVal = MeltdownLED.GetBrightness();
    uint8_t brightVal = MeltdownLED.SetBrightness(-1);

    if (MeltdownLED.HasChanged(currVal, brightVal))
    {
    #if DEBUG
        Serial.print("Setting Brightness: ");
        Serial.println(brightVal);
    #endif
        sendCommand("BRIT", brightVal);
    }
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
        Serial.println("Toggling Hue...");
    #endif
    String command = "HUE";
    sendCommand(command + index, hueValue);
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

void setRingColor(CRGB *ringLeds[], uint8_t numLeds, RingPosition position, CRGB::HTMLColorCode color)
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

bool canColorRingLed(uint8_t index, uint8_t numLeds, RingPosition position)
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
    EVERY_N_MILLISECONDS( 20 ) 
    { 
        //checkButtonState(&patternButton);
        //checkButtonState(&pauseButton);
        //checkButtonState(&bothButton);
        // checkButtonState(&spokesOnlyButton);
        // checkButtonState(&wheelsOnlyButton);
        // checkButtonState(&hue1Button);
        // checkButtonState(&hue2Button);
        // checkButtonState(&hue3Button);
        //checkButtonState(&hue4Button);
        //checkButtonState(&hue5Button);
        //checkButtonState(&inverseButton);
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
