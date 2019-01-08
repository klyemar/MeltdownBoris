#include <Arduino.h>

namespace Meltdown
{

#ifndef MELTDOWN_LOGGER
#define MELTDOWN_LOGGER

	class CMeltdownLogger
	{
		public: CMeltdownLogger();

		bool m_isDebug = false;

		void InitSerial(bool isDebug)
		{
			m_isDebug = isDebug;
		}

		void Debug(Stream &serial, String msg)
		{
			if (!m_isDebug) return;

			serial.println(msg);
		}

		void Debug(Stream &serial, String msg, String value)
		{
			if (!m_isDebug) return;

			serial.print(msg);
			serial.print(": ");
			serial.println(value);
		}

		void Debug(Stream &serial, String msg, int value)
		{
			if (!m_isDebug) return;

			serial.print(msg);
			serial.print(": ");
			serial.println(value);
		}

		void Debug(Stream &serial, String msg, float value)
		{
			if (!m_isDebug) return;

			serial.print(msg);
			serial.print(": ");
			serial.println(value);
		}

		void Debug(Stream &serial, String msg, bool value)
		{
			if (!m_isDebug) return;

			serial.print(msg);
			serial.print(": ");
			serial.println(value);
		}
	};

	extern CMeltdownLogger MeltdownLogger;

#endif
}