//#include <Arduino.h>
#include <FastLED.h>
#include <MeltdownSerial.h>

namespace Meltdown
{

#ifndef MELTDOWN_LED
#define MELTDOWN_LED

#define ARRAY_SIZE(A) (sizeof(A) / sizeof((A)[0]))

	class CMeltdownLED
	{
		public: CMeltdownLED();

		// Global LED values.
		int gCurrentPatternNumber = 0;
		int gCurrentEffectNumber = 0;
		int gCurrentModeNumber = 0;

		unsigned int gBrightness = 48;
		unsigned int gHue = 0;
		unsigned int gFps = 500;
		unsigned long gAnalogPattern = 0;
		unsigned long gAnalogEffect = 0;
		unsigned long gPos = 0;
		unsigned long gFade = 20;
		unsigned long gFrame = 0;
		bool gHue1 = false;
		bool gHue2 = false;
		bool gHue3 = false;
		bool gHue4 = false;
		bool gHue5 = false;
		bool gInverse = false;
		bool gGlitter = false;
		bool gPause = false;
		bool gTop = false;
		bool gBottom = false;
		bool gSleeping = false;

		unsigned long sleepStartMillis;
		unsigned long currentMillis;
		const unsigned long sleepPeriod = 1000L * 60 * 30; // 30 minutes

		// List of patterns to cycle through.  Each is defined as a separate function below.
		typedef void (CMeltdownLED::*SimplePatternList)(CRGB*[], int, int);
		SimplePatternList gPatterns[6] = {
			&CMeltdownLED::Rainbow,
			&CMeltdownLED::Confetti,
			&CMeltdownLED::Sinelon,
			&CMeltdownLED::Bpm,
			&CMeltdownLED::Juggle,
			&CMeltdownLED::MeteorRain
		};

		// List of modes to cycle through.  Each is defined as a separate function below.
		typedef void (CMeltdownLED::*ModeList)(CRGB*[], int);
		ModeList gEffects[2] = {
			&CMeltdownLED::NullEffect,
			&CMeltdownLED::GlitterEffect
		};

		// ANALOG //

		int SetAnalogPattern(int pin)
		{
			gAnalogPattern = MeltdownSerial.GetAnalogValue(pin, gAnalogPattern);
			return gAnalogPattern;
		}
		int GetAnalogPattern() { return gAnalogPattern; }

		int GetAnalogPattern(int minVal, int maxVal) { return map(gAnalogPattern, 0, 1023, minVal, maxVal); }

		int SetAnalogEffect(int pin)
		{
			gAnalogEffect = MeltdownSerial.GetAnalogValue(pin, gAnalogEffect);
			return gAnalogEffect;
		}
		int GetAnalogEffect() { return gAnalogEffect; }

		int GetAnalogEffect(int minVal, int maxVal) { return map(gAnalogEffect, 0, 1023, minVal, maxVal); }

		// BRIGHTNESS //

		int SetBrightness(int pin)
		{
			gBrightness = MeltdownSerial.GetAnalogValue(pin, gBrightness);
			LEDS.setBrightness(gBrightness);

			return gBrightness;
		}
		int GetBrightness() { return gBrightness; }

		int GetBrightness(int minVal, int maxVal) { return map(gBrightness, 0, 1023, minVal, maxVal); }

		// FPS //

		int GetFps() { return gFps; }

		// HUE //

		bool ToggleHue(int index)
		{
			bool hueVal = false;
			switch (index)
			{
			case 1:
				gHue1 = !gHue1;
				hueVal = gHue1;
			case 2:
				gHue2 = !gHue2;
				hueVal = gHue2;
			case 3:
				gHue3 = !gHue3;
				hueVal = gHue3;
			case 4:
				gHue4 = !gHue4;
				hueVal = gHue4;
			case 5:
				gHue5 = !gHue5;
				hueVal = gHue5;
			}
			SetHue();

			return hueVal;
		}
		void SetHue()
		{
			int val = 0;
			if (gHue1) val++;
			if (gHue2) val++;
			if (gHue3) val++;
			if (gHue4) val++;
			if (gHue5) val++;

			gHue = map(val, 0, 6, 0, 255);
		}
		int GetHue() { return gHue; }

		// PAUSE //

