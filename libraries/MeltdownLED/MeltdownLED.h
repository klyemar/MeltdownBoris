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

			int gNumModes = 0;
			int gBrightness = 48;
			int gHue = 0;
			int gDelay = 0;
			long gAnalogPattern = 0;
			long gAnalogEffect = 0;
			long gPos = 0;
			long gFade = 20;
			long gFrame = 1;
			int gFrameStep = 1;
			bool gHue1 = false;
			bool gHue2 = false;
			bool gHue3 = false;
			bool gHue4 = false;
			bool gHue5 = false;
			bool gMirror = false;
			bool gInverse = false;
			bool gGlitter = false;
			bool gPause = false;
			bool gBlack = false;
			bool gTop = false;
			bool gBottom = false;
			bool gAutoModeActive = false;

			enum AutoMode { None, Sleep, Pattern, Mode, PatternMode };
			AutoMode gAutoMode = None;

			unsigned long gCurrentMillis;
			// The millis at the time that the auto mode timer started.
			unsigned long gAutoModeStartMillis;
			// The number of minutes after which auto mode should begin.
			const unsigned long gAutoModePeriod = 1000L * 60 * 30; // 30 minutes

			// List of patterns to cycle through.  Each is defined as a separate function below.
			typedef void (CMeltdownLED::*SimplePatternList)(CRGB[], uint16_t[], int, int);
			SimplePatternList gPatterns[9] = {
				&CMeltdownLED::RunningLights,
				&CMeltdownLED::Juggle,
				//&CMeltdownLED::SolidColors,
				&CMeltdownLED::Rainbow,
				&CMeltdownLED::RainbowFull,
				&CMeltdownLED::Bpm,
				&CMeltdownLED::Sinelon,
				&CMeltdownLED::Confetti,
				&CMeltdownLED::BlendColor,
				&CMeltdownLED::MeteorRain,
				//&CMeltdownLED::Checkers,
			};

			// List of modes to cycle through.  Each is defined as a separate function below.
			typedef void (CMeltdownLED::* ModeList)(CRGB[], uint16_t[], int);
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

			int GetAnalogPattern(int minVal, int maxVal) { return mapFloat(gAnalogPattern, 0, 1023, minVal, maxVal); }

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

			// DELAY //

			int GetDelay() { return gDelay; }

			void ResetDelay() { gDelay = 0; }

			// HUE //

			/// Toggles the given Hue value. Since there can be a number of individual hue buttons, we toggle them individually.
			bool ToggleHue(int index)
			{
				bool hueVal = false;
				switch (index)
				{
				case 1:
					gHue1 = !gHue1;
					hueVal = gHue1;
					break;
				case 2:
					gHue2 = !gHue2;
					hueVal = gHue2;
					break;
				case 3:
					gHue3 = !gHue3;
					hueVal = gHue3;
					break;
				case 4:
					gHue4 = !gHue4;
					hueVal = gHue4;
					break;
				case 5:
					gHue5 = !gHue5;
					hueVal = gHue5;
					break;
				}
				SetHue();

				return hueVal;
			}

			/// Count and return the number of currently toggled hue values;
			int GetToggledHueCount()
			{
				int val = 0;
				if (gHue1) val++;
				if (gHue2) val++;
				if (gHue3) val++;
				if (gHue4) val++;
				if (gHue5) val++;

				return val;
			}

			bool HasToggledHues()
			{
				return GetToggledHueCount() > 0;
			}

			/// Return an arbitrary RGB color for a given number of toggled hue buttons. 
			/// This will be to add color to CRGBs, since they cannot have hue applied to them.
			CRGB GetRgbFromHue()
			{
				switch (GetToggledHueCount())
				{
				case 1:
					return CRGB::Purple;
				case 2:
					return CRGB::Green;
				case 3:
					return CRGB::Blue;
				case 4:
					return CRGB::Red;
				default:
					return CRGB::Black;
				}
			}

			void SetHue()
			{
				gHue = map(GetToggledHueCount(), 0, 6, 0, 255);
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

			// BLACK //

			bool ToggleBlack()
			{
				gBlack = !gBlack;
				return gBlack;
			}
			void SetBlack() { gBlack = MeltdownSerial.GetBoolValue(); }

			bool GetBlack() { return gBlack; }

			// MIRROR //

			bool ToggleMirror()
			{
				gMirror = !gMirror;
				return gMirror;
			}
			void SetMirror() { gMirror = MeltdownSerial.GetBoolValue(); }

			bool GetMirror() { return gMirror; }

			// AUTO MODE //

			void SetAutoMode(AutoMode mode)
			{
				gAutoMode = mode;

				if (mode == None)
				{
					SetAutoModeActive(false);
				}
				else
				{
					SetAutoModeActive(true);
				}
			}
			void SetAutoMode()
			{
				String mode = MeltdownSerial.GetCommand();

				if (mode == MeltdownSerial.AUTO_SLEEP)
				{
					gAutoMode = Sleep;
					SetAutoModeActive(true);
				}
				else if (mode == MeltdownSerial.AUTO_PATTERN)
				{
					gAutoMode = Pattern;
					SetAutoModeActive(true);
				}
				else if (mode == MeltdownSerial.AUTO_MODE)
				{
					gAutoMode = Mode;
					SetAutoModeActive(true);
				}
				else if (mode == MeltdownSerial.AUTO_PATTERN_MODE)
				{
					gAutoMode = PatternMode;
					SetAutoModeActive(true);
				}
				else
				{
					gAutoMode = None;
					SetAutoModeActive(false);
				}
			}

			AutoMode GetAutoMode() { return gAutoMode; }

			bool GetAutoModeEnabled() { return gAutoMode != None; }

			bool IsAutoPattern() { return gAutoMode == Pattern; }

			bool IsAutoMode() { return gAutoMode == Mode; }

			bool IsAutoPatternMode() { return gAutoMode == PatternMode; }

			bool IsAutoSleep() { return gAutoMode == Sleep; }

			bool SetAutoModeActive(bool isActive)
			{
				gAutoModeActive = isActive;
				// If we're waking up, reset the timer.
				if (!gAutoModeActive)
				{
					gAutoModeStartMillis = millis();
				}
				return gAutoModeActive;
			}
			void SetAutoModeActive() { gAutoModeActive = MeltdownSerial.GetBoolValue(); }

			bool GetAutoModeActive() { return gAutoModeActive; }

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

			void ResetFrame() { gFrame = 1; }

			void IncrementFrame(float multiplier = 1) 
			{ 
				gFrame += (GetFrameStep(multiplier)); 
			}

			void SetFrame() { gFrame = MeltdownSerial.GetValue(); }

			long GetFrame() 
			{ 
				if (gFrame < 1)
				{
					gFrame = 1;
				}
				return gFrame; 
			}

			long GetFrame(long maxFrames) { return gFrame % maxFrames; }

			void ResetFrameStep()
			{
				SetFrameStep(1);
			}

			int GetFrameStep(float multiplier = 1) 
			{ 
				// A value of less than zero indicates that we don't want to have any kind of modified increase, like in the case
				// of an especially long string of lights that would otherwise slow some patterns down.
				if (gFrameStep <= 0) return 1;

				return gFrameStep * multiplier; 
			}

			int SetFrameStep(int val) 
			{ 
				if (val < -1) val = -1;
				if (val == 0) val = 1;

				gFrameStep = val; 
			}

#pragma region EFFECTS

			int GetEffectNumber(int offset = 0) 
			{
				// A value less than zero indicates we want the base effect.
				if (offset < 0) return 0;

				return (gCurrentEffectNumber + offset) % ARRAY_SIZE(gEffects); 
			}

			int SetEffectNumber(int number)
			{
				gCurrentEffectNumber = number;
				return gCurrentEffectNumber;
			}

			int SetEffectNumber()
			{
				gCurrentEffectNumber = MeltdownSerial.GetValue();
				return gCurrentEffectNumber;
			}

			int IncrementEffectNumber() 
			{ 
				return SetEffectNumber(gCurrentEffectNumber + 1); 
			}

			void ExecuteEffect(CRGB leds[], uint16_t indexes[], int numLeds, int offset = 0) { (this->*(gEffects[GetEffectNumber(offset)]))(leds, indexes, numLeds); }

			void NullEffect(CRGB leds[], uint16_t indexes[], int numLeds) { }

			void GlitterEffect(CRGB leds[], uint16_t indexes[], int numLeds)
			{
				int chanceOfGlitter = GetAnalogEffect(25, 500);
				if (random8() < chanceOfGlitter)
				{
					SetRandomColor(leds, indexes, numLeds, 3, CRGB::White);
				}
			}

			void HueIncrementEffect(CRGB leds[], uint16_t indexes[], int numLeds)
			{
				static int hue = 0;

				int effectVal = GetAnalogEffect(100, 500);
				EVERY_N_MILLIS(effectVal)
				{
					for (int i = 0; i < numLeds; i++)
					{
						// leds[indexes[i]]
					}
				}
			}

#pragma endregion EFFECTS

#pragma region MODES

			int GetNumModes() { return gNumModes; }

			int SetNumModes(int number)
			{
				gNumModes = number;
				return gNumModes;
			}

			int GetModeNumber() { return gCurrentModeNumber; }

			int GetModeNumber(int offset) 
			{
				// A value less than zero indicates we want the base mode.
				if (offset < 0) return 0;

				return (gCurrentModeNumber + offset) % (gNumModes + 1);
			}

			int SetModeNumber(int number)
			{
				gCurrentModeNumber = number;
				return gCurrentModeNumber;
			}

			int SetModeNumber() 
			{ 
				gCurrentModeNumber = MeltdownSerial.GetValue(); 
				return gCurrentModeNumber;
			}

			int IncrementModeNumber() { return SetModeNumber(gCurrentModeNumber + 1); }

			void Invert(CRGB leds[], uint16_t indexes[], int numLeds)
			{
				for (int i = 0; i < numLeds; i++)
				{
					leds[indexes[i]] = -leds[indexes[i]];
				}
			}

			void MaximizeBrightness(CRGB leds[], uint16_t indexes[], int numLeds)
			{
				for (int i = 0; i < numLeds; i++)
				{
					(leds[indexes[i]]).maximizeBrightness();
				}
			}

#pragma endregion MODES

#pragma region TIMERS

			void InitTimers()
			{
				gAutoModeStartMillis = millis();
			}

			// Returns true if the difference between the current time and set time is greater than a specified period.
			bool IsTimerOver(unsigned long period, unsigned long timerMillis)
			{
				gCurrentMillis = millis();
				return gCurrentMillis - timerMillis >= period;
			}

			// Returns true as soon as the sleep timer has been exceeded.
			bool CheckAutoTimer()
			{
				// Skip if already in auto mode.
				if (!gAutoModeActive)
				{
					if (IsTimerOver(gAutoModePeriod, gAutoModeStartMillis))
					{
						return SetAutoModeActive(true);
					}
				}
				return false;
			}

#pragma endregion TIMERS

#pragma region UTILITY

			void SetAllColor(CRGB leds[], uint16_t indexes[], int numLeds, CRGB::HTMLColorCode color)
			{
				for (int i = 0; i < numLeds; i++)
				{
					leds[indexes[i]] = color;
				}
			}

			void SetAllColor(CRGB leds[], uint16_t indexes[], int numLeds, CRGB color)
			{
				for (int i = 0; i < numLeds; i++)
				{
					leds[indexes[i]] = color;
				}
			}

			void SetAllColor(CRGB leds[], uint16_t indexes[], int numLeds, int hue)
			{
				for (int i = 0; i < numLeds; i++)
				{
					leds[indexes[i]] = CHSV(hue, 255, 255);
				}
			}
			
			void FillGradientRgb(CRGB leds[], uint16_t indexes[], int numLeds,
				uint16_t startpos, CRGB startcolor,
				uint16_t endpos, CRGB endcolor)
			{
				// if the points are in the wrong order, straighten them
				if (endpos < startpos) 
				{
					uint16_t t = endpos;
					CRGB tc = endcolor;
					endcolor = startcolor;
					endpos = startpos;
					startpos = t;
					startcolor = tc;
				}

				saccum87 rdistance87;
				saccum87 gdistance87;
				saccum87 bdistance87;

				rdistance87 = (endcolor.r - startcolor.r) << 7;
				gdistance87 = (endcolor.g - startcolor.g) << 7;
				bdistance87 = (endcolor.b - startcolor.b) << 7;

				uint16_t pixeldistance = endpos - startpos;
				int16_t divisor = pixeldistance ? pixeldistance : 1;

				saccum87 rdelta87 = rdistance87 / divisor;
				saccum87 gdelta87 = gdistance87 / divisor;
				saccum87 bdelta87 = bdistance87 / divisor;

				rdelta87 *= 2;
				gdelta87 *= 2;
				bdelta87 *= 2;

				accum88 r88 = startcolor.r << 8;
				accum88 g88 = startcolor.g << 8;
				accum88 b88 = startcolor.b << 8;
				for (uint16_t i = startpos; i <= endpos; i++) 
				{
					leds[indexes[i]] = CRGB(r88 >> 8, g88 >> 8, b88 >> 8);
					r88 += rdelta87;
					g88 += gdelta87;
					b88 += bdelta87;
				}
			}

			void BlendAll(CRGB leds[], uint16_t indexes[], int numLeds, CRGB color, byte amount = 127)
			{
				for (int i = 0; i < numLeds; i++)
				{
					CRGB led = leds[indexes[i]];
					if (led)
					{
						nblend(leds[indexes[i]], color, amount);
					}
				}
			}

			void BlendFromHue(CRGB leds[], uint16_t indexes[], int numLeds, byte amount = 127)
			{
				if (GetToggledHueCount() == 0) return;

				BlendAll(leds, indexes, numLeds, GetRgbFromHue(), amount);
			}

			/// Generate random colors (withing a confinement of hue) in random locations.
			void SetRandomColor(CRGB leds[], uint16_t indexes[], int numLeds, int numPositions, int hueOffset = 0)
			{
				for (int i = 0; i < numPositions; i++)
				{
					uint8_t random = random8(1, 255);
					if ((float)numLeds / random > 1.5)
					{
						uint16_t index = indexes[random16(numLeds)];
						leds[index] = CHSV(gHue + random8(hueOffset), 200, 255);
					}
				}
			}

			/// Add a specific color in random locations.
			void SetRandomColor(CRGB leds[], uint16_t indexes[], int numLeds, int numPositions, CRGB::HTMLColorCode color)
			{
				for (int i = 0; i < numPositions; i++)
				{
					uint8_t random = random8(1, 255);
					if ((float)numLeds / random > 1.5)
					{
						uint16_t index = indexes[random16(numLeds)];
						leds[index] = color;
					}
				}
			}

			void GenerateSinelons(CRGB leds[], uint16_t indexes[], int numLeds, int numSinelons, int pos)
			{
				for (int i = 0; i < numSinelons; i++)
				{
					int hue = gHue + (int)((255 / numSinelons) * i);

					uint16_t index = indexes[(pos + (int)(numLeds / numSinelons) * (i + 1)) % numLeds];
					leds[index] = CHSV(hue, 255, 192);
				}
			}

			CRGB GetRainbowColor(int index = 0)
			{
				switch (index % 8)
				{
				case 0:
					return CRGB::Red;
				case 1:
					return CRGB::Orange;
				case 2:
					return CRGB::Yellow;
				case 3:
					return CRGB::Green;
				case 4:
					return CRGB::Blue;
				case 5:
					return CRGB::Indigo;
				case 6:
					return CRGB::Violet;
				case 7:
					return CRGB::Pink;
				}
			}

			CRGBPalette16 GetPalette(bool adjustHue = false)
			{
				return GetPalette(gCurrentModeNumber, adjustHue);
			}

			CRGBPalette16 GetPalette(int index, bool adjustHue = false)
			{
				CRGBPalette16 palettes[5] = {
					PartyColors_p,
					ForestColors_p,
					CloudColors_p,
					HeatColors_p,
					RainbowColors_p
				};

				if (adjustHue)
				{
					index += GetToggledHueCount();
				}

				return palettes[index % ARRAY_SIZE(palettes)];
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

			void FadeSetsToBlackBy(CRGB leds[], uint16_t indexes[], int numLeds, int fade)
			{
				for (int i = 0; i < numLeds; i++)
				{
					int scale = 255 - fade;
					(leds[indexes[i]]).nscale8(scale);
				}
			}

			void FillRainbow(CRGB leds[], uint16_t indexes[], int numLeds, int initialHue, int deltaHue)
			{
				CHSV hsv;
				hsv.hue = initialHue;
				hsv.val = 255;
				hsv.sat = 240;
				for (int i = 0; i < numLeds; i++)
				{
					leds[indexes[i]] = hsv;
					hsv.hue += deltaHue;
				}
			}

#pragma endregion UTILITY

#pragma region PATTERNS

			int GetPatternNumber(int offset = 0) 
			{
				// A value less than zero indicates we want the base pattern.
				if (offset < 0) return 0;

				return (gCurrentPatternNumber + offset) % ARRAY_SIZE(gPatterns); 
			}

			int SetPatternNumber(int number)
			{
				ResetFrame();
				ResetDelay();

				gCurrentPatternNumber = number;
				return gCurrentPatternNumber;
			}

			int SetPatternNumber()
			{
				ResetFrame();
				ResetDelay();

				gCurrentPatternNumber = MeltdownSerial.GetValue();
				return gCurrentPatternNumber;
			}

			int IncrementPatternNumber()
			{
				return SetPatternNumber(gCurrentPatternNumber + 1);
			}

			void ExecutePattern(CRGB leds[], uint16_t indexes[], int numLeds, int patternOffset = 0, int modeOffset = 0)
			{
				(this->*(gPatterns[GetPatternNumber(patternOffset)]))(leds, indexes, numLeds, modeOffset);
			}

			void BlendColor(CRGB leds[], uint16_t indexes[], int numLeds, int modeOffset = 0)
			{
				int speed = beatsin8(GetAnalogPattern(16, 48), 0, 255);

				// Modes
				SetNumModes(4);
				FillGradients(leds, indexes, numLeds, GetModeNumber(modeOffset) + 1, speed);
			}

			void FillGradients(CRGB leds[], uint16_t indexes[], int numLeds, int numGradients, int speed)
			{
				if (numGradients < 1) numGradients = 1;

				for (int i = 0; i < numGradients; i++)
				{
					CRGB color1 = GetRainbowColor(GetToggledHueCount() + i);
					CRGB color2 = GetRainbowColor(GetToggledHueCount() + i + 1);
					CRGB color3 = GetRainbowColor(GetToggledHueCount() + i + 2);

					// Blend between two different colors over time.
					CRGB blend1 = blend(color1, color2, speed);
					CRGB blend2 = blend(color2, color3, speed);

					int startPos = numLeds / numGradients * i;
					int endPos = (numLeds / numGradients * (i + 1)) - 1;

					FillGradientRgb(leds, indexes, numLeds, startPos, blend1, endPos, blend2);
				}
			}

			void SolidColors(CRGB leds[], uint16_t indexes[], int numLeds, int modeOffset = 0)
			{
				// Modes
				SetNumModes(10);
				switch (GetModeNumber(modeOffset))
				{
				case 1:
					SetAllColor(leds, indexes, numLeds, CRGB::OrangeRed);
					break;
				case 2:
					SetAllColor(leds, indexes, numLeds, CRGB::Orange);
					break;
				case 3:
					SetAllColor(leds, indexes, numLeds, CRGB::Yellow);
					break;
				case 4:
					SetAllColor(leds, indexes, numLeds, CRGB::GreenYellow);
					break;
				case 5:
					SetAllColor(leds, indexes, numLeds, CRGB::Green);
					break;
				case 6:
					SetAllColor(leds, indexes, numLeds, CRGB::Teal);
					break;
				case 7:
					SetAllColor(leds, indexes, numLeds, CRGB::Blue);
					break;
				case 8:
					SetAllColor(leds, indexes, numLeds, CRGB::Indigo);
					break;
				case 9:
					SetAllColor(leds, indexes, numLeds, CRGB::Violet);
					break;
				case 10:
					SetAllColor(leds, indexes, numLeds, CRGB::Magenta);
					break;
				default:
					SetAllColor(leds, indexes, numLeds, CRGB::Red);
				}
			}

			void Rainbow(CRGB leds[], uint16_t indexes[], int numLeds, int modeOffset = 0)
			{
				static int hue = 1;

				int deltaHue = GetAnalogPattern(1, 15);
				FillRainbow(leds, indexes, numLeds, gHue + gPos + hue, deltaHue);

				// Modes
				SetNumModes(3);
				switch (GetModeNumber(modeOffset))
				{
				case 1:
					// Increment the hue.
					hue++;
					break; 
				case 2:
					// Invert rainbox colors.
					Invert(leds, indexes, numLeds);
					break; 
				case 3:
					// Invert rainbox colors and increment the hue.
					Invert(leds, indexes, numLeds);
					hue++;
					break;
				}
			}

			void RainbowFull(CRGB leds[], uint16_t indexes[], int numLeds, int modeOffset = 0)
			{
				static int hue = 1;

				// Modes
				SetNumModes(3);
				switch (GetModeNumber(modeOffset))
				{
				case 1:
					if (GetFrame() % 2 == 0) hue++;
					break;
				case 2:
					hue++;
					break;
				case 3:
					hue += 2;
					break;
				default:
					if (GetFrame() % 3 == 0) hue++;
					break;
				}

				SetAllColor(leds, indexes, numLeds, hue);
			}

			void Confetti(CRGB leds[], uint16_t indexes[], int numLeds, int modeOffset = 0)
			{
				int fade = GetAnalogPattern(2, 30);
				FadeSetsToBlackBy(leds, indexes, numLeds, fade);

				// Modes
				SetNumModes(5);
				switch (GetModeNumber(modeOffset))
				{
				case 1:
					SetRandomColor(leds, indexes, numLeds, 4, 64);		// Orange
					SetRandomColor(leds, indexes, numLeds, 4, 0);		// Red
					break;
				case 2:
					SetRandomColor(leds, indexes, numLeds, 3, 64);		// Orange	
					SetRandomColor(leds, indexes, numLeds, 3, 0);		// Red
					SetRandomColor(leds, indexes, numLeds, 6, 128);	// Blue
					break;
				case 3:
					SetRandomColor(leds, indexes, numLeds, 2, 64);		// Orange
					SetRandomColor(leds, indexes, numLeds, 2, 0);		// Red
					SetRandomColor(leds, indexes, numLeds, 5, 128);	// Blue
					SetRandomColor(leds, indexes, numLeds, 5, 196);	// Purple
					break;
				case 4:
					SetRandomColor(leds, indexes, numLeds, 4, 128);	// Blue
					SetRandomColor(leds, indexes, numLeds, 4, 196);	// Purple
					break;
				case 5:
					SetRandomColor(leds, indexes, numLeds, 6, 196);	// Purple
					break;
				default:
					SetRandomColor(leds, indexes, numLeds, 6, 64);		// Orange
					break;
				}
			}

			void Sinelon(CRGB leds[], uint16_t indexes[], int numLeds, int modeOffset = 0)
			{
				// Display a dot moving back and forth in a sin wave pattern.
				int fade = GetAnalogPattern(2, 30);
				FadeSetsToBlackBy(leds, indexes, numLeds, fade);

				uint16_t index = indexes[beatsin16(8, 0, numLeds - 1)];
				leds[index] = CHSV(gHue + 64, 255, 192);

				// Modes
				SetNumModes(4);
				switch (GetModeNumber(modeOffset))
				{
				case 1:
				{
					// Add a second, opposite dot moving in the opposite direction.
					uint16_t oppositeIndex = indexes[(numLeds - 1) - (beatsin16(8, 0, numLeds - 1))];
					leds[oppositeIndex] = CHSV(gHue + 128, 255, 192);
					break;
				}
				case 2:
					GenerateSinelons(leds, indexes, numLeds, 2, index);
					break;
				case 3:
					GenerateSinelons(leds, indexes, numLeds, 3, index);
					break;
				case 4:
					GenerateSinelons(leds, indexes, numLeds, 5, index);
					break;
				}
			}

			void Bpm(CRGB leds[], uint16_t indexes[], int numLeds, int modeOffset = 0)
			{
				SetNumModes(4);

				int bpm = 60;
				int beat = beatsin8(bpm, 63, 255);
				int multiplier = GetAnalogPattern(2, 12);
				CRGBPalette16 palette = GetPalette(GetModeNumber(modeOffset));
				for (int i = 0; i < numLeds; i++)
				{
					leds[indexes[i]] = ColorFromPalette(palette, i * multiplier, beat - (i * 10));
				}
			}

			void Juggle(CRGB leds[], uint16_t indexes[], int numLeds, int modeOffset = 0)
			{
				int fade = GetAnalogPattern(3, 100);
				// Eight colored dots, weaving in and out of sync with each other.
				FadeSetsToBlackBy(leds, indexes, numLeds, fade);

				int numBalls = 3;

				// Modes
				SetNumModes(3);
				switch (GetModeNumber(modeOffset))
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
					uint16_t index = indexes[beatsin16(i + 7, 0, numLeds - 1)];
					leds[index] = CHSV(dotHue + gHue, 200, 255);
				}
			}

			void Sunrise(CRGB leds[], uint16_t indexes[], int numLeds, int modeOffset = 0)
			{
				static const uint8_t sunriseMinutes = 8;

				static uint8_t heatIndex = 0;
				CRGB color = ColorFromPalette(HeatColors_p, (heatIndex % 255));

				SetAllColor(leds, indexes, numLeds, color);

				static const uint8_t interval = (sunriseMinutes * 60) / 256;
				EVERY_N_SECONDS(interval) {
					heatIndex++;
				}
			}

			void MeteorRain(CRGB leds[], uint16_t indexes[], int numLeds, int modeOffset = 0)
			{
				SetFrameStep(-1);

				const boolean meteorRandomDecay = true;

				// Medium meteor.
				int meteorSize = 25;
				int frameMultiplier = 1;
				gDelay = 15;

				// Modes
				SetNumModes(2);
				switch (GetModeNumber(modeOffset))
				{
					// Small meteors.
					case 1:
					{
						meteorSize = 5;
						frameMultiplier = 1.5;
						gDelay = 7;
						break;
					}
					// Large meteor.
					case 2:
					{
						meteorSize = 50;
						frameMultiplier = 1;
						gDelay = 20;
						break;
					}
				}

				// Fade brightness all LEDs one step.
				for (int i = 0; i < numLeds; i++)
				{
					if (!meteorRandomDecay || random(10) > 5)
					{
						int meteorTrailDecay = GetAnalogPattern(32, 96);

						(leds[indexes[i]]).fadeToBlackBy(meteorTrailDecay);
					}
				}

				// Draw meteor.
				long frame = GetFrame(numLeds * frameMultiplier);
				for (int i = 0; i < meteorSize; i++)
				{
					if ((frame - i < numLeds) && (frame - i >= 0))
					{
						// Set the colors of the meteor using a reversed HeatColors gradient, so the meteor's tail is white hot trailing off to red.
						uint16_t index = indexes[frame - i];
						leds[index] = ColorGradientFromPalette(HeatColors_p, meteorSize, i, true);
						// Apply a blend from the hue, if toggled. We'll be using a designated arbitrary RGB color.
						if (HasToggledHues())
						{
							nblend(leds[index], GetRgbFromHue(), 127);
						}
					}
				}
			}

			void Checkers(CRGB leds[], uint16_t indexes[], int numLeds, int modeOffset = 0)
			{
				//FadeSetsToBlackBy(leds, indexes, numLeds, 50);

				SetNumModes(1);

				int checkerSize = 4;

				for (int i = 0; i < numLeds; i++)
				{
					int checker = i / 4;

					if ((checker % 2 != 0 && sin8(gFrame) < 128) || (checker % 2 == 0 && sin8(gFrame) >= 128))
					{
						leds[indexes[i]] = CRGB::Red;
					}
					else
					{
						leds[indexes[i]] = CRGB::Black;
					}
				}
			}

			void RunningLights(CRGB leds[], uint16_t indexes[], int numLeds, int modeOffset = 0)
			{
				SetNumModes(4);
				SetFrameStep(3);

				int length = GetAnalogPattern(4, 40);

				for (int i = 0; i < numLeds; i++)
				{
					double redSin;
					double greenSin;
					double blueSin;

					// Modes
					switch (GetModeNumber(modeOffset))
					{
						case 1:
						{
							redSin = (i * length) + (float)GetFrame() / 1.5;
							greenSin = (i * length) + (float)GetFrame();
							blueSin = (i * length) + (float)GetFrame() / 3;
							break;
						}
						case 2:
						{
							redSin = (i * length) + (float)GetFrame() * 1.5;
							greenSin = (i * length) + (float)GetFrame() * 3;
							blueSin = (i * length) + (float)GetFrame() / 1.5;
							break;
						}
						case 3:
						{
							redSin = (i * length) + (float)GetFrame();
							greenSin = (i * length / 4) - (float)GetFrame();
							blueSin = (i * length) + (float)GetFrame() / 2;
							break;
						}
						case 4:
						{
							redSin = (i * length) + (float)GetFrame();
							greenSin = 0;
							blueSin = (i * length / 2) + (float)GetFrame() / 2;
							break;
						}
						default:
						{
							redSin = 0; 
							greenSin = (i * length) + (float)GetFrame();
							blueSin = (i * length) + (float)GetFrame();
							break;
						}
					}

					byte redMult = sin8(redSin);
					byte greenMult = sin8(greenSin);
					byte blueMult = sin8(blueSin);

					leds[indexes[i]] = CRGB(redMult, greenMult, blueMult);
				}
			}


			//void theaterChase(byte red, byte green, byte blue, int SpeedDelay) {
			//	for (int j = 0; j < 10; j++) {  //do 10 cycles of chasing
			//		for (int q = 0; q < 3; q++) {
			//			for (int i = 0; i < NUM_LEDS; i = i + 3) {
			//				setPixel(i + q, red, green, blue);    //turn every third pixel on
			//			}
			//			showStrip();

			//			delay(SpeedDelay);

			//			for (int i = 0; i < NUM_LEDS; i = i + 3) {
			//				setPixel(i + q, 0, 0, 0);        //turn every third pixel off
			//			}
			//		}
			//	}
			//}

			//void theaterChaseRainbow(int SpeedDelay) {
			//	byte *c;

			//	for (int j = 0; j < 256; j++) {     // cycle all 256 colors in the wheel
			//		for (int q = 0; q < 3; q++) {
			//			for (int i = 0; i < NUM_LEDS; i = i + 3) {
			//				c = Wheel((i + j) % 255);
			//				setPixel(i + q, *c, *(c + 1), *(c + 2));    //turn every third pixel on
			//			}
			//			showStrip();

			//			delay(SpeedDelay);

			//			for (int i = 0; i < NUM_LEDS; i = i + 3) {
			//				setPixel(i + q, 0, 0, 0);        //turn every third pixel off
			//			}
			//		}
			//	}
			//}

			//byte *Wheel(byte WheelPos) 
			//{
			//	static byte c[3];

			//	if (WheelPos < 85) {
			//		c[0] = WheelPos * 3;
			//		c[1] = 255 - WheelPos * 3;
			//		c[2] = 0;
			//	}
			//	else if (WheelPos < 170) {
			//		WheelPos -= 85;
			//		c[0] = 255 - WheelPos * 3;
			//		c[1] = 0;
			//		c[2] = WheelPos * 3;
			//	}
			//	else {
			//		WheelPos -= 170;
			//		c[0] = 0;
			//		c[1] = WheelPos * 3;
			//		c[2] = 255 - WheelPos * 3;
			//	}

			//	return c;
			//}

			//void BouncingBalls(byte red, byte green, byte blue, int BallCount) 
			//{
			//	float Gravity = -9.81;
			//	int StartHeight = 1;

			//	float Height[BallCount];
			//	float ImpactVelocityStart = sqrt(-2 * Gravity * StartHeight);
			//	float ImpactVelocity[BallCount];
			//	float TimeSinceLastBounce[BallCount];
			//	int   Position[BallCount];
			//	long  ClockTimeSinceLastBounce[BallCount];
			//	float Dampening[BallCount];

			//	for (int i = 0; i < BallCount; i++) {
			//		ClockTimeSinceLastBounce[i] = millis();
			//		Height[i] = StartHeight;
			//		Position[i] = 0;
			//		ImpactVelocity[i] = ImpactVelocityStart;
			//		TimeSinceLastBounce[i] = 0;
			//		Dampening[i] = 0.90 - float(i) / pow(BallCount, 2);
			//	}

			//	while (true) {
			//		for (int i = 0; i < BallCount; i++) {
			//			TimeSinceLastBounce[i] = millis() - ClockTimeSinceLastBounce[i];
			//			Height[i] = 0.5 * Gravity * pow(TimeSinceLastBounce[i] / 1000, 2.0) + ImpactVelocity[i] * TimeSinceLastBounce[i] / 1000;

			//			if (Height[i] < 0) {
			//				Height[i] = 0;
			//				ImpactVelocity[i] = Dampening[i] * ImpactVelocity[i];
			//				ClockTimeSinceLastBounce[i] = millis();

			//				if (ImpactVelocity[i] < 0.01) {
			//					ImpactVelocity[i] = ImpactVelocityStart;
			//				}
			//			}
			//			Position[i] = round(Height[i] * (NUM_LEDS - 1) / StartHeight);
			//		}

			//		for (int i = 0; i < BallCount; i++) {
			//			setPixel(Position[i], red, green, blue);
			//		}

			//		showStrip();
			//		setAll(0, 0, 0);
			//	}


		//void Fire(int Cooling, int Sparking, int SpeedDelay) {
		//	static byte heat[NUM_LEDS];
		//	int cooldown;

		//	// Step 1.  Cool down every cell a little
		//	for (int i = 0; i < NUM_LEDS; i++) {
		//		cooldown = random(0, ((Cooling * 10) / NUM_LEDS) + 2);

		//		if (cooldown > heat[i]) {
		//			heat[i] = 0;
		//		}
		//		else {
		//			heat[i] = heat[i] - cooldown;
		//		}
		//	}

		//	// Step 2.  Heat from each cell drifts 'up' and diffuses a little
		//	for (int k = NUM_LEDS - 1; k >= 2; k--) {
		//		heat[k] = (heat[k - 1] + heat[k - 2] + heat[k - 2]) / 3;
		//	}

		//	// Step 3.  Randomly ignite new 'sparks' near the bottom
		//	if (random(255) < Sparking) {
		//		int y = random(7);
		//		heat[y] = heat[y] + random(160, 255);
		//		//heat[y] = random(160,255);
		//	}

		//	// Step 4.  Convert heat to LED colors
		//	for (int j = 0; j < NUM_LEDS; j++) {
		//		setPixelHeatColor(j, heat[j]);
		//	}

		//	showStrip();
		//	delay(SpeedDelay);
		//}

		//void setPixelHeatColor(int Pixel, byte temperature) {
		//	// Scale 'heat' down from 0-255 to 0-191
		//	byte t192 = round((temperature / 255.0) * 191);

		//	// calculate ramp up from
		//	byte heatramp = t192 & 0x3F; // 0..63
		//	heatramp <<= 2; // scale up to 0..252

		//	// figure out which third of the spectrum we're in:
		//	if (t192 > 0x80) {                     // hottest
		//		setPixel(Pixel, 255, 255, heatramp);
		//	}
		//	else if (t192 > 0x40) {             // middle
		//		setPixel(Pixel, 255, heatramp, 0);
		//	}
		//	else {                               // coolest
		//		setPixel(Pixel, heatramp, 0, 0);
		//	}
		//}
	//
	//void RGBLoop() {
	//	for (int j = 0; j < 3; j++) {
	//		// Fade IN
	//		for (int k = 0; k < 256; k++) {
	//			switch (j) {
	//			case 0: setAll(k, 0, 0); break;
	//			case 1: setAll(0, k, 0); break;
	//			case 2: setAll(0, 0, k); break;
	//			}
	//			showStrip();
	//			delay(3);
	//		}
	//		// Fade OUT
	//		for (int k = 255; k >= 0; k--) {
	//			switch (j) {
	//			case 0: setAll(k, 0, 0); break;
	//			case 1: setAll(0, k, 0); break;
	//			case 2: setAll(0, 0, k); break;
	//			}
	//			showStrip();
	//			delay(3);
	//		}
	//	}
	//}
	//
	//void FadeInOut(byte red, byte green, byte blue) {
	//	float r, g, b;
	//
	//	for (int k = 0; k < 256; k = k + 1) {
	//		r = (k / 256.0)*red;
	//		g = (k / 256.0)*green;
	//		b = (k / 256.0)*blue;
	//		setAll(r, g, b);
	//		showStrip();
	//	}
	//
	//	for (int k = 255; k >= 0; k = k - 2) {
	//		r = (k / 256.0)*red;
	//		g = (k / 256.0)*green;
	//		b = (k / 256.0)*blue;
	//		setAll(r, g, b);
	//		showStrip();
	//	}
	//}
	//
	//void Strobe(byte red, byte green, byte blue, int StrobeCount, int FlashDelay, int EndPause) {
	//	for (int j = 0; j < StrobeCount; j++) {
	//		setAll(red, green, blue);
	//		showStrip();
	//		delay(FlashDelay);
	//		setAll(0, 0, 0);
	//		showStrip();
	//		delay(FlashDelay);
	//	}
	//
	//	delay(EndPause);
	//}
	//
	//void HalloweenEyes(byte red, byte green, byte blue,
	//	int EyeWidth, int EyeSpace,
	//	boolean Fade, int Steps, int FadeDelay,
	//	int EndPause) {
	//	randomSeed(analogRead(0));
	//
	//	int i;
	//	int StartPoint = random(0, NUM_LEDS - (2 * EyeWidth) - EyeSpace);
	//	int Start2ndEye = StartPoint + EyeWidth + EyeSpace;
	//
	//	for (i = 0; i < EyeWidth; i++) {
	//		setPixel(StartPoint + i, red, green, blue);
	//		setPixel(Start2ndEye + i, red, green, blue);
	//	}
	//
	//	showStrip();
	//
	//	if (Fade == true) {
	//		float r, g, b;
	//
	//		for (int j = Steps; j >= 0; j--) {
	//			r = j * (red / Steps);
	//			g = j * (green / Steps);
	//			b = j * (blue / Steps);
	//
	//			for (i = 0; i < EyeWidth; i++) {
	//				setPixel(StartPoint + i, r, g, b);
	//				setPixel(Start2ndEye + i, r, g, b);
	//			}
	//
	//			showStrip();
	//			delay(FadeDelay);
	//		}
	//	}
	//
	//	setAll(0, 0, 0); // Set all black
	//
	//	delay(EndPause);
	//}
	//
	//void CylonBounce(byte red, byte green, byte blue, int EyeSize, int SpeedDelay, int ReturnDelay) {
	//
	//	for (int i = 0; i < NUM_LEDS - EyeSize - 2; i++) {
	//		setAll(0, 0, 0);
	//		setPixel(i, red / 10, green / 10, blue / 10);
	//		for (int j = 1; j <= EyeSize; j++) {
	//			setPixel(i + j, red, green, blue);
	//		}
	//		setPixel(i + EyeSize + 1, red / 10, green / 10, blue / 10);
	//		showStrip();
	//		delay(SpeedDelay);
	//	}
	//
	//	delay(ReturnDelay);
	//
	//	for (int i = NUM_LEDS - EyeSize - 2; i > 0; i--) {
	//		setAll(0, 0, 0);
	//		setPixel(i, red / 10, green / 10, blue / 10);
	//		for (int j = 1; j <= EyeSize; j++) {
	//			setPixel(i + j, red, green, blue);
	//		}
	//		setPixel(i + EyeSize + 1, red / 10, green / 10, blue / 10);
	//		showStrip();
	//		delay(SpeedDelay);
	//	}
	//
	//	delay(ReturnDelay);
	//}
	//
	//void NewKITT(byte red, byte green, byte blue, int EyeSize, int SpeedDelay, int ReturnDelay) {
	//	RightToLeft(red, green, blue, EyeSize, SpeedDelay, ReturnDelay);
	//	LeftToRight(red, green, blue, EyeSize, SpeedDelay, ReturnDelay);
	//	OutsideToCenter(red, green, blue, EyeSize, SpeedDelay, ReturnDelay);
	//	CenterToOutside(red, green, blue, EyeSize, SpeedDelay, ReturnDelay);
	//	LeftToRight(red, green, blue, EyeSize, SpeedDelay, ReturnDelay);
	//	RightToLeft(red, green, blue, EyeSize, SpeedDelay, ReturnDelay);
	//	OutsideToCenter(red, green, blue, EyeSize, SpeedDelay, ReturnDelay);
	//	CenterToOutside(red, green, blue, EyeSize, SpeedDelay, ReturnDelay);
	//}
	//
	//// used by NewKITT
	//void CenterToOutside(byte red, byte green, byte blue, int EyeSize, int SpeedDelay, int ReturnDelay) {
	//	for (int i = ((NUM_LEDS - EyeSize) / 2); i >= 0; i--) {
	//		setAll(0, 0, 0);
	//
	//		setPixel(i, red / 10, green / 10, blue / 10);
	//		for (int j = 1; j <= EyeSize; j++) {
	//			setPixel(i + j, red, green, blue);
	//		}
	//		setPixel(i + EyeSize + 1, red / 10, green / 10, blue / 10);
	//
	//		setPixel(NUM_LEDS - i, red / 10, green / 10, blue / 10);
	//		for (int j = 1; j <= EyeSize; j++) {
	//			setPixel(NUM_LEDS - i - j, red, green, blue);
	//		}
	//		setPixel(NUM_LEDS - i - EyeSize - 1, red / 10, green / 10, blue / 10);
	//
	//		showStrip();
	//		delay(SpeedDelay);
	//	}
	//	delay(ReturnDelay);
	//}
	//
	//// used by NewKITT
	//void OutsideToCenter(byte red, byte green, byte blue, int EyeSize, int SpeedDelay, int ReturnDelay) {
	//	for (int i = 0; i <= ((NUM_LEDS - EyeSize) / 2); i++) {
	//		setAll(0, 0, 0);
	//
	//		setPixel(i, red / 10, green / 10, blue / 10);
	//		for (int j = 1; j <= EyeSize; j++) {
	//			setPixel(i + j, red, green, blue);
	//		}
	//		setPixel(i + EyeSize + 1, red / 10, green / 10, blue / 10);
	//
	//		setPixel(NUM_LEDS - i, red / 10, green / 10, blue / 10);
	//		for (int j = 1; j <= EyeSize; j++) {
	//			setPixel(NUM_LEDS - i - j, red, green, blue);
	//		}
	//		setPixel(NUM_LEDS - i - EyeSize - 1, red / 10, green / 10, blue / 10);
	//
	//		showStrip();
	//		delay(SpeedDelay);
	//	}
	//	delay(ReturnDelay);
	//}
	//
	//// used by NewKITT
	//void LeftToRight(byte red, byte green, byte blue, int EyeSize, int SpeedDelay, int ReturnDelay) {
	//	for (int i = 0; i < NUM_LEDS - EyeSize - 2; i++) {
	//		setAll(0, 0, 0);
	//		setPixel(i, red / 10, green / 10, blue / 10);
	//		for (int j = 1; j <= EyeSize; j++) {
	//			setPixel(i + j, red, green, blue);
	//		}
	//		setPixel(i + EyeSize + 1, red / 10, green / 10, blue / 10);
	//		showStrip();
	//		delay(SpeedDelay);
	//	}
	//	delay(ReturnDelay);
	//}
	//
	//// used by NewKITT
	//void RightToLeft(byte red, byte green, byte blue, int EyeSize, int SpeedDelay, int ReturnDelay) {
	//	for (int i = NUM_LEDS - EyeSize - 2; i > 0; i--) {
	//		setAll(0, 0, 0);
	//		setPixel(i, red / 10, green / 10, blue / 10);
	//		for (int j = 1; j <= EyeSize; j++) {
	//			setPixel(i + j, red, green, blue);
	//		}
	//		setPixel(i + EyeSize + 1, red / 10, green / 10, blue / 10);
	//		showStrip();
	//		delay(SpeedDelay);
	//	}
	//	delay(ReturnDelay);
	//}
	//
	//void Twinkle(byte red, byte green, byte blue, int Count, int SpeedDelay, boolean OnlyOne) {
	//	setAll(0, 0, 0);
	//
	//	for (int i = 0; i < Count; i++) {
	//		setPixel(random(NUM_LEDS), red, green, blue);
	//		showStrip();
	//		delay(SpeedDelay);
	//		if (OnlyOne) {
	//			setAll(0, 0, 0);
	//		}
	//	}
	//
	//	delay(SpeedDelay);
	//}
	//
	//void TwinkleRandom(int Count, int SpeedDelay, boolean OnlyOne) {
	//	setAll(0, 0, 0);
	//
	//	for (int i = 0; i < Count; i++) {
	//		setPixel(random(NUM_LEDS), random(0, 255), random(0, 255), random(0, 255));
	//		showStrip();
	//		delay(SpeedDelay);
	//		if (OnlyOne) {
	//			setAll(0, 0, 0);
	//		}
	//	}
	//
	//	delay(SpeedDelay);
	//}
	//
	//void Sparkle(byte red, byte green, byte blue, int SpeedDelay) {
	//	int Pixel = random(NUM_LEDS);
	//	setPixel(Pixel, red, green, blue);
	//	showStrip();
	//	delay(SpeedDelay);
	//	setPixel(Pixel, 0, 0, 0);
	//}
	//
	//void SnowSparkle(byte red, byte green, byte blue, int SparkleDelay, int SpeedDelay) {
	//	setAll(red, green, blue);
	//
	//	int Pixel = random(NUM_LEDS);
	//	setPixel(Pixel, 0xff, 0xff, 0xff);
	//	showStrip();
	//	delay(SparkleDelay);
	//	setPixel(Pixel, red, green, blue);
	//	showStrip();
	//	delay(SpeedDelay);
	//}
	//
	//void RunningLights(byte red, byte green, byte blue, int WaveDelay) {
	//	int Position = 0;
	//
	//	for (int i = 0; i < NUM_LEDS * 2; i++)
	//	{
	//		Position++; // = 0; //Position + Rate;
	//		for (int i = 0; i < NUM_LEDS; i++) {
	//			// sine wave, 3 offset waves make a rainbow!
	//			//float level = sin(i+Position) * 127 + 128;
	//			//setPixel(i,level,0,0);
	//			//float level = sin(i+Position) * 127 + 128;
	//			setPixel(i, ((sin(i + Position) * 127 + 128)*red,
	//				((sin(i + Position) * 127 + 128)*green,
	//				((sin(i + Position) * 127 + 128)*blue);
	//		}
	//
	//		showStrip();
	//		delay(WaveDelay);
	//	}
	//}
	//
	//void colorWipe(byte red, byte green, byte blue, int SpeedDelay) {
	//	for (uint16_t i = 0; i < NUM_LEDS; i++) {
	//		setPixel(i, red, green, blue);
	//		showStrip();
	//		delay(SpeedDelay);
	//	}
	//}
	//
	//void rainbowCycle(int SpeedDelay) {
	//	byte *c;
	//	uint16_t i, j;
	//
	//	for (j = 0; j < 256 * 5; j++) { // 5 cycles of all colors on wheel
	//		for (i = 0; i < NUM_LEDS; i++) {
	//			c = Wheel(((i * 256 / NUM_LEDS) + j) & 255);
	//			setPixel(i, *c, *(c + 1), *(c + 2));
	//		}
	//		showStrip();
	//		delay(SpeedDelay);
	//	}
	//}

#pragma endregion PATTERNS

			float mapFloat(long x, long inMin, long inMax, long outMin, long outMax)
			{
				return (float)(x - inMin) * (outMax - outMin) / (float)(inMax - inMin) + outMin;
			}
	};

	extern CMeltdownLED MeltdownLED;

#endif
}