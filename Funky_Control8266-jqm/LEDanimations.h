/* This is currently a (working) mess copied together from animation examples over the web
  and I will try to credit everyone and clean it up over time - if you see code here that you wrote
  and you want to be credited, please let me know - I do not wish to cause offense. */

  // declaracion de funciones

void all_off();
void SetupRandomPalette(); //2-color palette
void SetupRandomPalette_g(); //3-color palette
void SetupPurpleAndGreenPalette();
void SetupBlackAndWhiteStripedPalette_a();

//RIPPLE START
int color;
int center = 0;
int step = -1;
int maxSteps = 16;
float fadeRate = 0.8;
int diff;

//background color
uint32_t currentBg = random(256);
uint32_t nextBg = currentBg;

int wrap(int step) {
	if (step < 0) return CUSTOM_NUM_LEDS + step;
	if (step > CUSTOM_NUM_LEDS - 1) return step - CUSTOM_NUM_LEDS;
	return step;
}

//void video(byte nada1 , byte nada2) {
	// matrix gests updated on the heroku websocket callback
	// verify is connected to server
	//if (!isConnectedH )
	//{
		//herokuWs.begin(herokuHost, herokuport);
	//}
	//FastLED.show(); // display this frame
//}
void one_color_allHSV(byte ahue, byte abright) {                // SET ALL LEDS TO ONE COLOR (HSV)
	//ahue = myHue;
	//abright = myBrightness;
	//FastLED.clear();
	//CHSV pp = rgb2hsv_approximate(CRGB(red, green, blue));
	//fill_solid(c_leds[0], CUSTOM_NUM_LEDS /*number of leds*/, CHSV(ahue, mySaturation, abright));
	//fill_solid(c_leds[0], CUSTOM_NUM_LEDS /*number of leds*/,CRGB(realRed,realGreen,realBlue));
	FastLED.showColor(CRGB(realRed, realGreen, realBlue));
	//FastLED.show();
}

void ripple() {
	static unsigned long rippleTick = millis();
	if ((millis() - rippleTick) > 300) //if the amount of milliseconds difference is too large, reset the difference.
	{
		rippleTick = millis() - (100 + 1);

		Serial.print("mi:");
		Serial.print(millis());
		Serial.print(" ");
		Serial.println(rippleTick);
	}
	if ((millis() - rippleTick) > 100)
	{
		rippleTick += 100;

		if (currentBg == nextBg) {
			nextBg = random(256);
		}
		else if (nextBg > currentBg) {
			currentBg++;
		}
		else {
			currentBg--;
		}
		for (uint16_t l = 0; l < CUSTOM_NUM_LEDS; l++) {
			c_leds(l) = CHSV(currentBg, 255, 50);         // strip.setPixelColor(l, Wheel(currentBg, 0.1));
		}

		if (step == -1) {
			center = random(CUSTOM_NUM_LEDS);
			color = random(256);
			step = 0;
		}

		if (step == 0) {
			c_leds(center) = CHSV(color, 255, 255);         // strip.setPixelColor(center, Wheel(color, 1));
			step++;
		}
		else {
			if (step < maxSteps) {
				//Serial.println(pow(fadeRate,step));

				c_leds(wrap(center + step)) = CHSV(color, 255, pow(fadeRate, step) * 255);     //   strip.setPixelColor(wrap(center + step), Wheel(color, pow(fadeRate, step)));
				c_leds(wrap(center - step)) = CHSV(color, 255, pow(fadeRate, step) * 255);     //   strip.setPixelColor(wrap(center - step), Wheel(color, pow(fadeRate, step)));
				if (step > 3) {
					c_leds(wrap(center + step - 3)) = CHSV(color, 255, pow(fadeRate, step - 2) * 255);   //   strip.setPixelColor(wrap(center + step - 3), Wheel(color, pow(fadeRate, step - 2)));
					c_leds(wrap(center - step + 3)) = CHSV(color, 255, pow(fadeRate, step - 2) * 255);   //   strip.setPixelColor(wrap(center - step + 3), Wheel(color, pow(fadeRate, step - 2)));
				}
				step++;
			}
			else {
				step = -1;
			}
		}

		LEDS.show();
	}
}
// RIPPLE END

// Fire2012 Start
#define FRAMES_PER_SECOND 25

bool gReverseDirection = false;

#define COOLING  100

// SPARKING: What chance (out of 255) is there that a new spark will be lit?
// Higher chance = more roaring fire.  Lower chance = more flickery fire.
// Default 120, suggested range 50-200.
#define SPARKING 70

void Fire2012()
{
	// Array of temperature readings at each simulation cell
	static byte heat[CUSTOM_NUM_LEDS];
	static unsigned long fireTick = millis();
	if ((millis() - fireTick) > 300) //if the amount of milliseconds difference is too large, reset the difference.
	{
		fireTick = millis() + (1000 / FRAMES_PER_SECOND + 1);
		Serial.print("mi:");
		Serial.print(millis());
		Serial.print(" ");
		Serial.println(fireTick);
	}
	if ((millis() - fireTick) > (1000 / FRAMES_PER_SECOND))
	{
		fireTick += (1000 / FRAMES_PER_SECOND);

		// Step 1.  Cool down every cell a little
		for (int i = 0; i < CUSTOM_NUM_LEDS; i++) {
			heat[i] = qsub8(heat[i], random8(0, ((COOLING * 10) / CUSTOM_NUM_LEDS) + 2));
		}

		// Step 2.  Heat from each cell drifts 'up' and diffuses a little
		for (int k = CUSTOM_NUM_LEDS - 1; k >= 2; k--) {
			heat[k] = (heat[k - 1] + heat[k - 2] + heat[k - 2]) / 3;
		}

		// Step 3.  Randomly ignite new 'sparks' of heat near the bottom
		if (random8() < SPARKING) {
			int y = random8(7);
			heat[y] = qadd8(heat[y], random8(160, 255));
		}

		// Step 4.  Map from heat cells to LED colors
		for (int j = 0; j < CUSTOM_NUM_LEDS; j++) {
			CRGB color = HeatColor(heat[j]);
			int pixelnumber;
			if (gReverseDirection) {
				pixelnumber = (CUSTOM_NUM_LEDS - 1) - j;
			}
			else {
				pixelnumber = j;
			}
			c_leds(pixelnumber) = color;
		}
		FastLED.show(); // display this frame
	}
}
//Fire2012 End
//CYLON START

void fadeall() {
	for (int i = 0; i < CUSTOM_NUM_LEDS; i++) {
		c_leds(i).nscale8(200);
	}
}

void cylon() {
	static uint8_t hue = 0;
	static int i = 0;
	static int cylon_state = 2;
	static unsigned long cylonTick = millis();
	//Serial.print("x");
	// First slide the led in one direction
	if ((millis() - cylonTick) > 200) //if the amount of milliseconds difference is too large, reset the difference.
	{
		cylonTick = millis() - 30;
		Serial.print("mi:");
		Serial.print(millis());
		Serial.print(" ");
		Serial.println(cylonTick);
	}
	if ((millis() - cylonTick) > 25)
	{
		cylonTick += 25;
		switch (cylon_state) {
		case 0:
			if (i < CUSTOM_NUM_LEDS) {
				i++;
				// Set the i'th led to red
				c_leds(i) = CHSV(hue++, 255, 255);
				// Show the leds2
				FastLED.show();

				fadeall();
				// Wait a little bit before we loop around and do it again
			}
			else
			{
				cylon_state = 1;
			}
			break;
		case 1:
			if (i > 0) {
				// Now go in the other direction.
				// Set the i'th led to red
				c_leds((i--) - 1) = CHSV(hue++, 255, 255);
				// Show the leds
				FastLED.show();
				// now that we've shown the leds, reset the i'th led to black
				// leds[i] = CRGB::Black;
				fadeall();
				// Wait a little bit before we loop around and do it again
			}
			else
			{
				cylon_state = 2;
			}
			break;
		default:
			cylon_state = 0;
			cylonTick = millis();
			i = 0;
			break;
		}
	}
}
//CYLON END