		bool TogglePause()
		{
			gPause = !gPause;
			return gPause;
		}
		void SetPause() { gPause = MeltdownSerial.GetBoolValue(); }

		bool GetPause() { return gPause; }

		// SLEEP //

		bool SetSleeping(bool isSleeping)
		{
			gSleeping = isSleeping;
			// If we're waking up, reset the timer.
			if (!gSleeping)
			{
				sleepStartMillis = millis();
			}
			return gSleeping;
		}
		void SetSleeping() { gSleeping = MeltdownSerial.GetBoolValue(); }

		bool GetSleeping() { return gSleeping; }

		// POSITION //

		bool ToggleTop()
		{
			gBottom = false;
			gTop = !gTop;
			return gTop;
		}
		void SetTop() { gTop = MeltdownSerial.GetBoolValue(); }

		bool GetTop() { return gTop; }

		bool ToggleBottom()
		{
			gTop = false;
			gBottom = !gBottom;
			return gBottom;
		}
		void SetBottom() { gBottom = MeltdownSerial.GetBoolValue(); }

		bool GetBottom() { return gBottom; }

		void ResetFrame() { gFrame = 0; }

		void IncrementFrame() { gFrame++; }

		void SetFrame() { gFrame = MeltdownSerial.GetValue(); }

		long GetFrame(long maxFrames) { return gFrame % maxFrames; }

#pragma region EFFECTS

		int GetEffectNumber(int offset = 0) { return (gCurrentEffectNumber + offset) % ARRAY_SIZE(gEffects); }

		void NextEffect() { gCurrentEffectNumber = GetEffectNumber(1); }

		void ExecuteEffect(CRGB *ledSet[], int numLeds, int offset = 0) { (this->*(gEffects[GetEffectNumber(offset)]))(ledSet, numLeds); }

		void NullEffect(CRGB *ledSet[], int numLeds) { }

		void GlitterEffect(CRGB *ledSet[], int numLeds)
		{
			int chanceOfGlitter = GetAnalogEffect(15, 115);
			if (random8() < chanceOfGlitter)
			{
				SetRandomColor(ledSet, numLeds, 3, CRGB::White);
			}
		}

		void HueIncrementEffect(CRGB *ledSet[], int numLeds)
		{
			static int hue = 0;

			int effectVal = GetAnalogEffect(100, 500);
			EVERY_N_MILLIS(effectVal)
			{
				for (int i = 0; i < numLeds; i++)
				{
					// *ledSet[i]
				}
			}
		}

#pragma endregion EFFECTS

#pragma region MODES

		int GetModeNumber(int numModes, int offset = 0) { return (gCurrentModeNumber + offset) % (numModes + 1); }

		int NextMode() { return gCurrentModeNumber++; }

		void Invert(CRGB *ledSet[], int numLeds)
		{
			for (int i = 0; i < numLeds; i++)
			{
				*ledSet[i] = -*ledSet[i];
			}
		}

		void MaximizeBrightness(CRGB *ledSet[], int numLeds)
		{
			for (int i = 0; i < numLeds; i++)
			{
				(*ledSet[i]).maximizeBrightness();
			}
		}

#pragma endregion MODES

#pragma region TIMERS

		void InitTimers()
		{
			sleepStartMillis = millis();
		}

		bool IsTimerOver(unsigned long period, unsigned long timerMillis)
		{
			currentMillis = millis();
			return currentMillis - timerMillis >= period;
		}

		// Returns true as soon as the sleep timer has been exceeded.
		bool CheckSleepTimer()
		{
			// Skip if already sleeping.
			if (!GetSleeping())
			{
				if (IsTimerOver(sleepPeriod, sleepStartMillis))
				{
					return SetSleeping(true);
				}
			}
			return false;
		}

#pragma endregion TIMERS

#pragma region PATTERNS

		int GetPatternNumber(int offset = 0) { return (gCurrentPatternNumber + offset) % ARRAY_SIZE(gPatterns); }

		void NextPattern() 
		{ 
			ResetFrame();

			gCurrentPatternNumber = GetPatternNumber(1); 
		}

