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

void video(byte nada1 , byte nada2) {
	// matrix gests updated on the heroku websocket callback
	// verify is connected to server 
	//if (!isConnectedH )
	//{
		//herokuWs.begin(herokuHost, herokuport);
	//}
	FastLED.show(); // display this frame
}
void one_color_allHSV(byte ahue, byte abright) {                // SET ALL LEDS TO ONE COLOR (HSV)
	//ahue = myHue;
	//abright = myBrightness;
	//FastLED.clear();
	//CHSV pp = rgb2hsv_approximate(CRGB(red, green, blue));
	//fill_solid(c_leds[0], CUSTOM_NUM_LEDS /*number of leds*/, CHSV(ahue, mySaturation, abright));
	//fill_solid(c_leds[0], CUSTOM_NUM_LEDS /*number of leds*/,CRGB(realRed,realGreen,realBlue));
	FastLED.showColor( CRGB(realRed, realGreen, realBlue));
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
    } else {
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
      step ++;
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
        step ++;
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
    for ( int i = 0; i < CUSTOM_NUM_LEDS; i++) {
      heat[i] = qsub8( heat[i],  random8(0, ((COOLING * 10) / CUSTOM_NUM_LEDS) + 2));
    }

    // Step 2.  Heat from each cell drifts 'up' and diffuses a little
    for ( int k = CUSTOM_NUM_LEDS - 1; k >= 2; k--) {
      heat[k] = (heat[k - 1] + heat[k - 2] + heat[k - 2] ) / 3;
    }

    // Step 3.  Randomly ignite new 'sparks' of heat near the bottom
    if ( random8() < SPARKING ) {
      int y = random8(7);
      heat[y] = qadd8( heat[y], random8(160, 255) );
    }

    // Step 4.  Map from heat cells to LED colors
    for ( int j = 0; j < CUSTOM_NUM_LEDS; j++) {
      CRGB color = HeatColor( heat[j]);
      int pixelnumber;
      if ( gReverseDirection ) {
        pixelnumber = (CUSTOM_NUM_LEDS - 1) - j;
      } else {
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
void fillnoise8a(byte nada , byte nada1) {
  // If we're runing at a low "speed", some 8-bit artifacts become visible
  // from frame-to-frame.  In order to reduce this, we can do some fast data-smoothing.
  // The amount of data smoothing we're doing depends on "speed".
  uint8_t dataSmoothing = 0;
  speed = qadd8(0,myparameter1);

  if( speed < 50) {
    dataSmoothing = 200 - (speed * 4);
  }
  
  for(int i = 0; i < MAX_DIMENSION; i++) {
    int ioffset = scale * i;
    for(int j = 0; j < MAX_DIMENSION; j++) {
      int joffset = scale * j;
      
      uint8_t data = inoise8(x + ioffset,y + joffset,z);

      // The range of the inoise8 function is roughly 16-238.
      // These two operations expand those values out to roughly 0..255
      // You can comment them out if you want the raw noise data.
      data = qsub8(data,16);
      data = qadd8(data,scale8(data,39));

      if( dataSmoothing ) {
        uint8_t olddata = noise[i][j];
        uint8_t newdata = scale8( olddata, dataSmoothing) + scale8( data, 256 - dataSmoothing);
        data = newdata;
      }
      
      noise[i][j] = data;
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
  static uint8_t ihue=0;
  
  for(int i = 0; i < CUSTOM_WIDTH; i++) {
    for(int j = 0; j < CUSTOM_HEIGHT; j++) {
      // We use the value at the (i,j) coordinate in the noise
      // array for our brightness, and the flipped value from (j,i)
      // for our pixel's index into the color palette.

      uint8_t index = noise[j][i];
      uint8_t bri =   noise[i][j];
	 
      // if this palette is a 'loop', add a slowly-changing base value
      if( colorLoop) { 
        index += ihue;
      }

      // brighten up, as the color palette itself often contains the 
      // light/dark dynamic range desired
      if( bri > 127 ) {
        bri = 255;
      } else {
        bri = dim8_raw( bri * 2);
      }

      CRGB color = ColorFromPalette( currentPalette, index, bri);
	  //c_leds(XY2(i,j)) = color;
	  c_leds(i, j) = color;
    }
  }
  
  ihue+=1;
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
  nblendPaletteTowardPalette( currentPalette, targetPalette, maxChanges);
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
      case  7: targetPalette = CloudColors_p; speed =  4; scale = 20; colorLoop = 0; break;
      case  8: targetPalette = LavaColors_p;  speed =  8; scale = 19; colorLoop = 0; break;
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
void all_off() {  fill_solid( targetPalette, 16, CRGB::Black);}
void SetupRandomPalette()//two colors
{
  EVERY_N_MILLISECONDS( 8000 ){ //new random palette every 8 seconds. Might have to wait for the first one to show up
  CRGB black  = CRGB::Black;
  CRGB random1 = CHSV( random8(), 255, 255);
  CRGB random2 = CHSV( random8(), 255, 255);
  targetPalette = CRGBPalette16( 
//                      CRGB( random8(), 255, 32), 
//                      CHSV( random8(), 255, 255),
                      random1,random1,black, black, 
                      random2,random2,black, black,
                      random1,random1,black, black, 
                      random2,random2,black, black);
//                      CHSV( random8(), 128, 255), 
//                      CHSV( random8(), 255, 255), );
}
}
void SetupRandomPalette_g()//three colors
{
  EVERY_N_MILLISECONDS( 8000 ){ //new random palette every 8 seconds
  CRGB black  = CRGB::Black;
  CRGB random1 = CHSV( random8(), 255, 255);
  CRGB random2 = CHSV( random8(), 200, 100);
  CRGB random3 = CHSV( random8(), 150, 200);
  targetPalette = CRGBPalette16( 
//                      CRGB( random8(), 255, 32), 
//                      CHSV( random8(), 255, 255),
                      random1,random1,black, black, 
                      random2,random2,black, random3,
                      random1,random1,black, black, 
                      random2,random2,black, random3);
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
  fill_solid( targetPalette, 16, CRGB::Black);
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
void FillLEDsFromPaletteColors( uint8_t colorIndex)
{
  uint8_t brightness = 255;
  
  for( int i = 0; i < CUSTOM_NUM_LEDS; i++) {
    c_leds(i) = ColorFromPalette( currentPalette, colorIndex + sin8(i*16), brightness);
    colorIndex += 3;
  }
}

void LedsNoise (byte nada,byte nada1)
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
  fillnoise8a(0,0);

  
    // convert the noise data to colors in the LED array
  
  // using the current palette
  mapNoiseToLEDsUsingPalette();
  LEDS.show();
  
  }

// animaciones pasadas desde funciones


// 2 oscillators flying arround one ;)
void Dots1(uint8_t color1, uint8_t color2) {
	MoveOscillators();
	//2 lissajous dots red
	byte c1 = color1;
	byte c2 = color2;
	if (myparameter1 > 2) { c1 = myparameter1; c2 = 255 - c1; }
	leds[XY(p[0], p[1])] = CHSV(c1, 255, 255);
	leds[XY(p[2], p[3])] = CHSV(c2, 255, 150);
	//average of the coordinates in yellow
	Pixel((p[2] + p[0]) / 2, (p[1] + p[3]) / 2, 50);
	ShowFrame();

	FastLED.delay(20);
	HorizontalStream(myparameter1); // antes scale
	
}

// x and y based on 3 sine waves
void Dots2(uint8_t scale, uint8_t nada) {
	MoveOscillators();
	Pixel((p[2] + p[0] + p[1]) / 3, (p[1] + p[3] + p[2]) / 3, osci[3]);
	ShowFrame();
	FastLED.delay(20);
	HorizontalStream(myparameter1); // anstes scale
}

// beautifull but periodic
void SlowMandala2(byte dim, byte nada) {
	for (int i = 1; i < 8; i++) {
		for (int j = 0; j < 16; j++) {
			MoveOscillators();
			Pixel(j, i, (osci[0] + osci[1])/2); // max (15+15)*8
			SpiralStream(4, 4, 4, dim); // antes dim
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
			Pixel(j, j, (osci[0] + osci[1])/2);
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
	HorizontalStream(myparameter1); //antes dim
	ShowFrame();
}

// colorfull 2 chanel 7 band analyzer
void MSGEQtest(byte scale, byte nada) {
	ReadAudio();
	for (int i = 0; i < 7; i++) {
		Pixel(i, 16 - left[i] / 64, left[i] / 4);
	}
	for (int i = 0; i < 7; i++) {
		Pixel(8 + i, 16 - right[i] / 64, right[i] / 4);
	}
	ShowFrame();
	VerticalStream(scale);
}

// 2 frequencies linked to dot emitters in a spiral mandala
void MSGEQtest2(byte scale, byte color) {
	ReadAudio();
	if (left[0]>left_offset[0]) {
		PixelHsv(0, 0, CHSV(myparameter1,mySaturation,left[1])); // color 1
		PixelHsv(1, 1, CHSV(myparameter1, mySaturation, left[1])); // color 1 
	}
	if (left[2]>left_offset[2]) {
		PixelHsv(2, 2, CHSV(myHue, mySaturation, left[2]));// color 100 , mysaturation used as color
	}
	if (left[6]>left_offset[6]) {
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
	SpiralStream(7, 7, 7, myParameter2 );// color1 160
	SpiralStream(4, 4, 4, myHue);
	SpiralStream(11, 11, 3,myParameter3);// color2 110
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
		BresenhamLine(2 * i, 16 - left[i]*(left[i]>left_offset[i]) / 64, 2 * i, 15, i*hmult); // hmult
		BresenhamLine(1 + 2 * i, 16 - left[i] * (left[i]>left_offset[i]) / 64, 1 + 2 * i, 15, i*hmult); // hmult
	}
	ShowFrame();
	myParameter2 = dim*(myMode)+!(myMode)*myValue; // 124
	SpiralStream(7, 7, 7, myParameter2); // dim 120
}

// classic analyzer, slow falldown
void MSGEQtest6(byte dim, byte hmult) {
	ReadAudio();
	for (int i = 0; i < 7; i++) {
		BresenhamLine(2 * i, 16 - left[i] * (left[i]>left_offset[i]) / 64, 2 * i, 15, i*hmult);
		BresenhamLine(1 + 2 * i, 16 - left[i] * (left[i]>left_offset[i]) / 64, 1 + 2 * i, 15, i*hmult);
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
		PixelHsv(7 - i, 8 - left[i] * (left[i]>left_offset[i]) / 128,CHSV( i*hmult + left[1] * (left[1]>left_offset[1]) / 8,mySaturation,myValue+left[2]));
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
		Pixel(7 - i, 8 - right[i] / 128, i*hmult + osci[1]);
	}
	Caleidoscope5();
	Caleidoscope2();
	ShowFrame();
	DimAll(dim);
}

// falling spectogram
void MSGEQtest9(byte hmult, byte s) {
	ReadAudio();
	for (int i = 0; i < 7; i++) {
		leds[XY(i * 2, 0)] = CHSV(i*hmult, s, right[i] / 3); // brightness should be divided by 4
		leds[XY(1 + i * 2, 0)] = CHSV(i*hmult, s, left[i] / 3);
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
		BresenhamLine(i, 4 - left[i] / 256, i, 4, i*hmult);
	}
	Copy(0, 0, 4, 4, 5, 0);
	Copy(0, 0, 4, 4, 10, 0);
	Copy(0, 0, 14, 4, 0, 5);
	Copy(0, 0, 14, 4, 0, 10);
	ShowFrame();
	DimAll(dim);
}

// test scale
// NOT WORKING as intended YET!
void CopyTest2(byte dim, byte hmult) {
	ReadAudio();
	for (int i = 0; i < 5; i++) {
		BresenhamLine(i * 2, 4 - left[i] / 128, i * 2, 4, i*hmult);
	}
	Scale(0, 0, 4, 4, 7, 7, 15, 15);
	ShowFrame();
	DimAll(dim);

}

// line spectogram mandala
void Audio1(byte dim, byte hmult) {
	ReadAudio();
	for (int i = 0; i < 5; i++) {
		BresenhamLine(3 * i, 16 - left[i] / 64, 3 * (i + 1), 16 - left[i + 1] / 64, 255 - i*hmult);
	}
	Caleidoscope4();
	ShowFrame();
	DimAll(dim);
}

// line analyzer with stream
void Audio2(byte dim, byte hmult) {
	ReadAudio();
	for (int i = 0; i < 5; i++) {
		BresenhamLine(3 * i, 16 - left[i] / 64, 3 * (i + 1), 16 - left[i + 1] / 64, 255 - i*hmult);
	}
	ShowFrame();
	HorizontalStream(dim);
}

void Audio3(byte dim, byte hmult) {
	ReadAudio();
	for (int i = 0; i < 7; i++) {
		leds[XY(6 - i, right[i] / 128)] = CHSV(i*hmult, 255, right[i]);
	} // brightness should be divided by 4
	Caleidoscope6();
	Caleidoscope2();
	ShowFrame();
	DimAll(dim);
}

void Audio4(byte dim, byte hdiv) {
	ReadAudio();
	for (int i = 0; i < 5; i++) {
		BresenhamLine(3 * i, 8 - left[i] / 128, 3 * (i + 1), 8 - left[i + 1] / 128, i*left[i] / hdiv);
	}
	Caleidoscope4();
	ShowFrame();
	DimAll(dim);
}

void CaleidoTest1(byte dim, byte hdiv) {
	ReadAudio();
	for (int i = 0; i < 7; i++) {
		BresenhamLine(i, left[i] / 256, i, 0, left[i] / hdiv);
	}
	RotateTriangle();
	Caleidoscope2();  //copy + rotate
	ShowFrame();
	DimAll(dim);
}

void CaleidoTest2(byte dim, byte color_ofset) {
	MoveOscillators();
	ReadAudio();
	for (int i = 0; i < 7; i++) {
		BresenhamLine(i, left[i] / 200, i, 0, ((left[i] / 16) + myparameter1)%255); // antes color_offset
	}
	MirrorTriangle();
	Caleidoscope1();  //mirror + rotate
	ShowFrame();
	DimAll(dim);
}

void Audio5(byte dim, byte hmult) {
	ReadAudio();
	for (int i = 0; i < 5; i++) {
		BresenhamLine(
			3 * i, 8 - left[i] / 128,        // from
			3 * (i + 1), 8 - left[i + 1] / 128,  // to
			i * hmult);
	}                       // color
	Caleidoscope4();
	ShowFrame();
	DimAll(dim);
}

void Audio6(byte dim, byte hmult) {
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
	DimAll(dim);
	//ClearAll();
}

void NoiseExample1(byte hpow2, byte s) {
	MoveOscillators();
	scale2 = 30 + p[1] * 3;
	x = p[0] * 16;
	fillnoise8();
	fillnoise82();
	for (int i = 0; i < kMatrixWidth; i++) {
		for (int j = 0; j < kMatrixHeight; j++) {
			leds[XY(i, j)] =
				CHSV(noise[i][j] << hpow2, s, (noise2[i][j] + noise[i][j]) / 2);
		}
	}
	ShowFrame();
}

void NoiseExample2(byte noisez, byte scale) {
	MoveOscillators();
	FillNoise(2000 - p[2] * 100, 100, noisez, scale);
	for (int i = 0; i < p[2]; i++) {
		for (int j = 0; j < kMatrixHeight; j++) {
			leds[XY(i, j)] = CRGB(noise[i][j], 0, 0);
		}
	}
	for (int i = 0; i < p[1]; i++) {
		for (int j = 0; j < kMatrixHeight; j++) {
			leds[XY(j, i)] += CRGB(0, 0, noise[i][j]);
		}
	}
	ShowFrame();
	ClearAll();
}

void NoiseExample3(byte noisez, byte scale) {
	MoveOscillators();
	FillNoise(p[1] * 100, p[2] * 100, noisez, scale);
	for (int i = 0; i < p[1]; i++) {
		for (int j = 0; j < kMatrixHeight; j++) {
			leds[XY(i, j)] = CHSV(noise[i][j], 255, 200);
		}
	}

	for (int i = 0; i < p[3]; i++) {
		for (int j = 0; j < kMatrixHeight; j++) {
			leds[XY(j, i)] += CHSV(128 + noise[i][j], 255, 200);
		}
	}

	ShowFrame();
	ClearAll();
}

void NoiseExample4(byte noisez, byte scale) {
	MoveOscillators();
	FillNoise(100, 100, noisez, scale);
	for (int i = 0; i < p[0] + 1; i++) {
		for (int j = 0; j < kMatrixHeight; j++) {
			leds[XY(i, j)] += CHSV(noise[i][j + p[2]], 255, 255);
		}
	}
	ShowFrame();
	ClearAll();
}

void NoiseExample5(byte noisez, byte scale) {


	MoveOscillators();
	ReadAudio();
	FillNoise(100, 100, noisez, scale);

	for (int i = 0; i < kMatrixWidth; i++) {
		for (int j = 0; j < left[1] / 64; j++) {
			leds[XY(i, 15 - j)] = CRGB(0, noise[i][left[1] / 64 - j], 0);
		}
	}

	for (int i = 0; i < kMatrixWidth; i++) {
		for (int j = 0; j < left[5] / 64; j++) {
			leds[XY(j, i)] += CRGB(noise[i][left[5] / 64 - j], 0, 0);
		}
	}
	ShowFrame();
	ClearAll();
}

void NoiseExample6(byte hofset, byte scale) {
	//MoveOscillators();
	for (int size = 1; size < scale; size++) {
		z++;


		FillNoise(size, size, z, size);

		for (int i = 0; i < kMatrixWidth; i++) {

			for (int j = 0; j < kMatrixHeight; j++) {
				yield();

				leds[XY(i, j)] = CHSV(hofset + noise[i][j], 255, 255);
			}
		}


		ShowFrame();
		//ClearAll();
	}
	for (int size = 200; size > 1; size--) {
		z++;
		yield();
		FillNoise(size, size, z, size);
		for (int i = 0; i < kMatrixWidth; i++) {
			for (int j = 0; j < kMatrixHeight; j++) {
				leds[XY(i, j)] = CHSV(50 + noise[i][j], 255, 255);
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
				leds[XY(i, j)] = ColorFromPalette(currentPalette, noise[i][j], 255, currentBlending);
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
				leds[XY(i, j)] = ColorFromPalette(currentPalette, noise[i][j], 255, currentBlending);
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
	for (uint8 y = 0; y<c_leds.Height(); ++y)
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
	for (uint8 y = 0; y<c_leds.Height(); ++y)
	{

		c_leds.DrawCircle(10, 12, y, CHSV(h, 255, 250));

		h += 16;
	}
	blur2d(c_leds[0], MATRIX_WIDTH, MATRIX_HEIGHT, 16);// blur the individual circles
	hue += 4;
	FastLED.show();
	fadeToBlackBy(c_leds[0], 480, 16);
}

void circulo1(byte nada,byte nada1) {
	cir1.Update();
	fadeToBlackBy(c_leds[0], 480, 32);
	blur2d((c_leds[0]), MATRIX_WIDTH, MATRIX_HEIGHT, 32);
}

void HorMirror(byte nada, byte nada1) {
	HorizontalStripes(0,0);
	hue += 4;
	c_leds.HorizontalMirror();
	FastLED.show();
	fadeToBlackBy(c_leds[0], 480, 16);
};

void VertMirror(byte nada, byte nada1) {
	HorizontalStripes(0,0);
	hue += 4;
	c_leds.VerticalMirror();
	FastLED.show();
	fadeToBlackBy(c_leds[0], 480, 16);
};

void QuadMirror(byte nada, byte nada1) {
	HorizontalStripes(0,0);
	hue += 4;
	c_leds.QuadrantMirror();
	FastLED.show();
	fadeToBlackBy(c_leds[0], 480, 16);
};

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