// Fill the x/y array of 8-bit noise values using the inoise8 function.
void fillnoise8a(byte nada, byte nada1) {
	// If we're runing at a low "speed", some 8-bit artifacts become visible
	// from frame-to-frame.  In order to reduce this, we can do some fast data-smoothing.
	// The amount of data smoothing we're doing depends on "speed".
	uint8_t dataSmoothing = 0;
	speed = qadd8(0, myparameter1);

	if (speed < 50) {
		dataSmoothing = 200 - (speed * 4);
	}

	for (int i = 0; i < MAX_DIMENSION; i++) {
		int ioffset = scale * i;
		for (int j = 0; j < MAX_DIMENSION; j++) {
			int joffset = scale * j;

			uint8_t data = inoise8(x + ioffset, y + joffset, z);

			// The range of the inoise8 function is roughly 16-238.
			// These two operations expand those values out to roughly 0..255
			// You can comment them out if you want the raw noise data.
			data = qsub8(data, 16);
			data = qadd8(data, scale8(data, 39));

			if (dataSmoothing) {
				uint8_t olddata = noise[0][i][j];
				uint8_t newdata = scale8(olddata, dataSmoothing) + scale8(data, 256 - dataSmoothing);
				data = newdata;
			}

			noise[0][i][j] = data;
		}
	}

	z += speed;

	// apply slow drift to X and Y, just for visual variation.
	x += speed / 8;
	y -= speed / 16;
}

// end fillnoise8

void mapNoiseToLEDsUsingPalette()
{
	static uint8_t ihue = 0;

	for (int i = 0; i < CUSTOM_WIDTH; i++) {
		for (int j = 0; j < CUSTOM_HEIGHT; j++) {
			// We use the value at the (i,j) coordinate in the noise
			// array for our brightness, and the flipped value from (j,i)
			// for our pixel's index into the color palette.

			uint8_t index = noise[0][j][i];
			uint8_t bri = noise[0][i][j];

			// if this palette is a 'loop', add a slowly-changing base value
			if (colorLoop) {
				index += ihue;
			}

			// brighten up, as the color palette itself often contains the
			// light/dark dynamic range desired
			if (bri > 127) {
				bri = 255;
			}
			else {
				bri = dim8_raw(bri * 2);
			}

			CRGB color = ColorFromPalette(currentPalette, index, bri);
			//c_leds(XY2(i,j)) = color;
			c_leds(i, j) = color;
		}
	}

	ihue += 1;
}
//end map noiseToLeds

// There are several different palettes of colors demonstrated here.
//
// FastLED provides several 'preset' palettes: RainbowColors_p, RainbowStripeColors_p,
// OceanColors_p, CloudColors_p, LavaColors_p, ForestColors_p, and PartyColors_p.
//
// Additionally, you can manually define your own color palettes, or you can write
// code that creates color palettes on the fly.

// 1 = 5 sec per palette
// 2 = 10 sec per palette
// etc
#define HOLD_PALETTES_X_TIMES_AS_LONG 2

void ChangePaletteAndSettingsPeriodically()
{
	uint8_t maxChanges = 10;
	nblendPaletteTowardPalette(currentPalette, targetPalette, maxChanges);
	//uint8_t secondHand = ((millis() / 1000) / HOLD_PALETTES_X_TIMES_AS_LONG) % 60; //not used with webserver
	//static uint8_t lastSecond = 99;                                                 //not used with webserver
	increaseLedMode();

	if (ledMode != 999) {
		switch (ledMode) {
		case  1: all_off(); break;
		case  2: SetupRandomPalette(); speed = 3; scale = 25; colorLoop = 1; break; //2-color palette
		case  3: SetupRandomPalette_g(); speed = 3; scale = 25; colorLoop = 1; break; //3-color palette
		case  4: SetupPurpleAndGreenPalette(); speed = 6; scale = 20; colorLoop = 1; break;
		case  5: SetupBlackAndWhiteStripedPalette_a(); speed = 4; scale = 20; colorLoop = 1; ; break;
		case  6: targetPalette = ForestColors_p; speed = 3; scale = 20; colorLoop = 0; break;
		case  7: targetPalette = CloudColors_p; speed = 4; scale = 20; colorLoop = 0; break;
		case  8: targetPalette = LavaColors_p;  speed = 8; scale = 19; colorLoop = 0; break;
		case  9: targetPalette = OceanColors_p; speed = 6; scale = 25; colorLoop = 0;  break;
		case  10: targetPalette = PartyColors_p; speed = 3; scale = 20; colorLoop = 1; break;
		}
	}
}

// This function generates a random palette that's a gradient
// between four different colors.  The first is a dim hue, the second is
// a bright hue, the third is a bright pastel, and the last is
// another bright hue.  This gives some visual bright/dark variation
// which is more interesting than just a gradient of different hues.

// LED animations ###############################################################################
void all_off() { fill_solid(targetPalette, 16, CRGB::Black); }
void SetupRandomPalette()//two colors
{
	EVERY_N_MILLISECONDS(8000) { //new random palette every 8 seconds. Might have to wait for the first one to show up
		CRGB black = CRGB::Black;
		CRGB random1 = CHSV(random8(), 255, 255);
		CRGB random2 = CHSV(random8(), 255, 255);
		targetPalette = CRGBPalette16(
			//                      CRGB( random8(), 255, 32),
			//                      CHSV( random8(), 255, 255),
			random1, random1, black, black,
			random2, random2, black, black,
			random1, random1, black, black,
			random2, random2, black, black);
		//                      CHSV( random8(), 128, 255),
		//                      CHSV( random8(), 255, 255), );
	}
}
void SetupRandomPalette_g()//three colors
{
	EVERY_N_MILLISECONDS(8000) { //new random palette every 8 seconds
		CRGB black = CRGB::Black;
		CRGB random1 = CHSV(random8(), 255, 255);
		CRGB random2 = CHSV(random8(), 200, 100);
		CRGB random3 = CHSV(random8(), 150, 200);
		targetPalette = CRGBPalette16(
			//                      CRGB( random8(), 255, 32),
			//                      CHSV( random8(), 255, 255),
			random1, random1, black, black,
			random2, random2, black, random3,
			random1, random1, black, black,
			random2, random2, black, random3);
		//                      CHSV( random8(), 128, 255),
		//                      CHSV( random8(), 255, 255), );
	}
}

// This function sets up a palette of black and white stripes,
// using code.  Since the palette is effectively an array of
// sixteen CRGB colors, the various fill_* functions can be used
// to set them up.
void SetupBlackAndWhiteStripedPalette_a()
{
	// 'black out' all 16 palette entries...
	fill_solid(targetPalette, 16, CRGB::Black);
	// and set every eighth one to white.
	currentPalette[0] = CRGB::White;
	// currentPalette[4] = CRGB::White;
	currentPalette[8] = CRGB::White;
	//  currentPalette[12] = CRGB::White;
}
//
// Mark's xy coordinate mapping code.  See the XYMatrix for more information on it.
//
/*  BORRAR  YA ESTA DEFINIDA   SE DEJA COMO REFERENCIA POR SI ES MEJOR SOLUCION  QUE LA OTRA

uint16_t XY( uint8_t x, uint8_t y)
{
  uint16_t i;
  if( kMatrixSerpentineLayout == false) {
	i = (y * CUSTOM_WIDTH) + x;
  }
  if( kMatrixSerpentineLayout == true) {
	if( y & 0x01) {
	  // Odd rows run backwards
	  uint8_t reverseX = (CUSTOM_WIDTH - 1) - x;
	  i = (y * CUSTOM_WIDTH) + reverseX;
	} else {
	  // Even rows run forwards
	  i = (y * CUSTOM_WIDTH) + x;
	}
  }
  return i;
} */
void FillLEDsFromPaletteColors(uint8_t colorIndex)
{
	uint8_t brightness = 255;

	for (int i = 0; i < CUSTOM_NUM_LEDS; i++) {
		c_leds(i) = ColorFromPalette(currentPalette, colorIndex + sin8(i * 16), brightness);
		colorIndex += 3;
	}
}

