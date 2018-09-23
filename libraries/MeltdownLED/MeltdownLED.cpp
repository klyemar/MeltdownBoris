#include "MeltdownLED.h"

CMeltdownLED MeltdownLED;

CMeltdownLED::CMeltdownLED() {
    m_analogTolerance = 10;

    m_inputString.reserve(100);
}