/* This is currently a (working) mess copied together from animation examples over the web
  and I will try to credit everyone and clean it up over time - if you see code here that you wrote
  and you want to be credited, please let me know - I do not wish to cause offense. */

// declaracion de funciones 

//uint16_t  XY( uint8_t x, uint8_t y); // borrar
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


void one_color_allHSV(int ahue, int abright) {                // SET ALL LEDS TO ONE COLOR (HSV)
  
    //   leds[i] = CHSV(ahue, 254, abright);

    ledSet(0,CUSTOM_NUM_LEDS)= CHSV(ahue, 254, abright);
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
      leds2[l] = CHSV(currentBg, 255, 50);         // strip.setPixelColor(l, Wheel(currentBg, 0.1));
    }

    if (step == -1) {
      center = random(CUSTOM_NUM_LEDS);
      color = random(256);
      step = 0;
    }

    if (step == 0) {
      leds2[center] = CHSV(color, 255, 255);         // strip.setPixelColor(center, Wheel(color, 1));
      step ++;
    }
    else {
      if (step < maxSteps) {
        //Serial.println(pow(fadeRate,step));

        leds2[wrap(center + step)] = CHSV(color, 255, pow(fadeRate, step) * 255);     //   strip.setPixelColor(wrap(center + step), Wheel(color, pow(fadeRate, step)));
        leds2[wrap(center - step)] = CHSV(color, 255, pow(fadeRate, step) * 255);     //   strip.setPixelColor(wrap(center - step), Wheel(color, pow(fadeRate, step)));
        if (step > 3) {
          leds2[wrap(center + step - 3)] = CHSV(color, 255, pow(fadeRate, step - 2) * 255);   //   strip.setPixelColor(wrap(center + step - 3), Wheel(color, pow(fadeRate, step - 2)));
          leds2[wrap(center - step + 3)] = CHSV(color, 255, pow(fadeRate, step - 2) * 255);   //   strip.setPixelColor(wrap(center - step + 3), Wheel(color, pow(fadeRate, step - 2)));
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
      leds2[pixelnumber] = color;
    }
    FastLED.show(); // display this frame
  }
}
//Fire2012 End
//CYLON START


void fadeall() {
  for (int i = 0; i < CUSTOM_NUM_LEDS; i++) {
    leds2[i].nscale8(200);
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
          leds2[i] = CHSV(hue++, 255, 255);
          // Show the leds2
          FastLED.show();
          // now that we've shown the leds, reset the i'th led to black
          // leds2[i] = CRGB::Black;
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
          leds2[(i--) - 1] = CHSV(hue++, 255, 255);
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
void fillnoise8a() {
  // If we're runing at a low "speed", some 8-bit artifacts become visible
  // from frame-to-frame.  In order to reduce this, we can do some fast data-smoothing.
  // The amount of data smoothing we're doing depends on "speed".
  uint8_t dataSmoothing = 0;
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
      leds2[XY2(i,j)] = color;
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
    leds2[i] = ColorFromPalette( currentPalette, colorIndex + sin8(i*16), brightness);
    colorIndex += 3;
  }
}

void LedsNoise ()
  {

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
  fillnoise8a();
    // convert the noise data to colors in the LED array
  
  // using the current palette
  mapNoiseToLEDsUsingPalette();
  }