void LedsNoise(byte nada, byte nada1)
{
	//Serial.print("LedsNoise "); // debug

	// Periodically choose a new palette, speed, and scale
	ChangePaletteAndSettingsPeriodically();

	// Crossfade current palette slowly toward the target palette
  //
  // Each time that nblendPaletteTowardPalette is called, small changes
  // are made to currentPalette to bring it closer to matching targetPalette.
  // You can control how many changes are made in each call:
  // - the default of 24 is a good balance
  // - meaningful values are 1-48. 1=veeeeeeeery slow, 48=quickest
  // - "0" means do not change the currentPalette at all; freeze
  //  uint8_t maxChanges = 7;
  //  nblendPaletteTowardPalette( currentPalette, targetPalette, maxChanges);
   // generate noise data
	fillnoise8a(0, 0);

	// convert the noise data to colors in the LED array

  // using the current palette
	mapNoiseToLEDsUsingPalette();
	LEDS.show();
}

// animaciones pasadas desde funciones

// 2 oscillators flying arround one ;)
void Dots1(uint8_t color1, uint8_t color2) {
	MoveOscillators();
	ReadAudio();
	//2 lissajous dots red
	byte c1 = color1 + h;
	byte c2 = color2 + h;
	//if (myparameter1 > 2) { c1 = myparameter1; c2 = 255 - c1; }
	leds[XY(p[0], p[1])] = CHSV(c1, 255, myValue + soundOffset(2));
	leds[XY(p[2], p[3])] = CHSV(c2, 255, myValue + soundOffset(4));
	//average of the coordinates in yellow
	PixelHsv((p[2] + p[0]) / 2, (p[1] + p[3]) / 2, CHSV(50, 255, soundOffset(3)));
	ShowFrame();
	FastLED.delay(5 + random(20)); // antes 20
	HorizontalStream(myparameter1); // antes scale valor correcto 140
}

// x and y based on 3 sine waves
void Dots2(uint8_t scale, uint8_t nada) {
	MoveOscillators();
	Pixel((p[2] + p[0] + p[1]) / 3, (p[1] + p[3] + p[2]) / 3, osci[3]);
	ShowFrame();
	FastLED.delay(20);
	myParameter2 = scale*(myMode)+!(myMode)*myparameter1; //
	DimAll(myParameter2);
	HorizontalStream(myParameter2); // anstes scale
}

// beautifull but periodic
void SlowMandala2(byte dim, byte val) {
	for (int i = 1; i < 8; i++) {
		for (int j = 0; j < 16; j++) {
			MoveOscillators();
			myParameter2 = val*(myMode)+!(myMode)*myValue; //
			CHSV color = CHSV(((osci[0] + osci[1]) / 2), mySaturation, myParameter2);
			PixelHsv(j, i, color); // max (15+15)*8
			myParameter2 = dim*(myMode)+!(myMode)*myparameter1; //
			int dimAux = map(myParameter2, 0, 255, 95, 130);
			SpiralStream(4, 4, 4, dimAux); // antes dim
			Caleidoscope2();
			ShowFrame();
			FastLED.delay(20);
		}
	}
}

// same with a different timing
void SlowMandala3(byte dim, byte nada) {
	for (int i = 0; i < 16; i++) {
		for (int j = 0; j < 16; j++) {
			MoveOscillators();
			Pixel(j, j, (osci[0] + osci[1]) / 2);
			SpiralStream(4, 4, 4, dim);//antes myparameter1
			Caleidoscope2();
			ShowFrame();
			FastLED.delay(20);
		}
	}
}

// 2 lissajou dots *2 *4
void Mandala8(byte dim, byte nada) {
	MoveOscillators();
	Pixel(p[0] / 2, p[1] / 2, osci[2]);
	Pixel(p[2] / 2, p[3] / 2, osci[3]);
	Caleidoscope5();
	Caleidoscope2();
	myParameter2 = dim*(myMode)+!(myMode)*myparameter1; //
	int dimAux = map(myParameter2, 0, 255, 20, 110);
	HorizontalStream(dimAux); //antes dim
	ShowFrame();
}

// colorfull 2 chanel 7 band analyzer
void MSGEQtest(byte scale, byte nada) {
	ReadAudio();
	int s = 0;
	for (int i = 0; i < 7; i++) {
		s = soundOffset(i);
		Pixel(i, 16 - s / 64, s / 4); //left
		Pixel(8 + i, 16 - s / 64, s / 4);// right
	}
	ShowFrame();
	VerticalStream(scale);
}

// 2 frequencies linked to dot emitters in a spiral mandala
void MSGEQtest2(byte scale, byte color) {
	ReadAudio();
	if (left[0] > left_offset[0]) {
		PixelHsv(0, 0, CHSV(myparameter1, mySaturation, left[1])); // color 1
		PixelHsv(1, 1, CHSV(myparameter1, mySaturation, left[1])); // color 1
	}
	if (left[2] > left_offset[2]) {
		PixelHsv(2, 2, CHSV(myHue, mySaturation, left[2]));// color 100 , mysaturation used as color
	}
	if (left[6] > left_offset[6]) {
		PixelHsv(5, 0, CHSV(myHue, mySaturation, left[6])); // color color, myvalue used as color
	}
	SpiralStream(4, 4, 4, myValue);
	Caleidoscope1();
	ShowFrame();
}

// analyzer 2 bars
void MSGEQtest3(byte scale, byte color) {
	ReadAudio();
	for (int i = 0; i < 8; i++) {
		Pixel(i, 16 - left[0] / 64, 1);
	}
	for (int i = 8; i < 16; i++) {
		Pixel(i, 16 - left[4] / 64, color);
	}
	ShowFrame();
	VerticalStream(scale);
}

// analyzer x 4 (as showed on youtube)
void MSGEQtest4(byte dim, byte hmult) {
	ReadAudio();
	for (int i = 0; i < 7; i++) {
		Pixel(7 - i, 8 - right[i] / 128, i*hmult);
	}
	Caleidoscope2();
	ShowFrame();
	DimAll(dim);
}

// basedrum/snare linked to red/green emitters
void AudioSpiral(byte color1, byte color2) {
	MoveOscillators();
	myParameter2 = color1*(myMode)+!(myMode)*left_offset[5];
	myParameter3 = color2*(myMode)+!(myMode)*left_offset[6];
	SpiralStream(7, 7, 7, myParameter2);// color1 160
	SpiralStream(4, 4, 4, myHue);
	SpiralStream(11, 11, 3, myParameter3);// color2 110
	ReadAudio();
	int soundHue = map(left[3], 0, 1023, 0, 255);
	if (left[1] > left_offset[1]) { leds[4, 2] = CHSV(myHue, 255, 255); }
	if (left[4] > left_offset[4]) { leds[XY(random(15), random(15))] = CHSV(soundHue, 255, 255); } // antes hue variava con realRed ( color picker de light)

	ShowFrame();
	DimAll(250);
}

// one channel 7 band spectrum analyzer (spiral fadeout)
void MSGEQtest5(byte dim, byte hmult) {
	ReadAudio();
	for (int i = 0; i < 7; i++) {
		BresenhamLine(2 * i, 16 - left[i] * (left[i] > left_offset[i]) / 64, 2 * i, 15, i*hmult); // hmult
		BresenhamLine(1 + 2 * i, 16 - left[i] * (left[i] > left_offset[i]) / 64, 1 + 2 * i, 15, i*hmult); // hmult
	}
	ShowFrame();
	myParameter2 = dim*(myMode)+!(myMode)*myValue; // 124
	SpiralStream(7, 7, 7, myParameter2); // dim 120
}

