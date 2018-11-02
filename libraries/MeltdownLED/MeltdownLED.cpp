#include "MeltdownLED.h"

CMeltdownLED MeltdownLED;

CMeltdownLED::CMeltdownLED() {
    m_analogTolerance = 2;

    m_inputString.reserve(100);
}