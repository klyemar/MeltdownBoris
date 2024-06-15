#include <FastLED.h>
#include <MeltdownSerial.h>
#include <math.h>

namespace Meltdown
{

#ifndef MOBILE_MELTDOWN
#define MOBILE_MELTDOWN

#define ARRAY_SIZE(A) (sizeof(A) / sizeof((A)[0]))

	class CMobileMeltdown;

	struct MeltdownPattern
	{
		// The pattern function.
		typedef void(Meltdown::CMobileMeltdown::* PatternFunc) (CRGB[]);

		int numModes;
		PatternFunc patternFunc;
	};

	struct PaletteData
	{
		TProgmemRGBPalette16 palette;
		TBlendType blendType;
	};

	class CMobileMeltdown
	{
	public: CMobileMeltdown();

		  // Global LED values.
		  int gNumLeds = 0;

		  int gCurrentPatternNumber = 0;
		  int gCurrentEffectNumber = 0;
		  int gCurrentModeNumber = 0;

		  // Palette values, to be honest this is kind of hacky for now... I dunno... TODO!!!
		  int gLastRandomPaletteNumber = 0;
		  int gRandomPaletteNumber = 0;

		  int gBrightness = 255;
		  int gHue = 0;
		  int gDelay = 0;
		  long gAnalogPattern = 0;
		  long gAnalogEffect = 0;
		  long gPos = 0;
		  long gFade = 150;
		  long gFrame = 1;
		  int gFrameStep = 1;
		  bool gInverse = false;
		  bool gGlitter = false;
		  bool gAutoModeActive = false;

		  enum AutoMode { None, Sleep, Pattern, Mode, PatternMode };
		  AutoMode gAutoMode = PatternMode;

		  unsigned long gCurrentMillis;
		  // The millis at the time that the auto mode timer started.
		  unsigned long gAutoModeStartMillis;
		  // The number of minutes after which auto mode should begin.
		  const unsigned long gAutoModePeriod = 1000L * 60 * 30; // 30 minutes

		// List of patterns to cycle through.  Each is defined as a separate function below.
		  MeltdownPattern gPatterns[9] = {
			  //{ 10, &CMobileMeltdown::SolidColors },
			  //{ 3, &CMobileMeltdown::RainbowFull },

			  { 4, &CMobileMeltdown::Sinelon },
			  { 3, &CMobileMeltdown::Bpm },
			  { 2, &CMobileMeltdown::MeteorRain },
			  { 3, &CMobileMeltdown::Orbital },
			  { 2, &CMobileMeltdown::Rainbow },
			  { 4, &CMobileMeltdown::RunningLights },
			  { 3, &CMobileMeltdown::Juggle },
			  { 3, &CMobileMeltdown::Confetti },
			  { 4, &CMobileMeltdown::BlendColor },
		  };

		  // List of effects to cycle through.  Each is defined as a separate function below.
		  typedef void (CMobileMeltdown::* EffectList)(CRGB[]);
		  EffectList gEffects[2] = {
			  &CMobileMeltdown::NullEffect,
			  &CMobileMeltdown::GlitterEffect
		  };

		  // CONFIGURATION //

		  void SetNumLeds(int numLeds)
		  {
			  gNumLeds = numLeds;
		  }

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

		  // FRAME //

		  void ResetFrame() { gFrame = 1; }