// classic analyzer, slow falldown
void MSGEQtest6(byte dim, byte hmult) {
	ReadAudio();
	for (int i = 0; i < 7; i++) {
		BresenhamLine(2 * i, 16 - left[i] * (left[i] > left_offset[i]) / 64, 2 * i, 15, i*hmult);
		BresenhamLine(1 + 2 * i, 16 - left[i] * (left[i] > left_offset[i]) / 64, 1 + 2 * i, 15, i*hmult);
	}
	ShowFrame();
	myParameter2 = dim*(myMode)+!(myMode)*myValue; // 124
	VerticalStream(myParameter2); //dim 193
}

void offsets_test(byte dim, byte hmult) {
	for (int i = 0; i < 7; i++) {
		BresenhamLine(2 * i, 16 - left_offset[i] / 64, 2 * i, 15, i*hmult);
		BresenhamLine(1 + 2 * i, 16 - left_offset[i] / 64, 1 + 2 * i, 15, i*hmult);
	}
	ShowFrame();
	VerticalStream(dim);
}

// geile Schei?e
// spectrum mandala, color linked to 160Hz band
void MSGEQtest7(byte dim, byte hmult) {
	MoveOscillators();
	ReadAudio();
	for (int i = 0; i < 7; i++) {
		PixelHsv(7 - i, 8 - soundOffset(i) / 128, CHSV(i*hmult + soundOffset(1) / 8, mySaturation, myValue + soundOffset(2)));
	}
	Caleidoscope5();
	Caleidoscope1();
	ShowFrame();
	myParameter2 = dim*(myMode)+!(myMode)*myparameter1; // 200
	DimAll(myParameter2);
}

// spectrum mandala, color linked to osci
void MSGEQtest8(byte dim, byte hmult) {
	MoveOscillators();
	ReadAudio();
	for (int i = 0; i < 7; i++) {
		PixelHsv(7 - i, 8 - soundOffset(i) / 128, CHSV(i*hmult + osci[1], mySaturation, myValue + soundOffset(2)));
	}
	Caleidoscope5();
	Caleidoscope2();
	ShowFrame();
	myParameter2 = dim*(myMode)+!(myMode)*myparameter1; //
	DimAll(myParameter2);
}

// falling spectogram
void MSGEQtest9(byte hmult, byte s) {
	ReadAudio();
	myParameter2 = s*(myMode)+!(myMode)*mySaturation; //
	for (int i = 0; i < 7; i++) {
		leds[XY(i * 2, 0)] = CHSV(i*hmult, s, soundOffset(i) / 3); // brightness should be divided by 4
		leds[XY(1 + i * 2, 0)] = CHSV(i*hmult, myParameter2, soundOffset(i) / 3);
	}
	leds[XY(14, 0)] = 0;
	leds[XY(15, 0)] = 0;
	ShowFrame();
	VerticalMove();
}

// 9 analyzers
void CopyTest(byte dim, byte hmult) {
	ReadAudio();
	for (int i = 0; i < 5; i++) {
		//int hue = i*hmult;
		int hue = (soundOffset(i) + soundOffset(6 - i)) / 4 + myHue;
		CHSV color = CHSV(i*hue, mySaturation, hue);
		BresenhamLineHsv(i, 4 - soundOffset(i) / 256, i, 4, color);
	}
	Copy(0, 0, 4, 4, 5, 0);
	Copy(0, 0, 4, 4, 10, 0);
	Copy(0, 0, 14, 4, 0, 5);
	Copy(0, 0, 14, 4, 0, 10);
	ShowFrame();
	myParameter2 = dim*(myMode)+!(myMode)*myparameter1; //
	DimAll(myParameter2);
}

// test scale
// NOT WORKING as intended YET!
void CopyTest2(byte dim, byte hmult) {
	ReadAudio();
	for (int i = 0; i < 5; i++) {
		myParameter2 = dim*(myMode)+!(myMode)*myparameter1; //
		myParameter3 = hmult*i*(myMode);
		myParameter3 += !(myMode)*myHue; //
		CHSV color = CHSV(i*myParameter3, mySaturation, soundOffset(i));
		BresenhamLineHsv(i * 2, 4 - soundOffset(i) / 128, i * 2, 4, color);
	}
	Scale(0, 0, 4, 4, 7, 7, 15, 15);
	ShowFrame();
	DimAll(myParameter2);
}

// line spectogram mandala
void Audio1(byte dim, byte hmult) {
	ReadAudio();
	for (int i = 0; i < 5; i++) {
		int s1 = soundOffset(i);
		s1 = map(s1, 0, 1023, 0, 15);
		int s2 = soundOffset(i + 1);
		s2 = map(s2, 0, 1023, 0, 15);
		CHSV color = CHSV(myHue + i*hmult, mySaturation, soundAverage());
		BresenhamLineHsv(3 * i, 15 - s1, 3 * (i + 1), 15 - s2, color);
	}
	Caleidoscope4();
	ShowFrame();
	myParameter2 = dim*(myMode)+!(myMode)*myparameter1; //
	DimAll(myParameter2);
}

// line analyzer with stream
void Audio2(byte dim, byte hmult) {
	ReadAudio();
	for (int i = 0; i < 5; i++) {
		int s1 = soundOffset(i);
		s1 = map(s1, 0, 1023, 0, 15);
		int s2 = soundOffset(i + 1);
		s2 = map(s2, 0, 1023, 0, 15);
		BresenhamLine(3 * i, 15 - s1, 3 * (i + 1), 15 - s2, 255 - i*hmult);
	}
	ShowFrame();
	myParameter2 = dim*(myMode)+!(myMode)*myparameter1; //
	HorizontalStream(myParameter2);
}

void Audio3(byte dim, byte hmult) {
	ReadAudio();
	for (int i = 0; i < 7; i++) {
		int s1 = soundOffset(i);
		int s2 = map(s1, 0, 1023, 0, 8);
		leds[XY(7 - i, s2)] = CHSV(i*hmult, 255, soundAverage()); // antes 6
	} // brightness should be divided by 4
	Caleidoscope6();
	Caleidoscope2();
	ShowFrame();
	myParameter2 = dim*(myMode)+!(myMode)*myparameter1; //
	DimAll(myParameter2);
}

void Audio4(byte dim, byte hdiv) {
	ReadAudio();
	for (int i = 0; i < 5; i++) {
		int s1 = soundOffset(i);
		int s2 = map(s1, 0, 1023, 0, 8);
		CHSV color = CHSV(i*s1 / hdiv, mySaturation, soundAverage() + myValue);
		BresenhamLineHsv(3 * i, 8 - s2, 3 * (i + 1), 8 - soundOffset(i + 1) / 128, color);
	}
	Caleidoscope4();
	ShowFrame();
	myParameter2 = dim*(myMode)+!(myMode)*myparameter1; //
	DimAll(myParameter2);
}

void CaleidoTest1(byte dim, byte hdiv) {
	ReadAudio();
	for (int i = 0; i < 8; i++) {
		int ch = (i < 7) ? i : 6;
		int s1 = soundOffset(ch);
		int s2 = map(s1, 0, 1023, 0, 8);
		CHSV color = CHSV(s1, mySaturation, s1 / 4 + myValue);
		BresenhamLineHsv(i, s2, i, 0, color);
	}
	RotateTriangle();
	Caleidoscope2();  //copy + rotate
	ShowFrame();
	myParameter2 = dim*(myMode)+!(myMode)*myparameter1; //
	DimAll(myParameter2);
}

void CaleidoTest2(byte dim, byte color_ofset) {
	MoveOscillators();
	ReadAudio();
	for (int i = 0; i < 8; i++) {
		int ch = (i < 7) ? i : 6;
		int s1 = soundOffset(ch);
		BresenhamLine(i, s1 / 200, i, 0, ((s1 / 8) + myHue) % 255); // antes color_offset
	}
	MirrorTriangle();
	Caleidoscope1();  //mirror + rotate
	ShowFrame();
	myParameter2 = dim*(myMode)+!(myMode)*myparameter1; //
	DimAll(myParameter2);
}