		void ExecutePattern(CRGB *ledSet[], int numLeds, int patternOffset = 0, int modeOffset = 0) 
		{ 
			(this->*(gPatterns[GetPatternNumber(patternOffset)]))(ledSet, numLeds, modeOffset); 

			IncrementFrame();
		}

		void SetAllColor(CRGB *ledSet[], int numLeds, CRGB::HTMLColorCode color)
		{
			for (int i = 0; i < numLeds; i++)
			{
				*ledSet[i] = color;
			}
		}

		void SetAllColor(CRGB *ledSet[], int numLeds, CRGB color)
		{
			for (int i = 0; i < numLeds; i++)
			{
				*ledSet[i] = color;
			}
		}

		void SetRandomColor(CRGB *ledSet[], int numLeds, int numPositions, int hueOffset = 0)
		{
			for (int i = 0; i < numPositions; i++)
			{
				*ledSet[random16(numLeds)] += CHSV(gHue + random8(hueOffset), 200, 255);
			}
		}

		void SetRandomColor(CRGB *ledSet[], int numLeds, int numPositions, CRGB::HTMLColorCode color)
		{
			for (int i = 0; i < numPositions; i++)
			{
				*ledSet[random16(numLeds)] += color;
			}
		}


		void Rainbow(CRGB *ledSet[], int numLeds, int modeOffset = 0)
		{
			int deltaHue = GetAnalogPattern(1, 15);
			FillRainbow(ledSet, numLeds, gHue + gPos, deltaHue);

			// Modes
			int numModes = 1;
			switch (GetModeNumber(numModes, modeOffset))
			{
			case 1:
				// Invert rainbox colors.
				Invert(ledSet, numLeds);
				break;
			}
		}

		void Confetti(CRGB *ledSet[], int numLeds, int modeOffset = 0)
		{
			int fade = GetAnalogPattern(2, 30);
			FadeSetsToBlackBy(ledSet, numLeds, fade);

			// Modes
			int numModes = 2;
			switch (GetModeNumber(numModes, modeOffset))
			{
			case 1:
				SetRandomColor(ledSet, numLeds, 12, 64);
				break;
			case 2:
				SetRandomColor(ledSet, numLeds, 18, 64);
				break;
			default:
				SetRandomColor(ledSet, numLeds, 6, 64);
				break;
			}
		}

		void Sinelon(CRGB *ledSet[], int numLeds, int modeOffset = 0)
		{
			// Display a dot moving back and forth in a sin wave pattern.
			int fade = GetAnalogPattern(2, 30);
			FadeSetsToBlackBy(ledSet, numLeds, fade);

			int pos = beatsin16(8, 0, numLeds - 1);
			*ledSet[pos] += CHSV(gHue + 64, 255, 192);

			// Modes
			int numModes = 4;
			switch (GetModeNumber(numModes, modeOffset))
			{
				case 1:
				{
					// Add a second, opposite dot moving in the opposite direction.
					int opppositePos = (numLeds - 1) - (beatsin16(8, 0, numLeds - 1));
					*ledSet[opppositePos] += CHSV(gHue + 128, 255, 192);
					break;
				}
				case 2:
					GenerateSinelons(ledSet, numLeds, 2, pos);
					break;
				case 3:
					GenerateSinelons(ledSet, numLeds, 3, pos);
					break;
				case 4:
					GenerateSinelons(ledSet, numLeds, 5, pos);
					break;
			}
		}

		void GenerateSinelons(CRGB *ledSet[], int numLeds, int numSinelons, int pos)
		{
			for (int i = 0; i < numSinelons; i++)
			{
				int hue = gHue + (int)((255 / numSinelons) * i);

				*ledSet[(pos + (int)(numLeds / numSinelons) * (i + 1)) % numLeds] += CHSV(hue, 255, 192);
			}
		}

		void Bpm(CRGB *ledSet[], int numLeds, int modeOffset = 0)
		{
			int bpm = 60;
			int beat = beatsin8(bpm, 63, 255);
			int multiplier = GetAnalogPattern(2, 12);
			for (int i = 0; i < numLeds; i++)
			{
				*ledSet[i] = ColorFromPalette(GetPalette(), gHue + (i * multiplier), beat - gHue + (i * 10));
			}
		}

