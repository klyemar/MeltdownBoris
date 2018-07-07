#include <Arduino.h>

class CMeltdownLED
{
    uint8_t m_analogTolerance;

    public: CMeltdownLED();

    float GetAnalogValue(uint8_t pin, float currVal, int32_t minVal, int32_t maxVal)
    {
        uint16_t rawVal = analogRead(pin);
        float normalizedVal = map(rawVal, 0, 1023, minVal, maxVal);

        if (HasChanged(currVal, normalizedVal, minVal, maxVal))
        {
            return normalizedVal;
        }

        return currVal;
    }

    // Because an analog read can waiver between values, we need to determine if an analog value has changed enough
    // for us to do anything about.
    bool HasChanged(float oldVal, float newVal, int32_t minVal, int32_t maxVal)
    {
        // If the value has changed to the min or max, return true regardless of tolerance.
        if ((oldVal != minVal && newVal == minVal) || (oldVal != maxVal && newVal == maxVal))
        {
            return true;
        }
        // Otherwise, check that the value has surpassed the tolerance threshold.
        return newVal <= (oldVal - m_analogTolerance) || newVal >= (oldVal + m_analogTolerance);
    }
};

extern CMeltdownLED MeltdownLED;