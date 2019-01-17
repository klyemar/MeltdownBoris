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

		//void RunningLights(byte red, byte green, byte blue, int WaveDelay) {
		//	int Position = 0;

		//	for (int j = 0; j < NUM_LEDS * 2; j++)
		//	{
		//		Position++; // = 0; //Position + Rate;
		//		for (int i = 0; i < NUM_LEDS; i++) {
		//			// sine wave, 3 offset waves make a rainbow!
		//			//float level = sin(i+Position) * 127 + 128;
		//			//setPixel(i,level,0,0);
		//			//float level = sin(i+Position) * 127 + 128;
		//			setPixel(i, ((sin(i + Position) * 127 + 128) / 255)*red,
		//				((sin(i + Position) * 127 + 128) / 255)*green,
		//				((sin(i + Position) * 127 + 128) / 255)*blue);
		//		}

		//		showStrip();
		//		delay(WaveDelay);
		//	}


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
//			setPixel(i, ((sin(i + Position) * 127 + 128) / 255)*red,
//				((sin(i + Position) * 127 + 128) / 255)*green,
//				((sin(i + Position) * 127 + 128) / 255)*blue);
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