void Audio5(byte dim, byte hmult) {
	ReadAudio();
	for (int i = 0; i < 5; i++) {
		BresenhamLine(
			3 * i, 8 - soundOffset(i) / 128,        // from
			3 * (i + 1), 8 - soundOffset(i + 1) / 128,  // to
			i * hmult);
	}                       // color
	Caleidoscope4();
	ShowFrame();
	myParameter2 = dim*(myMode)+!(myMode)*myparameter1; //
	DimAll(myParameter2);
}

// este es una version mas tipo vumetro del audio 6, este es cuadrado
void Audio6(byte dim, byte hmult) {
	ReadAudio();
	for (int i = 0; i < 16; i++) {
		int ch = map(i, 0, 15, 0, 5);
		BresenhamLine(
			i, 8 - soundOffset(ch) / 128,        // from
			(i), 8 - soundOffset(ch + 1) / 128,  // to
			i * 10);                         // lolor
		BresenhamLine(
			15 - i, 7 + soundOffset(ch) / 128,        // from
			15 - i, 7 + soundOffset(ch + 1) / 128,  // to
			i * hmult);                              // color
	}
	ShowFrame();
	myParameter2 = dim*(myMode)+!(myMode)*myparameter1; //
	DimAll(myParameter2);
	//ClearAll();
}

void Audio7(byte dim, byte hmult) {
	ReadAudio();
	for (int i = 0; i < 5; i++) {
		BresenhamLine(
			3 * i, 8 - left[i] / 128,        // from
			3 * (i + 1), 8 - left[i + 1] / 128,  // to
			i * 10);                         // lolor
		BresenhamLine(
			15 - (3 * i), 7 + left[i] / 128,        // from
			15 - (3 * (i + 1)), 7 + left[i + 1] / 128,  // to
			i * hmult);                              // color
	}
	ShowFrame();
	myParameter2 = dim*(myMode)+!(myMode)*myparameter1; //
	DimAll(myParameter2);
}

void NoiseExample1(byte hpow2, byte scale1) {
	MoveOscillators();
	scale = scale1 * (myMode)+!(myMode)*myparameter1; // fill noise zoom
	myParameter2 = 255 * (myMode)+!(myMode)*mySaturation; // default value in auto mode
	scale2 = 30 + p[1] * 3; // fill noise2 zoom
	x = p[0] * 16;
	fillnoise8();
	fillnoise82();
	for (int i = 0; i < kMatrixWidth; i++) {
		for (int j = 0; j < kMatrixHeight; j++) {
			leds[XY(i, j)] =
				CHSV(noise[0][i][j] << hpow2, myParameter2, (noise2[i][j] + noise[0][i][j]) / 2);
		}
	}
	ShowFrame();
}

void NoiseExample2(byte noisez, byte scale) { //original  100 , 100
	MoveOscillators();
	myParameter2 = scale*(myMode)+!(myMode)*myparameter1; // 124
	myParameter3 = noisez*(myMode)+!(myMode)*myHue; //
	FillNoise(1550 - p[2] * 100, 100, myParameter3 + z, myParameter2);
	for (int i = 0; i < p[2]; i++) {
		for (int j = 0; j < kMatrixHeight; j++) {
			CHSV color = CHSV(myHue, mySaturation, (noise[0][i][j] < myValue) ? 0 : noise[0][i][j]);
			leds[XY(i, j)] = CHSV(color);
		}
	}
	for (int i = 0; i < p[1]; i++) {
		for (int j = 0; j < kMatrixHeight; j++) {
			leds[XY(j, i)] += CHSV(noise[0][i][j], mySaturation, noise[0][i][j]);
		}
	}
	ShowFrame();
	ClearAll();
	z += 0;
}

void NoiseExample3(byte noisez, byte scale) {
	MoveOscillators();
	FillNoise(p[1] * 100, p[2] * 100, noisez, scale);
	for (int i = 0; i < p[1]; i++) {
		for (int j = 0; j < kMatrixHeight; j++) {
			leds[XY(i, j)] = CHSV(noise[0][i][j], 255, 200);
		}
	}

	for (int i = 0; i < p[3]; i++) {
		for (int j = 0; j < kMatrixHeight; j++) {
			leds[XY(j, i)] += CHSV(128 + noise[0][i][j], 255, 200);
		}
	}

	ShowFrame();
	ClearAll();
}
//
void NoiseExample4(byte noisez, byte scale) {
	MoveOscillators();
	myParameter2 = scale*(myMode)+!(myMode)*myparameter1; // 124
	myParameter3 = noisez*(myMode)+!(myMode)*myValue; //
	FillNoise(100, 100, myParameter3, myParameter2);
	for (int i = 0; i < 16; i++) {
		for (int j = 0; j < kMatrixHeight; j++) {
			CHSV color = CHSV(noise[0][i][j + p[1]] + myHue, mySaturation, noise[0][i][j + p[1]]);
			leds[XY(i, j)] += color;
		}
	}
	ShowFrame();
	ClearAll(); // hace efecto de cortina
	//fadeToBlackBy(leds,NUM_LEDS,myValue);
	z = myParameter3;
}
//noiseExample4  original
void NoiseExample14(byte noisez, byte scale) {
	MoveOscillators();
	FillNoise(100, 100, noisez, scale);
	for (int i = 0; i < p[0] + 1; i++) {
		for (int j = 0; j < kMatrixHeight; j++) {
			leds[XY(i, j)] += CHSV(noise[0][i][j + p[2]], 255, 255);
		}
	}
	ShowFrame();
	//ClearAll(); // hace efecto de cortina
	fadeToBlackBy(leds, NUM_LEDS, myValue);
}

void NoiseExample5(byte noisez, byte scale) {
	MoveOscillators();
	ReadAudio();
	FillNoise(100, 100, noisez, scale);

	for (int i = 0; i < kMatrixWidth; i++) {
		for (int j = 0; j < left[1] / 64; j++) {
			leds[XY(i, 15 - j)] = CRGB(0, noise[0][i][left[1] / 64 - j], 0);
		}
	}

	for (int i = 0; i < kMatrixWidth; i++) {
		for (int j = 0; j < left[5] / 64; j++) {
			leds[XY(j, i)] += CRGB(noise[0][i][left[5] / 64 - j], 0, 0);
		}
	}
	ShowFrame();
	ClearAll();
}

void NoiseExample6(byte hofset, byte scale) {
	//MoveOscillators();
	myParameter2 = scale*(myMode)+!(myMode)*myparameter1; // 124
	myParameter3 = hofset*(myMode)+!(myMode)*myValue; //
	for (int size = 1; size < myParameter2; size++) {
		z++;
		// simula que hace zoom
		FillNoise(size, size, z, size);
		for (int i = 0; i < kMatrixWidth; i++) {
			for (int j = 0; j < kMatrixHeight; j++) {
				myParameter3 = (noise[0][i][j] > 30) ? myValue : 0;
				yield();
				leds[XY(i, j)] = CHSV(myHue + noise[0][i][j], mySaturation, myParameter3);
			}
		}
		ShowFrame();
		//ClearAll();
	}
	// simula que se reduce
	for (int size = myParameter2; size > 2; size--) {
		z++;
		yield();
		FillNoise(size, size, z, size);
		for (int i = 0; i < kMatrixWidth; i++) {
			for (int j = 0; j < kMatrixHeight; j++) {
				myParameter3 = (noise[0][i][j] > 10) ? myValue : 0;
				leds[XY(i, j)] = CHSV(myHue + noise[0][i][j], mySaturation, myParameter3);
			}
		}
		ShowFrame();
		//ClearAll();
	}
}

void NoiseExample7(byte nada, byte scale) {
	ChangePalettePeriodically();
	for (int size = 1; size < scale; size++) {
		yield();
		z++;
		FillNoise(size * 3, size * 3, z, size);
		for (int i = 0; i < kMatrixWidth; i++) {
			for (int j = 0; j < kMatrixHeight; j++) {
				leds[XY(i, j)] = ColorFromPalette(currentPalette, noise[0][i][j], 255, currentBlending);
			}
		}
		ShowFrame();
	}
	for (int size = scale; size > 1; size--) {
		yield();
		z++;
		FillNoise(size * 3, size * 3, z, size);
		for (int i = 0; i < kMatrixWidth; i++) {
			for (int j = 0; j < kMatrixHeight; j++) {
				leds[XY(i, j)] = ColorFromPalette(currentPalette, noise[0][i][j], 255, currentBlending);
			}
		}
		ShowFrame();
	}
}

