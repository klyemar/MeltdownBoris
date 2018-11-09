#include "MeltdownLED.h"

CMeltdownLED MeltdownLED;

CMeltdownLED::CMeltdownLED() {
    m_analogTolerance = 50;

    m_inputString.reserve(100);
}