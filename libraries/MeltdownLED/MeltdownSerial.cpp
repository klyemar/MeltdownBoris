#include "MeltdownSerial.h"

CMeltdownSerial MeltdownSerial;

CMeltdownSerial::CMeltdownSerial() 
{
    m_analogTolerance = 50;
    m_inputString.reserve(100);
}