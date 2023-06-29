#include "FastLED.h"
#include "MeltdownLogger.h"
#include "MeltdownSerial.h"
#include <math.h>

namespace Meltdown
{

#ifndef MELTDOWN_LED
#define MELTDOWN_LED

#define ARRAY_SIZE(A) (sizeof(A) / sizeof((A)[0]))

	class CMeltdownLED;

	struct MeltdownPattern
	{
		// The pattern function.
		typedef void(Meltdown::CMeltdownLED::* PatternFunc) (CRGB[], uint16_t[], int, int);

		int numModes;
		PatternFunc patternFunc;
	};

	struct PaletteData
	{
		CRGBPalette16 palette;
		TBlendType blendType;
	};

	class CMeltdownLED
	{
	public: CMeltdownLED();

		  // Global LED values.
		  int gCurrentPatternNumber = 0;
		  int gCurrentEffectNumber = 0;
		  int gCurrentModeNumber = 0;

		  // Palette values, to be honest this is kind of hacky for now... I dunno... TODO!!!
		  int gLastRandomPaletteNumber = 0;
		  int gRandomPaletteNumber = 0;

		  int gBrightness = 48;
		  int gDelay = 0;
		  long gAnalogPattern = 0;
		  long gAnalogEffect = 0;
		  long gPos = 0;
		  long gFade = 20;
		  long gFrame = 1;
		  int gFrameStep = 1;
		  bool gFullPurple = false;
		  bool gFullYellow = false;
		  bool gFullRainbow = false;
		  bool gFullGreen = false;
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
		  MeltdownPattern gPatterns[10] = {

			  { 3, &CMeltdownLED::Rainbow },
			  { 4, &CMeltdownLED::Sinelon },
			  { 3, &CMeltdownLED::Bpm },
			  { 3, &CMeltdownLED::Orbital },
			  { 3, &CMeltdownLED::Juggle },
			  { 3, &CMeltdownLED::Confetti },
			  { 4, &CMeltdownLED::BlendColor },
			  { 2, &CMeltdownLED::MeteorRain },
			  { 4, &CMeltdownLED::RunningLights },
			  { 10, &CMeltdownLED::SolidColors },
		  }; 

