#include <Arduino.h>
#include <MeltdownLogger.h>

namespace Meltdown
{

#ifndef MELTDOWN_SERIAL
#define MELTDOWN_SERIAL

	class CMeltdownSerial
	{
	public: CMeltdownSerial();

			int m_analogTolerance;

			// Serial input commands.
			String m_inputString;
			boolean m_inputStringComplete; // whether the String is complete

#pragma region INPUTS

			int GetAnalogValue(int currVal)
			{
				int val = currVal;

				if (!m_inputString.equals(""))
				{
					val = GetSerialValue(m_inputString, currVal);
				}

				return val;
			}

			int GetAnalogValue(int pin, int currVal)
			{
				if (pin == -1)
				{
					return GetAnalogValue(currVal);
				}
				else
				{
					int val = analogRead(pin);
					if (HasChanged(currVal, val))
					{
						return val;
					}

					return currVal;
				}
			}

			int GetSerialValue(String inputString, int currVal)
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
			bool HasChanged(int oldVal, int newVal)
			{
				// Otherwise, check that the value has surpassed the tolerance threshold.
				return newVal <= (oldVal - m_analogTolerance) || newVal >= (oldVal + m_analogTolerance);
			}

			String GetInputString() { return m_inputString; }

			void ClearInputString() { m_inputString = ""; }

			void AddCharToInputString(char inChar) { m_inputString += inChar; }

			bool GetInputStringComplete() { return m_inputStringComplete; }

			void SetInputStringComplete(bool value) { m_inputStringComplete = value; }

			String GetCommand()
			{
				if (m_inputString[0] == '#' && m_inputString.length() >= 6)
				{
					return m_inputString.substring(1, 5);
				}
				return "";
			}

			int GetValue()
			{
				int val = 0;
				if (m_inputString[0] == '#' && m_inputString.length() >= 10)
				{
					String valString = m_inputString.substring(5, 9);
					val = valString.toInt();
				}
				return val;
			}

			bool GetBoolValue()
			{
				int val = GetValue();
				return val != 0 ? true : false;
			}

#pragma endregion INPUTS

#pragma region COMMANDS

			const String PATTERN = "PTRN";
			const String EFFECT = "EFCT";
			const String MODE = "MODE";
			const String BRIGHTNESS = "BRIT";
			const String HUE1 = "HUE1";
			const String HUE2 = "HUE2";
			const String HUE3 = "HUE3";
			const String HUE4 = "HUE4";
			const String HUE5 = "HUE5";
			const String FULL_BRIGHT = "FLBR";
			const String TOP = "TOPP";
			const String BOTTOM = "BOTP";
			const String BOTH = "BOTH";
			const String ANALOG_PATTERN = "ANPT";
			const String ANALOG_EFFECT = "ANEF";
			const String PAUSE = "PAUS";
			const String ACTIVATE_AUTO = "AUAC";
			const String AUTO_NONE = "AUNO";
			const String AUTO_SLEEP = "AUSL";
			const String AUTO_PATTERN = "AUPT";
			const String AUTO_MODE = "AUMO";
			const String AUTO_PATTERN_MODE = "AUPM";

			String PrepareBoolCommand(String command, bool value) { return PrepareCommand(command, value ? 1 : 0); }

			String PrepareCommand(String command, int value)
			{
				if (value < 0) value = 0;
				if (value > 9999) value = 9999;

				String valueString = "";
				if (value < 1000) valueString += "0";
				if (value < 100) valueString += "0";
				if (value < 10) valueString += "0";
				valueString += value;

				return "#" + command + valueString + "\n";
			}

			void SendBoolCommand(Stream &serial1, Stream &serial2, String command, bool value)
			{
				SendCommand(serial1, serial2, command, value ? 1 : 0);
			}

			void SendCommand(Stream &serial1, Stream &serial2, String command, int value)
			{
				String serialCommand = PrepareCommand(command, value);

				if (serial2.availableForWrite())
				{
					MeltdownLogger.Debug(serial1, "Sending command", serialCommand);
					for (uint8_t i = 0; i < serialCommand.length(); i++)
					{
						serial2.write(serialCommand[i]);   // Push each char 1 by 1 on each loop pass.
					}
				}
				else
				{
					MeltdownLogger.Debug(serial1, "Failed to send command, Serial unavailable.");
				}
			}

#pragma endregion COMMANDS
	};

	extern CMeltdownSerial MeltdownSerial;

#endif
}