void drawEstrella(uint8_t x, uint8_t y, uint8_t arms, uint8_t start, uint8_t length, CHSV color) //center , #arms,start length, length, color
{
	uint8_t delta = 64 / arms; // angulo incremental
	for (uint16_t angle = 0; angle <= 254; angle = angle + delta) // vuelta completa
	{
		drawLineByAngle(x, y, angle, start, length, color);
		//Serial.print("\tangle= ");Serial.println( angle);// debug
		drawLineByAngle(x, y, angle + 128, start, length, color);
		;
	}

	blur2d(c_leds[0], MATRIX_WIDTH, MATRIX_HEIGHT, 16);
	RenderCustomMatrix();
}

void DrawVentilador(uint8_t centrox, uint8_t centroy, uint8_t radio, int angulo, CHSV color) {
	color.h = random8(200);
	drawLineByAngle(centrox, centroy, angulo, 2, radio, color);
	//blur2d((leds[0]), MATRIX_WIDTH, MATRIX_HEIGHT, 32);
	FastLED.show();
}

void diagonalFill(byte nada, byte nada1) {
	// ** Fill LED's with diagonal stripes
	for (uint8 x = 0; x < (c_leds.Width() + c_leds.Height()); ++x)
	{
		c_leds.DrawLine(x - c_leds.Height(), c_leds.Height() - 1, x, 0, CHSV(h, 255, 255));
		h += 16;
	}
	hue += 4;
	FastLED.show();
	fadeToBlackBy(c_leds[0], 480, 16);
}

void HorizontalStripes(byte nada, byte nada1) {
	// ** Fill LED's with horizontal stripes
	for (uint8 y = 0; y < c_leds.Height(); ++y)
	{
		ReadAudio();
		uint8_t val1 = uint8(map(soundOffset(2), 0, 1023, myValue, 255));// limits max val according to   myValue setting
		c_leds.DrawLine(0, y, c_leds.Width() - 1, y, CHSV(h, mySaturation, val1));

		h += 16;
	}
	blur2d(c_leds[0], MATRIX_WIDTH, MATRIX_HEIGHT, 16);// blur the individual circles
	hue += 4;
	FastLED.show();
	fadeToBlackBy(c_leds[0], 480, 16);
}
// horizontal stripes original
void HorizontalStripes1(byte nada, byte nada1) {
	// ** Fill LED's with horizontal stripes
	for (uint8 y = 0; y < c_leds.Height(); ++y)
	{
		c_leds.DrawLine(0, y, c_leds.Width() - 1, y, CHSV(h, 255, 255));

		h += 16;
	}

	blur2d(c_leds[0], MATRIX_WIDTH, MATRIX_HEIGHT, 16);// blur the individual circles
	hue += 4;
	FastLED.show();
	fadeToBlackBy(c_leds[0], 480, 16);
}

void anillos(byte nada, byte nada1) {
	// ** circulos concentricos
	for (uint8 y = 0; y < c_leds.Height(); ++y)
	{
		c_leds.DrawCircle(10, 12, y, CHSV(h, 255, 250));

		h += 16;
	}
	blur2d(c_leds[0], MATRIX_WIDTH, MATRIX_HEIGHT, 16);// blur the individual circles
	hue += 4;
	FastLED.show();
	fadeToBlackBy(c_leds[0], 480, 16);
}

void circulo1(byte nada, byte nada1) {
	cir1.Update();
	fadeToBlackBy(c_leds[0], 480, 32);
	blur2d((c_leds[0]), MATRIX_WIDTH, MATRIX_HEIGHT, 32);
}

void HorMirror(byte nada, byte nada1) {
	HorizontalStripes(0, 0);
	hue += 4;
	c_leds.HorizontalMirror();
	FastLED.show();
	fadeToBlackBy(c_leds[0], 480, 16);
};

void VertMirror(byte nada, byte nada1) {
	HorizontalStripes(0, 0);
	hue += 4;
	c_leds.VerticalMirror();
	FastLED.show();
	fadeToBlackBy(c_leds[0], 480, 16);
};

//void QuadMirror(byte nada, byte nada1) {
//	HorizontalStripes(0,0);
//	hue += 4;
//	c_leds.QuadrantMirror();
//	FastLED.show();
//	fadeToBlackBy(c_leds[0], 480, 16);
//};

void circulo2(byte nada, byte nada1) {
	circleB1.Update();
	circleB2.Update();
	fadeToBlackBy(c_leds[0], 480, myparameter1);
	FastLED.show();
	//blur2d((c_leds[0]), MATRIX_WIDTH, MATRIX_HEIGHT, 32);
}

void nextPattern()
{
	// add one to the current pattern number, and wrap around at the end
	//const int numberOfPatterns = sizeof(gPatternsAndArguments) / sizeof(gPatternsAndArguments[0]);
	gCurrentPatternNumber = (gCurrentPatternNumber + 1) % numberOfPatterns;
	Serial.print("funcion : \t");
	Serial.println(gCurrentPatternNumber);
	sendAll(); // lo pongo aqui , pero en realidad deberia ser solo cuando hay un cambio de efecto
}

// ----- stefan noise audio
// as showed on youtube
DEFINE_GRADIENT_PALETTE(pit3) {
	0, 3, 3, 3,
		64, 13, 13, 255,  // blue
		128, 3, 3, 3,
		192, 255, 130, 3,   // orange
		255, 3, 3, 3
};
DEFINE_GRADIENT_PALETTE(pit4) {
	0, 0, 0, 0,   //black
		128, 255, 0, 0,   //red
		224, 255, 255, 0,   //bright yellow
		255, 255, 255, 255,
		255, 3, 3, 3
}; //full white
//void noise_audio2() {  // original name
void QuadMirror(byte nada1, byte nada2) {
	ReadAudio();
	CRGBPalette16 Pal(pit3); // the red one
	y_n[0] += (soundOffset(4) - 10) * 4;
	scale_x[0] = 10000 - (soundOffset(0) * 40);
	scale_y[0] = scale_x[0];
	byte layer = 0;
	for (uint8_t i = 0; i < WIDTH; i++) {
		uint32_t ioffset = scale_x[layer] * (i - CentreX);
		for (uint8_t j = 0; j < HEIGHT; j++) {
			uint32_t joffset = scale_y[layer] * (j - CentreY);
			uint16_t data = inoise16(x_n[layer] + ioffset, y_n[layer] + joffset, z_n[layer]);
			// results in the range of 11k - 51k
			// limit results:
			if (data < 11000) data = 11000;
			if (data > 51000) data = 51000;
			// normalize data to a range of 0 - 40000
			data = data - 11000;
			// scale down
			data = data / 161;
			noise_n[layer][i][j] = data;
		}
	}
	// map 1st layer
	for (uint8_t y = 0; y < HEIGHT; y++) {
		for (uint8_t x = 0; x < WIDTH; x++) {
			leds[XY(x, y)] = ColorFromPalette(Pal, (millis() / 130) + noise_n[0][x][y]);
		}
	}

	//2nd layer
	CRGBPalette16 Pal2(pit4); // the blue one
	y_n[1] -= (soundOffset(5) - 10) * 4;
	//z[1] += 9;
	scale_x[1] = 10000 - (soundOffset(1) * 40);
	scale_y[1] = scale_x[1];
	layer = 1;
	for (uint8_t i = 0; i < WIDTH; i++) {
		uint32_t ioffset = scale_x[layer] * (i - CentreX);
		for (uint8_t j = 0; j < HEIGHT; j++) {
			uint32_t joffset = scale_y[layer] * (j - CentreY);
			uint16_t data = inoise16(x_n[layer] + ioffset, y_n[layer] + joffset, z_n[layer]);
			// results in the range of 11k - 51k
			// limit results:
			if (data < 11000) data = 11000;
			if (data > 51000) data = 51000;
			// normalize data to a range of 0 - 40000
			data = data - 11000;
			// scale down
			data = data / 161;
			noise_n[layer][i][j] = data;
		}
	}
	for (uint8_t y = 0; y < HEIGHT; y++) {
		for (uint8_t x = 0; x < WIDTH; x++) {
			// map 2nd layer
			buffer1[XY(x, y)] = ColorFromPalette(Pal2, (soundOffset(0) / 4) + noise_n[1][x][y]);
			// add both layers
			leds[XY(x, y)] = buffer1[XY(x, y)] + leds[XY(x, y)];
		}
	}
	adjust_gamma();
	ShowFrame();
}