		void Juggle(CRGB *ledSet[], int numLeds, int modeOffset = 0)
		{
			int fade = GetAnalogPattern(3, 100);
			// Eight colored dots, weaving in and out of sync with each other.
			FadeSetsToBlackBy(ledSet, numLeds, fade);

			int numBalls = 3;

			// Modes
			int numModes = 3;
			switch (GetModeNumber(numModes, modeOffset))
			{
			case 1:
				numBalls = 5;
				break;
			case 2:
				numBalls = 7;
				break;
			case 3:
				numBalls = 9;
				break;
			}

			for (int i = 0; i < numBalls; i++)
			{
				int dotHue = i * (255 / numBalls);
				int pos = beatsin16(i + 7, 0, numLeds - 1);

				*ledSet[pos] |= CHSV(dotHue + gHue, 200, 255);
			}
		}

		void Sunrise(CRGB *ledSet[], int numLeds, int modeOffset = 0)
		{
			static const uint8_t sunriseMinutes = 8;

			static uint8_t heatIndex = 0;
			CRGB color = ColorFromPalette(HeatColors_p, (heatIndex % 255));

			SetAllColor(ledSet, numLeds, color);

			static const uint8_t interval = (sunriseMinutes * 60) / 256;
			EVERY_N_SECONDS(interval) {
				heatIndex++;
			}
		}

		void MeteorRain(CRGB *ledSet[], int numLeds, int modeOffset = 0) 
		{
			const boolean meteorRandomDecay = true;

			// Medium meteor.
			int meteorSize = 25;
			int speedDelay = 15;
			int frameMultiplier = 2;

			// Modes
			int numModes = 2;
			switch (GetModeNumber(numModes, modeOffset))
			{
				// Small meteors.
				case 1:
				{
					meteorSize = 5;
					speedDelay = 7;
					frameMultiplier = 2.5;
					break;
				}
				// Large meteor.
				case 2:
				{
					meteorSize = 50;
					speedDelay = 20;
					frameMultiplier = 2.5;
					break;
				}
			}

			// Fade brightness all LEDs one step.
			for (int i = 0; i < numLeds; i++)
			{
				if (!meteorRandomDecay || random(10) > 5) 
				{
					int meteorTrailDecay = GetAnalogPattern(32, 96);

					(*ledSet[i]).fadeToBlackBy(meteorTrailDecay);
				}
			}

			// Draw meteor.
			long frame = GetFrame(numLeds * frameMultiplier);
			for (int i = 0; i < meteorSize; i++)
			{
				if ((frame - i < numLeds) && (frame - i >= 0))
				{
					*ledSet[frame - i] = ColorGradientFromPalette(HeatColors_p, meteorSize, i, true);
				}
			}

			delay(speedDelay);
		}

		CRGBPalette16 GetPalette()
		{
			CRGBPalette16 palettes[5] = {
				PartyColors_p,
				ForestColors_p,
				CloudColors_p,
				HeatColors_p,
				RainbowColors_p
			};

			return palettes[gCurrentModeNumber % 5];
		}

		CRGB ColorGradientFromPalette(CRGBPalette16 palette, int numLeds, int pos, bool isReverse = false)
		{
			const int numPaletteColors = 255;
			// Validity check.
			if (pos > numLeds) pos = numLeds;

			// Get the appropriate color from the palette, mapped to the number of LEDs.
			int mappedPos = numPaletteColors / numLeds * pos;
			if (isReverse)
			{
				return ColorFromPalette(palette, (numPaletteColors - mappedPos));
			}
			else
			{
				return ColorFromPalette(palette, mappedPos);
			}
		}

		void FadeSetsToBlackBy(CRGB *ledSet[], int numLeds, int fade)
		{
			for (int i = 0; i < numLeds; i++)
			{
				int scale = 255 - fade;
				(*ledSet[i]).nscale8(scale);
			}
		}

		void FillRainbow(CRGB *ledSet[], int numLeds, int initialHue, int deltaHue)
		{
			CHSV hsv;
			hsv.hue = initialHue;
			hsv.val = 255;
			hsv.sat = 240;
			for (int i = 0; i < numLeds; i++)
			{
				*ledSet[i] = hsv;
				hsv.hue += deltaHue;
			}
		}

#pragma endregion PATTERNS
	};

	extern CMeltdownLED MeltdownLED;

#endif
}