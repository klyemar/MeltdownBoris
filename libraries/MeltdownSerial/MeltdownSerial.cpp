#include "MeltdownSerial.h"

namespace Meltdown
{
	CMeltdownSerial MeltdownSerial;

	CMeltdownSerial::CMeltdownSerial()
	{
		m_analogTolerance = 15;
		m_inputString.reserve(100);
	}
}