		  // List of effects to cycle through.  Each is defined as a separate function below.
		  typedef void (CMeltdownLED::* EffectList)(CRGB[], uint16_t[], int);
		  EffectList gEffects[4] = {
			  &CMeltdownLED::NullEffect,
			  &CMeltdownLED::WhiteGlitterEffect,
			  &CMeltdownLED::GoldGlitterEffect,
			  &CMeltdownLED::PinkGlitterEffect,
			  //&CMeltdownLED::RainbowGlitterEffect,
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

		  // FULL COLORS //

		  bool EnableFullPurple()
		  {
			  gFullPurple = true;
			  return gFullPurple;
		  }
		  bool DisableFullPurple()
		  {
			  gFullPurple = false;
			  return gFullPurple;
		  }
		  void SetFullPurple() { gFullPurple = MeltdownSerial.GetBoolValue(); }

		  bool GetFullPurple() { return gFullPurple; }

		  bool EnableFullYellow()
		  {
			  gFullYellow = true;
			  return gFullYellow;
		  }
		  bool DisableFullYellow()
		  {
			  gFullYellow = false;
			  return gFullYellow;
		  }
		  void SetFullYellow() { gFullYellow = MeltdownSerial.GetBoolValue(); }

		  bool GetFullYellow() { return gFullYellow; }

		  bool EnableFullRainbow()
		  {
			  gFullRainbow = true;
			  return gFullRainbow;
		  }
		  bool DisableFullRainbow()
		  {
			  gFullRainbow = false;
			  return gFullRainbow;
		  }
		  void SetFullRainbow() { gFullRainbow = MeltdownSerial.GetBoolValue(); }

		  bool GetFullRainbow() { return gFullRainbow; }

		  bool EnableFullGreen()
		  {
			  gFullGreen = true;
			  return gFullGreen;
		  }
		  bool DisableFullGreen()
		  {
			  gFullGreen = false;
			  return gFullGreen;
		  }
		  void SetFullGreen() { gFullGreen = MeltdownSerial.GetBoolValue(); }

		  bool GetFullGreen() { return gFullGreen; }

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

		  int GetFrameOffset(float multiplier) { return GetFrame() * multiplier; }

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

		  void WhiteGlitterEffect(CRGB leds[], uint16_t indexes[], int numLeds)
		  {
			  GlitterEffect(leds, indexes, numLeds, CRGB::White);
		  }

		  void GoldGlitterEffect(CRGB leds[], uint16_t indexes[], int numLeds)
		  {
			  GlitterEffect(leds, indexes, numLeds, CRGB::Gold);
		  }

		  void PinkGlitterEffect(CRGB leds[], uint16_t indexes[], int numLeds)
		  {
			  GlitterEffect(leds, indexes, numLeds, CRGB::Magenta);
		  }

		  void RainbowGlitterEffect(CRGB leds[], uint16_t indexes[], int numLeds)
		  {
			  GlitterEffect(leds, indexes, numLeds, CRGB::Green);
		  }

		  void GlitterEffect(CRGB leds[], uint16_t indexes[], int numLeds, CRGB::HTMLColorCode color)
		  {
			  int chanceOfGlitter = GetAnalogEffect(25, 500);
			  if (random8() < chanceOfGlitter)
			  {
				  SetRandomColor(leds, indexes, numLeds, 3, color, .4f);
			  }
		  }

#pragma endregion EFFECTS

#pragma region MODES

		  int GetNumModes() 
		  { 
			  MeltdownPattern pattern = GetPattern(); 

			  return pattern.numModes;
		  }

		  int GetModeNumber() { return gCurrentModeNumber; }

		  int GetModeNumber(int offset)
		  {
			  // A value less than zero indicates we want the base mode.
			  if (offset < 0) return 0;

			  return (gCurrentModeNumber + offset) % (GetNumModes() + 1);
		  }

		  int SetModeNumber(int number)
		  {
			  // TODO - I'm trying to randomize the global palette. This is hacky, do something better?
			  while (gLastRandomPaletteNumber == gRandomPaletteNumber)
			  {
				  gRandomPaletteNumber = random8(8);
			  }
			  gLastRandomPaletteNumber = gRandomPaletteNumber;

			  gCurrentModeNumber = number;
			  return gCurrentModeNumber;
		  }

		  int SetModeNumber()
		  {
			  // TODO - I'm trying to randomize the global palette. This is hacky, do something better?
			  while (gLastRandomPaletteNumber == gRandomPaletteNumber)
			  {
				  gRandomPaletteNumber = random8(8);
			  }
			  gLastRandomPaletteNumber = gRandomPaletteNumber;

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

		  /// Generate random colors (withing a confinement of hue) in random locations.
		  void SetRandomColor(CRGB leds[], uint16_t indexes[], int numLeds, CRGB color, int numPositions, float frequency = .25)
		  {
			  for (int i = 0; i < numPositions; i++)
			  {
				  uint8_t random = random8(1, 255);
				  if (random < 255 * frequency)
				  {
					  leds[indexes[random16(numLeds)]] = CRGB(color);
				  }
			  }
		  }

		  /// Add a specific color in random locations.
		  void SetRandomColor(CRGB leds[], uint16_t indexes[], int numLeds, int numPositions, CRGB::HTMLColorCode color, float frequency = .05)
		  {
			  for (int i = 0; i < numPositions; i++)
			  {
				  uint8_t random = random8(1, 255);
				  if (random < 255 * frequency)
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
				  uint16_t index = indexes[(pos + (int)(numLeds / numSinelons) * (i + 1)) % numLeds];
				  // Let's get some contrast up in here!
				  int rainbowOffset = 5;

				  leds[index] = GetRainbowColor(i, rainbowOffset);
			  }
		  }

		  CRGB GetRainbowColor(int index = 0, int offset = 0)
		  {
			  switch ((index + offset) % 6)
			  {
			  case 0:
				  return 0xBE00AE;	// Magenta
				  //return CRGB::Red;
			  case 1:
				  return 0x3E00C3;	// Purple
				  //return CRGB::Orange;
			  case 2:
				  return 0x08CFE2;	// Cyan
				  //return CRGB::Yellow;
			  case 3:
				  return 0x37E600;	// Green
				  //return CRGB::Green;
			  case 4:
				  return 0xFFFF00;	// Yellow
				  //return CRGB::Blue;
			  case 5:
				  return 0xFF6200;	// Orange
			  }
		  }

		  PaletteData GetPalette()
		  {
			  return GetPalette(gCurrentModeNumber);
		  }

		  PaletteData GetPalette(int index)
		  {
			  PaletteData palettes[8] = {
				  { Contrast_p, LINEARBLEND },
				  { Cool_p, LINEARBLEND },
				  { Neon_p, LINEARBLEND },
				  { Warm_p, LINEARBLEND },
				  { Cyberpunk_p, LINEARBLEND },
				  { Rainbow_p, LINEARBLEND },
				  { PartyColors_p, LINEARBLEND },
				  { RainbowColors_p, LINEARBLEND }
			  };

			  return palettes[index % ARRAY_SIZE(palettes)];
		  }

		  PaletteData GetRandomPalette()
		  {
			  return GetPalette(gRandomPaletteNumber);
		  }

		  CRGB ColorGradientFromPalette(CRGBPalette16 palette, int numLeds, int pos, int brightness = 255, TBlendType blendType = LINEARBLEND, bool isReverse = false)
		  {
			  const int numPaletteColors = 255;
			  // Validity check.
			  pos = pos % numLeds;

			  // Get the appropriate color from the palette, mapped to the number of LEDs.
			  int mappedPos = map(pos, 0, numLeds, 0, numPaletteColors);
			  if (isReverse)
			  {
				  return ColorFromPalette(palette, (numPaletteColors - mappedPos), brightness, blendType);
			  }
			  else
			  {
				  return ColorFromPalette(palette, mappedPos, brightness, blendType);
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

		  MeltdownPattern GetPattern(int offset = 0)
		  {
			  int patternNumber = GetPatternNumber(offset);

			  return gPatterns[patternNumber];
		  }

		  int GetPatternNumber(int offset = 0)
		  {
			  // A value less than zero indicates we want the base pattern.
			  if (offset < 0) return 0;

			  return (gCurrentPatternNumber + offset) % ARRAY_SIZE(gPatterns);
		  }

		  int GetNumPatterns()
		  {
			  return ARRAY_SIZE(gPatterns);
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
			  MeltdownPattern pattern = gPatterns[GetPatternNumber(patternOffset)];

			  (this->*(pattern.patternFunc))(leds, indexes, numLeds, modeOffset);
		  }

		  void BlendColor(CRGB leds[], uint16_t indexes[], int numLeds, int modeOffset = 0)
		  {
			  int speed = beatsin8(GetAnalogPattern(16, 48), 0, 255);

			  // Modes
			  FillGradients(leds, indexes, numLeds, GetModeNumber(modeOffset) + 1, speed);
		  }

		  void FillGradients(CRGB leds[], uint16_t indexes[], int numLeds, int numGradients, int speed)
		  {
			  if (numGradients < 1) numGradients = 1;

			  for (int i = 0; i < numGradients; i++)
			  {
				  int offset = 4;
				  CRGB color1 = GetRainbowColor(i + offset);
				  CRGB color2 = GetRainbowColor(i + 1 + offset);
				  CRGB color3 = GetRainbowColor(i + 2 + offset);

				  // Blend between two different colors over time.
				  CRGB blend1 = blend(color1, color2, speed);
				  CRGB blend2 = blend(color2, color3, speed);

				  int startPos = (numLeds / numGradients) * i;
				  if (startPos < 0) startPos = 0;
				  int endPos = ((numLeds / numGradients) * (i + 1)) - 1;
				  if (endPos < 1) endPos = 1;

				  FillGradientRgb(leds, indexes, numLeds, startPos, blend1, endPos, blend2);
			  }
		  }

		  void SolidColors(CRGB leds[], uint16_t indexes[], int numLeds, int modeOffset = 0)
		  {
			  // Modes
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
			  FillRainbow(leds, indexes, numLeds, gPos + hue, deltaHue);

			  // Modes
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

		  //void RainbowFull(CRGB leds[], uint16_t indexes[], int numLeds, int modeOffset = 0)
		  //{
			 // static int hue = 1;

			 // // Modes
			 // switch (GetModeNumber(modeOffset))
			 // {
			 // case 1:
				//  if (GetFrame() % 2 == 0) hue++;
				//  break;
			 // case 2:
				//  hue++;
				//  break;
			 // case 3:
				//  hue += 2;
				//  break;
			 // default:
				//  if (GetFrame() % 3 == 0) hue++;
				//  break;
			 // }

			 // SetAllColor(leds, indexes, numLeds, hue);
		  //}

		  void Confetti(CRGB leds[], uint16_t indexes[], int numLeds, int modeOffset = 0)
		  {
			  int fade = GetAnalogPattern(2, 30);
			  FadeSetsToBlackBy(leds, indexes, numLeds, fade);

			  // Get some slight variation in the selected colors.
			  int numColors = random8(4, 8);
			  // Get a random palette to select our colors.
			  PaletteData paletteData = GetRandomPalette();

			  for (int i = 0; i < numColors; i++)
			  {
				  SetRandomColor(leds, indexes, numLeds, ColorGradientFromPalette(paletteData.palette, numColors, i, random8(64, 195)), 1, .3);
			  }
		  }

		  void Orbital(CRGB leds[], uint16_t indexes[], int numLeds, int modeOffset = 0)
		  {
			  static int counter = 0;

			  // The speed to shift the gradient.
			  float gradientMultiplier = 1.5;
			  // The speed to shift the position.
			  float posMultiplier = .4;
			  // The pixel values.
			  int space = 40;
			  int width = 8;
			  if (!gMirror)
			  {
				  width = 40;
			  }
			  // Fade value.
			  int fade = 36;

			  FadeSetsToBlackBy(leds, indexes, numLeds, fade);

			  for (int i = 0; i < numLeds; i++)
			  {
				  // Effectively, make a solid block that is 'width' pixels wide with a 'space' pixel wide gap in between.
				  if ((i + GetFrameOffset(posMultiplier)) % (space + width) < width)
				  {
					  leds[indexes[i]] = ColorGradientFromPalette(GetRandomPalette().palette, numLeds, i + GetFrameOffset(gradientMultiplier));
				  }
			  }

			  counter++;
		  }

		  void Sinelon(CRGB leds[], uint16_t indexes[], int numLeds, int modeOffset = 0)
		  {
			  // Display a dot moving back and forth in a sin wave pattern.
			  int fade = GetAnalogPattern(2, 30);
			  FadeSetsToBlackBy(leds, indexes, numLeds, fade);

			  uint8_t beats = 8;
			  if (!gMirror)
			  {
				  beats = 4;
			  }

			  uint16_t index = beatsin16(beats, 0, numLeds - 1);

			  GenerateSinelons(leds, indexes, numLeds, GetModeNumber() + 2, index);
		  }

		  void Bpm(CRGB leds[], uint16_t indexes[], int numLeds, int modeOffset = 0)
		  {
			  int bpm = 60;
			  int beat = beatsin8(bpm, 0, 255);
			  PaletteData paletteData = GetRandomPalette();

			  for (int i = 0; i < numLeds; i++)
			  {
				  leds[indexes[i]] = ColorGradientFromPalette(paletteData.palette, numLeds, i + GetFrameOffset(.15), beat - (i * 10), paletteData.blendType);
			  }
		  }

		  void Juggle(CRGB leds[], uint16_t indexes[], int numLeds, int modeOffset = 0)
		  {
			  int fade = GetAnalogPattern(3, 100);
			  // Eight colored dots, weaving in and out of sync with each other.
			  FadeSetsToBlackBy(leds, indexes, numLeds, fade);

			  int numBalls = 3;

			  // Modes
			  switch (GetModeNumber(modeOffset))
			  {
			  case 1:
				  numBalls = 4;
				  break;
			  case 2:
				  numBalls = 5;
				  break;
			  case 3:
				  numBalls = 6;
				  break;
			  }

			  for (int i = 0; i < numBalls; i++)
			  {
				  leds[indexes[beatsin16(i + 5, 0, numLeds - 1)]] = GetRainbowColor(i + 4);
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

			  // If not mirroring, make a longer and faster meteor
			  if (!gMirror)
			  {
				  meteorSize *= 5;
			  }

			  // Ensure meteor size does not exceed num LEDs
			  if (meteorSize > numLeds)
			  {
				  meteorSize = numLeds;
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
					  leds[index] = ColorGradientFromPalette(GetRandomPalette().palette, meteorSize, i, 255, LINEARBLEND, true);
				  }
			  }
		  }

		  void Checkers(CRGB leds[], uint16_t indexes[], int numLeds, int modeOffset = 0)
		  {
			  //FadeSetsToBlackBy(leds, indexes, numLeds, 50);

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

		  const CRGBPalette16 Rainbow_p FL_PROGMEM =
		  {
			  0xBE00AE,	// Magenta
			  0xBE00AE,	// Magenta
			  0xBE00AE,	// Magenta
			  0x3E00C3,	// Purple
			  0x3E00C3,	// Purple
			  0x3E00C3,	// Purple
			  0x08CFE2,	// Cyan
			  0x08CFE2,	// Cyan
			  0x08CFE2,	// Cyan
			  0x37E600,	// Green
			  0x37E600,	// Green
			  0xFFFF00,	// Yellow
			  0xFFFF00,	// Yellow
			  0xFF6200,	// Orange
			  0xFF6200,	// Orange
			  0xFF6200,	// Orange
		  };

		  const CRGBPalette16 Cool_p FL_PROGMEM =
		  {
			  0xBE00AE,	// Magenta
			  0xBE00AE,	// Magenta
			  0xBE00AE,	// Magenta
			  0xBE00AE,	// Magenta
			  0xBE00AE,	// Magenta			  
			  0x3E00C3,	// Purple
			  0x3E00C3,	// Purple
			  0x3E00C3,	// Purple
			  0x3E00C3,	// Purple
			  0x3E00C3,	// Purple
			  0x08CFE2,	// Cyan
			  0x08CFE2,	// Cyan
			  0x08CFE2,	// Cyan
			  0x08CFE2,	// Cyan
			  0x08CFE2,	// Cyan
			  0x08CFE2,	// Cyan
		  };

		  const CRGBPalette16 Warm_p FL_PROGMEM =
		  {
			  0x37E600,	// Green
			  0x37E600,	// Green
			  0x37E600,	// Green
			  0x37E600,	// Green
			  0xFFFF00,	// Yellow
			  0xFFFF00,	// Yellow
			  0xFFFF00,	// Yellow
			  0xFF6200,	// Orange
			  0xFF6200,	// Orange
			  0xFF6200,	// Orange
			  0xFF6200,	// Orange
			  0xFFFF00,	// Yellow
			  0xFFFF00,	// Yellow
			  0xFFFF00,	// Yellow
			  0x37E600,	// Green
			  0x37E600,	// Green
		  };

		  const CRGBPalette16 Cyberpunk_p FL_PROGMEM =
		  {
			  0xBE00AE,	// Magenta
			  0xBE00AE,	// Magenta
			  0xBE00AE,	// Magenta
			  0xBE00AE,	// Magenta
			  0xBE00AE,	// Magenta
			  0x08CFE2,	// Cyan
			  0x08CFE2,	// Cyan
			  0x08CFE2,	// Cyan
			  0x08CFE2,	// Cyan
			  0x08CFE2,	// Cyan
			  0xFFFF00,	// Yellow
			  0xFFFF00,	// Yellow
			  0xFFFF00,	// Yellow
			  0xFFFF00,	// Yellow
			  0xFFFF00,	// Yellow
			  0xFFFF00,	// Yellow
		  };

		  const CRGBPalette16 Contrast_p FL_PROGMEM =
		  {
			  0xFF6200,	// Orange
			  0xFF6200,	// Orange
			  0xFF6200,	// Orange
			  0xFF6200,	// Orange
			  0xFF6200,	// Orange
			  0x08CFE2,	// Cyan
			  0x08CFE2,	// Cyan
			  0x08CFE2,	// Cyan
			  0xBE00AE,	// Magenta
			  0xBE00AE,	// Magenta
			  0xBE00AE,	// Magenta
			  0xBE00AE,	// Magenta
			  0xBE00AE,	// Magenta
			  0x08CFE2,	// Cyan
			  0x08CFE2,	// Cyan
			  0x08CFE2,	// Cyan
		  };

		  const CRGBPalette16 Neon_p FL_PROGMEM =
		  {
			  0x3E00C3,	// Purple
			  0x3E00C3,	// Purple
			  0x3E00C3,	// Purple
			  0x37E600,	// Green
			  0x37E600,	// Green
			  0x37E600,	// Green
			  0xFFFF00,	// Yellow
			  0xFFFF00,	// Yellow
			  0xFFFF00,	// Yellow
			  0x3E00C3,	// Purple
			  0x3E00C3,	// Purple
			  0x3E00C3,	// Purple
			  0xBE00AE,	// Magenta
			  0xBE00AE,	// Magenta
			  0x37E600,	// Green
			  0x37E600,	// Green
		  };

		  const CRGBPalette16 RainbowColors_p FL_PROGMEM =
		  {
			  0xFF0000,
			  0xD52A00,
			  0xAB5500,
			  0xAB7F00,
			  0xABAB00,
			  0x56D500,
			  0x00FF00,
			  0x00D52A,
			  0x00AB55,
			  0x0056AA,
			  0x0000FF,
			  0x2A00D5,
			  0x5500AB,
			  0x7F0081,
			  0xAB0055,
			  0xD5002B
		  };

		  const CRGBPalette16 PartyColors_p FL_PROGMEM =
		  {
			  0x5500AB, 0x84007C, 0xB5004B, 0xE5001B,
			  0xE81700, 0xB84700, 0xAB7700, 0xABAB00,
			  0xAB5500, 0xDD2200, 0xF2000E, 0xC2003E,
			  0x8F0071, 0x5F00A1, 0x2F00D0, 0x0007F9
		  };

#pragma endregion PATTERNS

		  float mapFloat(long x, long inMin, long inMax, long outMin, long outMax)
		  {
			  return (float)(x - inMin) * (outMax - outMin) / (float)(inMax - inMin) + outMin;
		  }
	};

	extern CMeltdownLED MeltdownLED;

#endif
}