DEFINE_GRADIENT_PALETTE(pit) {
	0, 3, 3, 3,
		64, 13, 13, 255,  //blue
		128, 3, 3, 3,
		192, 255, 130, 3,  //orange
		255, 3, 3, 3
};
//a noise controlled & modulated by itself
void noise_noise1(byte nada, byte nada1) {
	CRGBPalette16 Pal(pit);

	//modulate the position so that it increases/decreases x
	//(here based on the top left pixel - it could be any position else)
	//the factor "2" defines the max speed of the x movement
	//the "-255" defines the median moving direction
	x_n[0] = x_n[0] + (2 * noise_n[0][0][0]) - 255;
	//modulate the position so that it increases/decreases y
	//(here based on the top right pixel - it could be any position else)
	y_n[0] = y_n[0] + (2 * noise_n[0][WIDTH - 1][0]) - 255;
	//z just in one direction but with the additional "1" to make sure to never get stuck
	//in case the movement is stopped by a crazy parameter (noise data) combination
	//(here based on the down left pixel - it could be any position else)
	z_n[0] += 1 + ((noise_n[0][0][HEIGHT - 1]) / 4);
	//set the scaling based on left and right pixel of the middle line
	//here you can set the range of the zoom in both dimensions
	scale_x[0] = 8000 + (noise_n[0][0][CentreY] * 16);
	scale_y[0] = 8000 + (noise_n[0][WIDTH - 1][CentreY] * 16);

	//calculate the noise data
	uint8_t layer = 0;
	for (uint8_t i = 0; i < WIDTH; i++) {
		uint32_t ioffset = scale_x[layer] * (i - CentreX);
		for (uint8_t j = 0; j < HEIGHT; j++) {
			uint32_t joffset = scale_y[layer] * (j - CentreY);
			uint16_t data = inoise16(x_n[layer] + ioffset, y_n[layer] + joffset, z_n[layer]);
			// limit the 16 bit results to the interesting range
			if (data < 11000) data = 11000;
			if (data > 51000) data = 51000;
			// normalize
			data = data - 11000;
			// scale down that the result fits into a byte
			data = data / 161;
			// store the result in the array
			noise_n[layer][i][j] = data;
		}
	}

	//map the colors
	for (uint8_t y = 0; y < HEIGHT; y++) {
		for (uint8_t x = 0; x < WIDTH; x++) {
			//I will add this overlay CRGB later for more colors
			//it´s basically a rainbow mapping with an inverted brightness mask
			CRGB overlay = CHSV(noise_n[0][y][x], 255, noise_n[0][x][y]);
			//here the actual colormapping happens - note the additional colorshift caused by the down right pixel noise[0][15][15]
			leds[XY(x, y)] = ColorFromPalette(Pal, noise_n[0][WIDTH - 1][HEIGHT - 1] + noise_n[0][x][y]) + overlay;
		}
	}

	//make it looking nice
	adjust_gamma();

	//and show it!
	ShowFrame();
}

