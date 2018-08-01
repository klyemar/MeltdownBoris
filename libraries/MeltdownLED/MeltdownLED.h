#include <Arduino.h>
#include <FastLED.h>

class CMeltdownLED
{
    uint8_t m_analogTolerance;

    public: CMeltdownLED();

    float GetAnalogValue(uint8_t pin, float currVal, int32_t minVal, int32_t maxVal)
    {
        uint16_t rawVal = analogRead(pin);
        float normalizedVal = map(rawVal, 0, 1023, minVal, maxVal);

        if (HasChanged(currVal, normalizedVal))
        {
            return normalizedVal;
        }

        return currVal;
    }

    float GetSerialValue(String inputString, float currVal, int32_t minVal, int32_t maxVal)
    {
        if (inputString[0] == '#' && inputString.length() >= 10)
        {
            String valString = inputString.substring(5, 9);
            int val = valString.toInt();

            // A value of 0 indicates an error but is acceptable if the intended value is actually 0.
            if (val != 0 || valString.equals("0000"))
            {
                return val;
            }
        }

        return currVal;
    }

    // Because an analog read can waiver between values, we need to determine if an analog value has changed enough
    // for us to do anything about.
    bool HasChanged(float oldVal, float newVal)
    {
        // If the value has changed to the min or max, return true regardless of tolerance.
        // if ((oldVal != minVal && newVal == minVal) || (oldVal != maxVal && newVal == maxVal))
        // {
        //     return true;
        // }
        float normalizedOldVal = map(oldVal, 0, 1023, 0, 1023);
        float normalizedNewVal = map(newVal, 0, 1023, 0, 1023);

        // Otherwise, check that the value has surpassed the tolerance threshold.
        return normalizedNewVal <= (normalizedOldVal - m_analogTolerance) || normalizedNewVal >= (normalizedOldVal + m_analogTolerance);
    }

    // Because an analog read can waiver between values, we need to determine if an analog value has changed enough
    // for us to do anything about.
    // bool HasChanged(float oldVal, float newVal)
    // {
    //     // If the value has changed to the min or max, return true regardless of tolerance.
    //     // if ((oldVal != minVal && newVal == minVal) || (oldVal != maxVal && newVal == maxVal))
    //     // {
    //     //     return true;
    //     // }
    //     // Otherwise, check that the value has surpassed the tolerance threshold.
    //     return newVal <= (oldVal - m_analogTolerance) || newVal >= (oldVal + m_analogTolerance);
    // }
};

extern CMeltdownLED MeltdownLED;