		  void IncrementFrame()
		  {
			  gFrame++;
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

#pragma region EFFECTS

		  int GetEffectNumber()
		  {
			  return gCurrentEffectNumber % ARRAY_SIZE(gEffects);
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

		  void ExecuteEffect(CRGB leds[]) { (this->*(gEffects[GetEffectNumber()]))(leds); }

		  void NullEffect(CRGB leds[]) { }

		  void GlitterEffect(CRGB leds[])
		  {
			  int chanceOfGlitter = GetAnalogEffect(25, 500);
			  if (random8() < chanceOfGlitter)
			  {
				  SetRandomColor(leds, 3, CRGB::White);
			  }
		  }

		  void HueIncrementEffect(CRGB leds[])
		  {
			  static int hue = 0;

			  int effectVal = GetAnalogEffect(100, 500);
			  EVERY_N_MILLIS(effectVal)
			  {
				  for (int i = 0; i < gNumLeds; i++)
				  {
					  // leds[i]
				  }
			  }
		  }

#pragma endregion EFFECTS

#pragma region MODES

		  int GetNumModes()
		  {
			  MeltdownPattern pattern = GetPattern();

			  return pattern.numModes;
		  }

		  int GetModeNumber()
		  {
			  return (gCurrentModeNumber) % (GetNumModes() + 1);
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

		  void Invert(CRGB leds[])
		  {
			  for (int i = 0; i < gNumLeds; i++)
			  {
				  leds[i] = -leds[i];
			  }
		  }

		  void MaximizeBrightness(CRGB leds[])
		  {
			  for (int i = 0; i < gNumLeds; i++)
			  {
				  (leds[i]).maximizeBrightness();
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

		  void SetAllColor(CRGB leds[], CRGB::HTMLColorCode color)
		  {
			  for (int i = 0; i < gNumLeds; i++)
			  {
				  leds[i] = color;
			  }
		  }

		  void SetAllColor(CRGB leds[], CRGB color)
		  {
			  for (int i = 0; i < gNumLeds; i++)
			  {
				  leds[i] = color;
			  }
		  }

		  void SetAllColor(CRGB leds[], int hue)
		  {
			  for (int i = 0; i < gNumLeds; i++)
			  {
				  leds[i] = CHSV(hue, 255, 255);
			  }
		  }

		  void FillGradientRgb(CRGB leds[], uint16_t startpos, CRGB startcolor, uint16_t endpos, CRGB endcolor)
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
				  leds[i] = CRGB(r88 >> 8, g88 >> 8, b88 >> 8);
				  r88 += rdelta87;
				  g88 += gdelta87;
				  b88 += bdelta87;
			  }
		  }

		  void BlendAll(CRGB leds[], CRGB color, byte amount = 127)
		  {
			  for (int i = 0; i < gNumLeds; i++)
			  {
				  CRGB led = leds[i];
				  if (led)
				  {
					  nblend(leds[i], color, amount);
				  }
			  }
		  }

		  /// Generate random colors (withing a confinement of hue) in random locations.
		  void SetRandomColor(CRGB leds[], CRGB color, int numPositions, float frequency = .25)
		  {
			  for (int i = 0; i < numPositions; i++)
			  {
				  uint8_t random = random8(1, 255);
				  if (random < 255 * frequency)
				  {
					  //leds[random16(gNumLeds)] = CHSV(gHue + random8(hueOffsetMin, hueOffsetMax), 200, 255);
					  leds[random16(gNumLeds)] = CRGB(color);
				  }
			  }
		  }

		  /// Add a specific color in random locations.
		  void SetRandomColor(CRGB leds[], int numPositions, CRGB::HTMLColorCode color, float frequency = .05)
		  {
			  for (int i = 0; i < numPositions; i++)
			  {
				  uint8_t random = random8(1, 255);
				  if (random < 255 * frequency)
				  {
					  leds[random16(gNumLeds)] = color;
				  }
			  }
		  }

		  void GenerateSinelons(CRGB leds[], int numSinelons, int pos)
		  {
			  for (int i = 0; i < numSinelons; i++)
			  {
				  int index = (pos + (int)(gNumLeds / numSinelons) * (i + 1)) % gNumLeds;
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

		  PaletteData GetPalette(int8_t index)
		  {
			  /*CRGBPalette16 palettes[5] = {
				  PartyColors_p,
				  ForestColors_p,
				  CloudColors_p,
				  HeatColors_p,
				  RainbowColors_p
			  };*/

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

		  void FadeSetsToBlackBy(CRGB leds[], int fade)
		  {
			  for (int i = 0; i < gNumLeds; i++)
			  {
				  int scale = 255 - fade;
				  leds[i].nscale8(scale);
			  }
		  }

		  void FillRainbow(CRGB leds[], int initialHue, int deltaHue)
		  {
			  CHSV hsv;
			  hsv.hue = initialHue;
			  hsv.val = 255;
			  hsv.sat = 240;
			  for (int i = 0; i < gNumLeds; i++)
			  {
				  leds[i] = hsv;
				  hsv.hue += deltaHue;
			  }
		  }

#pragma endregion UTILITY

#pragma region PATTERNS

		  MeltdownPattern GetPattern()
		  {
			  int patternNumber = GetPatternNumber();

			  return gPatterns[patternNumber];
		  }

		  int GetPatternNumber()
		  {
			  return gCurrentPatternNumber % ARRAY_SIZE(gPatterns);
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

		  void ExecutePattern(CRGB leds[])
		  {
			  MeltdownPattern pattern = gPatterns[GetPatternNumber()];

			  (this->*(pattern.patternFunc))(leds);
		  }

		  void BlendColor(CRGB leds[])
		  {
			  int speed = beatsin8(GetAnalogPattern(16, 48), 0, 255);

			  // Modes
			  FillGradients(leds, GetModeNumber() + 1, speed);
		  }

		  void FillGradients(CRGB leds[], int numGradients, int speed)
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

				  int startPos = (gNumLeds / numGradients) * i;
				  if (startPos < 0) startPos = 0;
				  int endPos = ((gNumLeds / numGradients) * (i + 1)) - 1;
				  if (endPos < 1) endPos = 1;

				  FillGradientRgb(leds, startPos, blend1, endPos, blend2);
			  }
		  }

		  void HalloweenBlendColor(CRGB leds[])
		  {
			  int speed = beatsin8(48, 128, 255);

			  // Modes
			  HalloweenFillGradients(leds, GetModeNumber() + 3, speed);
		  }

		  void HalloweenFillGradients(CRGB leds[], int numGradients, int speed)
		  {
			  if (numGradients < 4) numGradients = 4;

			  for (int i = 0; i < numGradients; i++)
			  {
				  CRGB color1 = CRGB(117, 0, 255);
				  CRGB color2 = CRGB(90, 0, 138);
				  CRGB color3 = CRGB(30, 0, 90);

				  // Blend between two different colors over time.
				  CRGB blend1 = blend(color1, color2, speed);
				  CRGB blend2 = blend(color2, color3, speed);

				  int startPos = (gNumLeds / numGradients) * i;
				  if (startPos < 0) startPos = 0;
				  int endPos = ((gNumLeds / numGradients) * (i + 1)) - 1;
				  if (endPos < 1) endPos = 1;

				  FillGradientRgb(leds, startPos, blend1, endPos, blend2);
			  }
		  }

		  void SolidColors(CRGB leds[])
		  {
			  // Modes
			  switch (GetModeNumber())
			  {
			  case 1:
				  SetAllColor(leds, CRGB::OrangeRed);
				  break;
			  case 2:
				  SetAllColor(leds, CRGB::Orange);
				  break;
			  case 3:
				  SetAllColor(leds, CRGB::Yellow);
				  break;
			  case 4:
				  SetAllColor(leds, CRGB::GreenYellow);
				  break;
			  case 5:
				  SetAllColor(leds, CRGB::Green);
				  break;
			  case 6:
				  SetAllColor(leds, CRGB::Teal);
				  break;
			  case 7:
				  SetAllColor(leds, CRGB::Blue);
				  break;
			  case 8:
				  SetAllColor(leds, CRGB::Indigo);
				  break;
			  case 9:
				  SetAllColor(leds, CRGB::Violet);
				  break;
			  case 10:
				  SetAllColor(leds, CRGB::Magenta);
				  break;
			  default:
				  SetAllColor(leds, CRGB::Red);
			  }
		  }

		  void Rainbow(CRGB leds[])
		  {
			  static int hue = 1;

			  // Modes
			  switch (GetModeNumber())
			  {
			  case 0:
				  // Increment the hue.
				  hue++;
				  FillRainbow(leds, gHue + gPos + hue, 127 / gNumLeds);
				  break;
			  case 1:
				  hue++;
				  FillRainbow(leds, gHue + gPos + hue, 255 / gNumLeds);
				  break;
			  case 2:
				  hue++;
				  FillRainbow(leds, gHue + gPos + hue, 511 / gNumLeds);
				  break;
			  }
		  }

		  void RainbowFull(CRGB leds[])
		  {
			  static int hue = 1;

			  // Modes
			  switch (GetModeNumber())
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

			  SetAllColor(leds, hue);
		  }

		  void Confetti(CRGB leds[])
		  {
			  FadeSetsToBlackBy(leds, 15);

			  // Get some slight variation in the selected colors.
			  int numColors = random8(4, 8);
			  // Get a random palette to select our colors.
			  PaletteData paletteData = GetRandomPalette();

			  for (int i = 0; i < numColors; i++)
			  {
				  SetRandomColor(leds, ColorGradientFromPalette(paletteData.palette, numColors, i, random8(64, 195)), 1, .3);
			  }
		  }

		  void Orbital(CRGB leds[])
		  {
			  static int counter = 0;

			  // The speed to shift the gradient.
			  float gradientMultiplier = 1.5;
			  // The speed to shift the position.
			  float posMultiplier = .4;
			  // The pixel values.
			  int space = 40;
			  int width = 8;
			  // Fade value.
			  int fade = 10;

			  FadeSetsToBlackBy(leds, fade);

			  for (int i = 0; i < gNumLeds; i++)
			  {
				  // Effectively, make a solid block that is 'width' pixels wide with a 'space' pixel wide gap in between.
				  if ((i + GetFrameOffset(posMultiplier)) % (space + width) < width)
				  {
					  leds[i] = ColorGradientFromPalette(GetRandomPalette().palette, gNumLeds, i + GetFrameOffset(gradientMultiplier));
				  }
			  }

			  counter++;
		  }

		  //void HalloweenConfetti(CRGB leds[])
		  //{
			 // int fade = GetAnalogPattern(2, 30);
			 // FadeSetsToBlackBy(leds, fade);

			 // // Modes
			 // switch (GetModeNumber())
			 // {
			 // case 1:
				//  SetRandomColor(leds, 6, 38, .8);	// Orange
				//  break;
			 // case 2:
				//  SetRandomColor(leds, 3, 86, .8);	// Green	
				//  break;
			 // default:
				//  SetRandomColor(leds, 5, 230, .8);	// Purple
				//  break;
			 // }
		  //}

		  void Sinelon(CRGB leds[])
		  {
			  // Display a dot moving back and forth in a sin wave pattern.
			  FadeSetsToBlackBy(leds, 60);

			  uint16_t index = beatsin16(8, 0, gNumLeds - 1);

			  GenerateSinelons(leds, GetModeNumber() + 2, index);
		  }

		  void Bpm(CRGB leds[])
		  {
			  int bpm = 60;
			  int beat = beatsin8(bpm, 0, 255);
			  PaletteData paletteData = GetRandomPalette();

			  for (int i = 0; i < gNumLeds; i++)
			  {
				  leds[i] = ColorGradientFromPalette(paletteData.palette, gNumLeds, i + GetFrameOffset(.35), beat - (i * 10), paletteData.blendType);
			  }
		  }

		  void Juggle(CRGB leds[])
		  {
			  // Eight colored dots, weaving in and out of sync with each other.
			  FadeSetsToBlackBy(leds, 48);

			  int numBalls = 3;

			  // Modes
			  switch (GetModeNumber())
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
				  leds[beatsin16(i + 5, 0, gNumLeds - 1)] = GetRainbowColor(i + 4);
			  }
		  }

		  void Sunrise(CRGB leds[])
		  {
			  static const uint8_t sunriseMinutes = 8;

			  static uint8_t heatIndex = 0;
			  CRGB color = ColorFromPalette(HeatColors_p, (heatIndex % 255));

			  SetAllColor(leds, color);

			  static const uint8_t interval = (sunriseMinutes * 60) / 256;
			  EVERY_N_SECONDS(interval) {
				  heatIndex++;
			  }
		  }

		  void MeteorRain(CRGB leds[])
		  {
			  const boolean meteorRandomDecay = true;

			  // Medium meteor.
			  /*int meteorSize = 25;
			  gDelay = 15;*/
			  // Large meteor.
			  int meteorSize = 250;
			  gDelay = 0;

			  //// Modes
			  //switch (GetModeNumber())
			  //{
				 // // Small meteors.
				 // case 1:
				 // {
					//  meteorSize = 5;
					//  gDelay = 7;
					//  break;
				 // }
				 // // Large meteor.
				 // case 2:
				 // {
					//  meteorSize = 50;
					//  gDelay = 20;
					//  break;
				 // }
			  //}

			  // Fade brightness all LEDs one step.
			  for (int i = 0; i < gNumLeds; i++)
			  {
				  if (!meteorRandomDecay || random(10) > 5)
				  {
					  int meteorTrailDecay = GetAnalogPattern(32, 96);

					  (leds[i]).fadeToBlackBy(meteorTrailDecay);
				  }
			  }

			  // Draw meteor.
			  long frame = GetFrame(gNumLeds);
			  for (int i = 0; i < meteorSize; i++)
			  {
				  if ((frame - i < gNumLeds) && (frame - i >= 0))
				  {
					  // Set the colors of the meteor using a reversed HeatColors gradient, so the meteor's tail is white hot trailing off to red.
					  leds[frame - i] = ColorGradientFromPalette(GetRandomPalette().palette, meteorSize, i, 255, LINEARBLEND, true);
				  }
			  }
		  }

		  void Checkers(CRGB leds[])
		  {
			  //FadeSetsToBlackBy(leds, 50);

			  int checkerSize = 4;

			  for (int i = 0; i < gNumLeds; i++)
			  {
				  int checker = i / 4;

				  if ((checker % 2 != 0 && sin8(gFrame) < 128) || (checker % 2 == 0 && sin8(gFrame) >= 128))
				  {
					  leds[i] = CRGB::Red;
				  }
				  else
				  {
					  leds[i] = CRGB::Black;
				  }
			  }
		  }

		  void RunningLights(CRGB leds[])
		  {
			  int length = 4;

			  for (int i = 0; i < gNumLeds; i++)
			  {
				  double redSin;
				  double greenSin;
				  double blueSin;

				  // Modes
				  switch (GetModeNumber())
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

				  float speedMultiplier = 3.5;
				  byte redMult = sin8(redSin * speedMultiplier);
				  byte greenMult = sin8(greenSin * speedMultiplier);
				  byte blueMult = sin8(blueSin * speedMultiplier);

				  leds[i] = CRGB(redMult, greenMult, blueMult);
			  }
		  }

		  const TProgmemRGBPalette16 Rainbow_p FL_PROGMEM =
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

		  const TProgmemRGBPalette16 Cool_p FL_PROGMEM =
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

		  const TProgmemRGBPalette16 Warm_p FL_PROGMEM =
		  {
			  0x08CFE2,	// Cyan
			  0x37E600,	// Green
			  0x37E600,	// Green
			  0x37E600,	// Green
			  0x37E600,	// Green
			  0xFFFF00,	// Yellow
			  0xFFFF00,	// Yellow
			  0xFF6200,	// Orange
			  0xFF6200,	// Orange
			  0xFF6200,	// Orange
			  0xFF6200,	// Orange
			  0xFFFF00,	// Yellow
			  0xFFFF00,	// Yellow
			  0x37E600,	// Green
		  };

		  const TProgmemRGBPalette16 Cyberpunk_p FL_PROGMEM =
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

		  const TProgmemRGBPalette16 Contrast_p FL_PROGMEM =
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

		  const TProgmemRGBPalette16 Neon_p FL_PROGMEM =
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
			  0x37E600,	// Green
			  0x37E600,	// Green
			  0x37E600,	// Green
		  };

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

	extern CMobileMeltdown MobileMeltdown;

#endif
}