// highly experimental
// note that the noise values are in the range of 0-1023 when using them
void noise_noise2(byte nada, byte nada1) {
	// LAYER ONE
	//top left
	ctrl[0] = (ctrl[0] + noise_n[0][0][0] + noise_n[0][1][0] + noise_n[0][0][1] + noise_n[0][1][1]) / 20;
	//top right
	ctrl[1] = (ctrl[1] + noise_n[0][WIDTH - 1][0] + noise_n[0][WIDTH - 2][0] + noise_n[0][WIDTH - 1][1] + noise_n[0][WIDTH - 2][1]) / 20;
	//down left
	ctrl[2] = (ctrl[2] + noise_n[0][0][HEIGHT - 1] + noise_n[0][0][HEIGHT - 2] + noise_n[0][1][HEIGHT - 1] + noise_n[0][1][HEIGHT - 2]) / 20;
	//middle left
	ctrl[3] = (ctrl[3] + noise_n[0][0][CentreY] + noise_n[0][1][CentreY] + noise_n[0][0][CentreY + 1] + noise_n[0][1][CentreY + 1]) / 20;
	//center
	ctrl[4] = (ctrl[4] + noise_n[0][WIDTH - 1][CentreY] + noise_n[0][WIDTH - 2][CentreY] + noise_n[0][WIDTH - 1][CentreY + 1] + noise_n[0][WIDTH - 2][CentreY + 1]) / 20;
	ctrl[5] = (ctrl[5] + ctrl[0] + ctrl[1]) / 12;

	x_n[0] = x_n[0] + (ctrl[0]) - 127;
	y_n[0] = y_n[0] + (ctrl[1]) - 127;
	z_n[0] += 1 + (ctrl[2]);// ORIGINAL /4
	scale_x[0] = 8000 + (ctrl[3] * 16);
	scale_y[0] = 8000 + (ctrl[4] * 16);

	//calculate the noise data
	uint8_t layer = 0;
	for (uint8_t i = 0; i < WIDTH; i++) {
		uint32_t ioffset = scale_x[layer] * (i - CentreX);
		for (uint8_t j = 0; j < HEIGHT; j++) {
			uint32_t joffset = scale_y[layer] * (j - CentreY);
			uint16_t data = inoise16(x_n[layer] + ioffset, y_n[layer] + joffset, z_n[layer]);
			if (data < 11000) data = 11000;
			if (data > 51000) data = 51000;
			data = data - 11000;
			data = data / 41;
			noise_n[layer][i][j] = (noise_n[layer][i][j] + data) / 2;
		}
	}

	//map the colors
	//here the red layer
	for (uint8_t y = 0; y < HEIGHT; y++) {
		for (uint8_t x = 0; x < WIDTH; x++) {
			uint16_t i = noise_n[0][x][y];
			buffer1[XY(x, y)] = CRGB(a[i], 0, 0);
		}
	}

	// LAYER TWO
	//top left
	ctrl[0] = (ctrl[0] + noise_n[1][0][0] + noise_n[1][1][0] + noise_n[1][0][1] + noise_n[1][1][1]) / 20; // antes /20
	//top right
	ctrl[1] = (ctrl[1] + noise_n[1][WIDTH - 1][0] + noise_n[1][WIDTH - 2][0] + noise_n[1][WIDTH - 1][1] + noise_n[1][WIDTH - 2][1]) / 20;
	//down left
	ctrl[2] = (ctrl[2] + noise_n[1][0][HEIGHT - 1] + noise_n[1][0][HEIGHT - 2] + noise_n[1][1][HEIGHT - 1] + noise_n[1][1][HEIGHT - 2]) / 20;
	//middle left
	ctrl[3] = (ctrl[3] + noise_n[1][0][CentreY] + noise_n[1][1][CentreY] + noise_n[1][0][CentreY + 1] + noise_n[1][1][CentreY + 1]) / 20;
	//center
	ctrl[4] = (ctrl[4] + noise_n[1][WIDTH - 1][CentreY] + noise_n[1][WIDTH - 2][CentreY] + noise_n[1][WIDTH - 1][CentreY + 1] + noise_n[1][WIDTH - 2][CentreY + 1]) / 20;
	ctrl[5] = (ctrl[5] + ctrl[0] + ctrl[1]) / 12;

	x_n[1] = x_n[1] + (ctrl[0]) - 127;
	y_n[1] = y_n[1] + (ctrl[1]) - 127;
	z_n[1] += 1 + (ctrl[2]); // ORIGINAL /4
	scale_x[1] = 8000 + (ctrl[3] * 64); // antes  8000 + (ctrl[3] * 16);
	scale_y[1] = 8000 + (ctrl[4] * 64);

	//calculate the noise_n data
	layer = 1;
	for (uint8_t i = 0; i < WIDTH; i++) {
		uint32_t ioffset = scale_x[layer] * (i - CentreX);
		for (uint8_t j = 0; j < HEIGHT; j++) {
			uint32_t joffset = scale_y[layer] * (j - CentreY);
			uint16_t data = inoise16(x_n[layer] + ioffset, y_n[layer] + joffset, z_n[layer]);
			if (data < 11000) data = 11000;
			if (data > 51000) data = 51000;
			data = data - 11000;
			data = data / 41;
			noise_n[layer][i][j] = (noise_n[layer][i][j] + data) / 2;
		}
	}

	//map the colors
	//here the blue layer
	for (uint8_t y = 0; y < HEIGHT; y++) {
		for (uint8_t x = 0; x < WIDTH; x++) {
			uint16_t i = noise_n[1][x][y];
			buffer2[XY(x, y)] = CRGB(0, 0, a[i]);
		}
	}

	// blend
	//for (uint16_t i = 0; i < NUM_LEDS; i++) {leds[i] = buffer1[i] + buffer2[i];}
	for (uint8_t y = 0; y < HEIGHT; y++) {
		for (uint8_t x = 0; x < WIDTH; x++) {
			leds[XY(x, y)] = blend(buffer1[XY(x, y)], buffer2[XY(x, y)], noise_n[1][y][x] / 4);
			// you could also just add them:
			// leds[XY(x, y)] = buffer1[XY(x, y)] + buffer2[XY(x, y)];
		}
	}

	//make it looking nice
	adjust_gamma();

	//and show it!
	ShowFrame();
}// highly experimental

 /* https://github.com/pixelmatix/aurora/blob/d087efe69b539e2c00a026dde2fa205210d4f3b3/Menu.h
 * Aurora: https://github.com/pixelmatix/aurora
 * Copyright (c) 2014 Jason Coon
 *
 * Portions of this code are adapted from FastLED Fire2012 example by Mark Kriegsman: https://github.com/FastLED/FastLED/tree/master/examples/Fire2012WithPalette
 * Copyright (c) 2013 FastLED
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

 // Array of temperature readings at each simulation cell
byte heat[MATRIX_WIDTH * MATRIX_HEIGHT];

uint8_t noisesmoothing = 200;
const int MATRIX_CENTER_X = MATRIX_WIDTH / 2;
const int MATRIX_CENTER_Y = MATRIX_HEIGHT / 2;
const byte MATRIX_CENTRE_X = MATRIX_CENTER_X - 1;
const byte MATRIX_CENTRE_Y = MATRIX_CENTER_Y - 1;
CRGB ColorFromCurrentPalette(uint8_t, uint8_t, TBlendType);
void MoveFractionalNoiseX(byte);
TBlendType currentBlendType = LINEARBLEND;
// The coordinates for 3 16-bit noise spaces.

uint32_t noise_x[NOISE_NUM_LAYERS];
uint32_t noise_y[NOISE_NUM_LAYERS];
uint32_t noise_z[NOISE_NUM_LAYERS];
uint32_t noise_scale_x[NOISE_NUM_LAYERS];
uint32_t noise_scale_y[NOISE_NUM_LAYERS];

void  PatternFire(byte nada, byte nada1) {
	// There are two main parameters you can play with to control the look and
	// feel of your fire: COOLING (used in step 1 above), and SPARKING (used
	// in step 3 above).
	//
	// cooling: How much does the air cool as it rises?
	// Less cooling = taller flames.  More cooling = shorter flames.
	// Default 55, suggested range 20-100
	int cooling = constrain(ctrl[0],5,250);

	// sparking: What chance (out of 255) is there that a new spark will be lit?
	// Higher chance = more roaring fire.  Lower chance = more flickery fire.
	// Default 120, suggested range 50-200.
	unsigned int sparking = constrain(ctrl[1], 50, 200);


		// Add entropy to random number generator; we use a lot of it.
	random16_add_entropy(random(255));

	 //DimAll(byte value)
	for (int i = 0; i < MATRIX_WIDTH*MATRIX_HEIGHT; i++) {
		c_leds(i).nscale8(64); // dim to 192/256 = 75% of original value
	}
	FastLED.show();
	

	for (int x = 0; x < MATRIX_WIDTH; x++) {
		// Step 1.  Cool down every cell a little
		for (int y = 0; y < MATRIX_HEIGHT; y++) {
			//int xy = XY2(x, y);// antes XY()
			int xy = XY2(x, MATRIX_HEIGHT- y);// antes XY()
			heat[xy] = qsub8(heat[xy], random8(0, cooling+2 ));// random8(0, ((cooling * 10) / MATRIX_HEIGHT) + 2));
		}

		// Step 2.  Heat from each cell drifts 'up' and diffuses a little
		for (int y = 0; y < MATRIX_HEIGHT; y++) {
			heat[XY2(x, y)] = (heat[XY2(x, y + 1)] + heat[XY2(x, y + 2)] + heat[XY2(x, y + 3)]) / 3;//original
			//heat[XY2(x,MATRIX_HEIGHT- y)] = (heat[XY2(x, MATRIX_HEIGHT- y - 1)] + heat[XY2(x, MATRIX_HEIGHT- y - 2)] + heat[XY2(x, MATRIX_HEIGHT- y - 3)]) / 3;
		}

		// Step 2.  Randomly ignite new 'sparks' of heat
		if (random8() < sparking) {
			// int x = (p[0] + p[1] + p[2]) / 3;

			int xy = XY2(x, MATRIX_HEIGHT - 1);// original
			
			heat[xy] = qadd8(heat[xy], random8(160, 255));//random8(160, 255));
		}

		// Step 4.  Map from heat cells to LED colors
		for (int y = 0; y < MATRIX_HEIGHT; y++) {
			int xy = XY2(x, MATRIX_HEIGHT- y); // debo invertir la matriz de pallete
			byte colorIndex = heat[xy];

			// Recommend that you use values 0-240 rather than
			// the usual 0-255, as the last 15 colors will be
			// 'wrapping around' from the hot end to the cold end,
			// which looks wrong.
			colorIndex = scale8(colorIndex, 240); // colorIndex  DEBUG !!

			// override color 0 to ensure a black background?
			if (colorIndex != 0)
				//                    effects.leds[xy] = CRGB::Black;
				//                else
				leds2[xy] = ColorFromPalette(HeatColors_p, colorIndex, 255, LINEARBLEND);// ColorFromCurrentPalette(colorIndex, 255, LINEARBLEND);
			//  ColorFromCurrentPalette(uint8_t index = 0, uint8_t brightness = 255, TBlendType blendType = LINEARBLEND)
			c_leds(x, y) =  leds2[xy];
			//c_leds(x, 0) = CRGB(255, 0, 0);// DEBUG
		}
	}
	//FastLED.show();
	// Noise smearing
	noise_x[0] += 1000;
	noise_y[0] += 1000;
	noise_z[0] +=1000;//1000
	noise_scale_x[0] = 4000;
	noise_scale_y[0] = 4000;
	FillNoiseFire(0);

	MoveX(3);
	//MoveFractionalNoiseX(4);
	FastLED.show();
	FastLED.delay(3);
	
};

// Show the current palette.
void video(byte nada, byte nada1) {
	//void ShowPalette() {
	for (int i = 0; i < 480; i++) {
		byte color = i / (NUM_LEDS / 256);
		byte bri = 255;
		c_leds(i)= ColorFromPalette(HeatColors_p, color, bri);
	}
	FastLED.show();
}