
/*
-------------------------------------------------------------------
Basic Helper functions:

XY                  translate 2 dimensional coordinates into an index
Line                draw a line
Pixel               draw a pixel
ClearAll            empty the screenbuffer
MoveOscillators     increment osci[] and calculate p[]=sin8(osci)
InitMSGEQ7          activate the MSGEQ7
ReadAudio           get data from MSGEQ7 into left[7] and right[7]

-------------------------------------------------------------------
*/

// translates from x, y into an index into the LED array and
// finds the right index for a S shaped matrix
// para la matriz de height width

int XY(int x, int y) {
	if (y > HEIGHT) {
		y = HEIGHT;
	}
	if (y < 0) {
		y = 0;
	}
	if (x > WIDTH) {
		x = WIDTH;
	}
	if (x < 0) {
		x = 0;
	}
	// for a serpentine layout reverse every 2nd column:
	{
		uint16_t i;

		if (kMatrixSerpentineLayout == false) {
			i = (x * HEIGHT) + y;
		}

		if (kMatrixSerpentineLayout == true) {
			if (x & 0x01) {
				// Odd rows run backwards
				uint8_t reverseY = (HEIGHT - 1) - y;
				i = (x * HEIGHT) + reverseY;
			}
			else {
				// Even rows run forwards
				i = (x * HEIGHT) + y;
			}
		}

		return i;
	}
}

// translates from x, y into an index into the LED array and
// finds the right index for a S shaped matrix
// para la matriz custom

int XY2(int x, int y) {
	if (y > CUSTOM_HEIGHT) {
		y = CUSTOM_HEIGHT;
	}
	if (y < 0) {
		y = 0;
	}
	if (x > CUSTOM_WIDTH) {
		x = CUSTOM_WIDTH;
	}
	if (x < 0) {
		x = 0;
	}
	// for a serpentine layout reverse every 2nd row:
	{
		uint16_t i;

		if (kMatrixSerpentineLayout == false) {
			i = (x * CUSTOM_HEIGHT) + y;
		}

		if (kMatrixSerpentineLayout == true) {
			if (x & 0x01) {
				// Odd columns run backwards
				uint8_t reverseY = (CUSTOM_HEIGHT - 1) - y;
				i = (x * CUSTOM_HEIGHT) + reverseY;
			}
			else {
				// Even rows run forwards
				i = (x * CUSTOM_HEIGHT) + y;
			}
		}

		return i;
	}
}

//*************************************************
// Bresenham line algorythm based on 2 coordinates
void Line(int x0, int y0, int x1, int y1, byte color) {
	int dx = abs(x1 - x0), sx = x0 < x1 ? 1 : -1;
	int dy = -abs(y1 - y0), sy = y0 < y1 ? 1 : -1;
	int err = dx + dy, e2;
	for (;;) {
		leds[XY(x0, y0)] = CHSV(color, 255, 255);
		if (x0 == x1 && y0 == y1) break;
		e2 = 2 * err;
		if (e2 > dy) {
			err += dy;
			x0 += sx;
		}
		if (e2 < dx) {
			err += dx;
			y0 += sy;
		}
	}
}

// write one pixel with HSV color to coordinates
void Pixel(int x, int y, byte color) {
	leds[XY(x, y)] = CHSV(color, 255, 255);
}

// delete the screenbuffer
void ClearAll()
{
	for (int i = 0; i < NUM_LEDS; i++) {
		leds[i] = 0;
	}
}

/*
Oscillators and Emitters
*/

// set the speeds (and by that ratios) of the oscillators here
void MoveOscillators() {
	osci[0] = osci[0] + 5;
	osci[1] = osci[1] + 2;
	osci[2] = osci[2] + 3;
	osci[3] = osci[3] + 4;
	for (int i = 0; i < 4; i++) {
		p[i] = sin8(osci[i]) / 17;  //why 17? to keep the result in the range of 0-15 (matrix size)
	}
}

// wake up the MSGEQ7
void InitMSGEQ7() {
	pinMode(MSGEQ7_RESET_PIN, OUTPUT);
	pinMode(MSGEQ7_STROBE_PIN, OUTPUT);
	digitalWrite(MSGEQ7_RESET_PIN, LOW);
	digitalWrite(MSGEQ7_STROBE_PIN, HIGH);
}

// get the data from the MSGEQ7
// (still fucking slow...)
void ReadAudio() {
	digitalWrite(MSGEQ7_RESET_PIN, HIGH);
	digitalWrite(MSGEQ7_RESET_PIN, LOW);
	for (byte band = 0; band < 7; band++) {
		digitalWrite(MSGEQ7_STROBE_PIN, LOW);
		delayMicroseconds(30);
		left[band] = analogRead(AUDIO_LEFT_PIN);
		right[band] = analogRead(AUDIO_RIGHT_PIN);
		digitalWrite(MSGEQ7_STROBE_PIN, HIGH);
	}
}

/*
-------------------------------------------------------------------
Functions for manipulating existing data within the screenbuffer:

DimAll           scales the brightness of the screenbuffer down
Caleidoscope1    mirror one quarter to the other 3 (and overwrite them)
Caleidoscope2    rotate one quarter to the other 3 (and overwrite them)
Caleidoscope3    useless bullshit?!
Caleidoscope4    rotate and add the complete screenbuffer 3 times
Caleidoscope5    copy a triangle from the first quadrant to the other half
Caleidoscope6
SpiralStream     stream = give it a nice fading tail
HorizontalStream
VerticalStream
VerticalMove     move = just move it as it is one line down
Copy             copy a rectangle
RotateTriangle   copy + rotate a triangle (in 8*8)
MirrorTriangle   copy + mirror a triangle (in 8*8)
RainbowTriangle  static draw for debugging

-------------------------------------------------------------------
*/

// scale the brightness of the screenbuffer down
void DimAll(byte value)
{
	for (int i = 0; i < NUM_LEDS; i++) {
		leds[i].nscale8(value);
	}
}

// create a square twister
// x and y for center, r for radius
void SpiralStream(int x, int y, int r, byte dim) {
	for (int d = r; d >= 0; d--) {                // from the outside to the inside
		for (int i = x - d; i <= x + d; i++) {
			leds[XY(i, y - d)] += leds[XY(i + 1, y - d)];   // lowest row to the right
			leds[XY(i, y - d)].nscale8(dim);
		}
		for (int i = y - d; i <= y + d; i++) {
			leds[XY(x + d, i)] += leds[XY(x + d, i + 1)];   // right colum up
			leds[XY(x + d, i)].nscale8(dim);
		}
		for (int i = x + d; i >= x - d; i--) {
			leds[XY(i, y + d)] += leds[XY(i - 1, y + d)];   // upper row to the left
			leds[XY(i, y + d)].nscale8(dim);
		}
		for (int i = y + d; i >= y - d; i--) {
			leds[XY(x - d, i)] += leds[XY(x - d, i - 1)];   // left colum down
			leds[XY(x - d, i)].nscale8(dim);
		}
	}
}

// give it a linear tail to the side
void HorizontalStream(byte scale)
{
	for (int x = 1; x < WIDTH; x++) {
		for (int y = 0; y < HEIGHT; y++) {
			leds[XY(x, y)] += leds[XY(x - 1, y)];
			leds[XY(x, y)].nscale8(scale);
		}
	}
	for (int y = 0; y < HEIGHT; y++)
		leds[XY(0, y)].nscale8(scale);
}

// give it a linear tail downwards
void VerticalStream(byte scale)
{
	for (int x = 0; x < WIDTH; x++) {
		for (int y = 1; y < HEIGHT; y++) {
			leds[XY(x, y)] += leds[XY(x, y - 1)];
			leds[XY(x, y)].nscale8(scale);
		}
	}
	for (int x = 0; x < WIDTH; x++)
		leds[XY(x, 0)].nscale8(scale);
}

// just move everything one line down
void VerticalMove() {
	for (int y = 15; y > 0; y--) {
		for (int x = 0; x < 16; x++) {
			leds[XY(x, y)] = leds[XY(x, y - 1)];
		}
	}
}

// copy the rectangle defined with 2 points x0, y0, x1, y1
// to the rectangle beginning at x2, x3
void Copy(byte x0, byte y0, byte x1, byte y1, byte x2, byte y2) {
	for (int y = y0; y < y1 + 1; y++) {
		for (int x = x0; x < x1 + 1; x++) {
			leds[XY(x + x2 - x0, y + y2 - y0)] = leds[XY(x, y)];
		}
	}
}

// rotate + copy triangle (8*8)
void RotateTriangle() {
	for (int x = 1; x < 8; x++) {
		for (int y = 0; y<x; y++) {
			leds[XY(x, 7 - y)] = leds[XY(7 - x, y)];
		}
	}
}

// mirror + copy triangle (8*8)
void MirrorTriangle() {
	for (int x = 1; x < 8; x++) {
		for (int y = 0; y<x; y++) {
			leds[XY(7 - y, x)] = leds[XY(7 - x, y)];
		}
	}
}

// draw static rainbow triangle pattern (8x8)
// (just for debugging)
void RainbowTriangle() {
	for (int i = 0; i < 8; i++) {
		for (int j = 0; j <= i; j++) {
			Pixel(7 - i, j, i*j * 4);
		}
	}
}

/*
-------------------------------------------------------------------
Examples how to combine functions in order to create an effect

...or: how to visualize some of the following data
osci[0] ... osci[3]   (0-255) triangle
p[0] ... p[3]         (0-15)  sinus
left[0] ... left[6]   (0-1023) values of 63Hz, 160Hz, ...
right[0] ... right[6] (0-1023)

effects based only on oscillators (triangle/sine waves)

AutoRun        shows everything that follows
SlowMandala    red slow
Dots1          2 arround one
Dots2          stacking sines
SlowMandala2   just nice and soft
SlowMandala3   just nice and soft
Mandala8       copy one triangle all over

effects based on audio data (could be also linked to oscillators)

MSGEQtest      colorfull 2 chanel 7 band analyzer
MSGEQtest2     2 frequencies linked to dot emitters in a spiral mandala
MSGEQtest3     analyzer 2 bars
MSGEQtest4     analyzer x 4 (as showed on youtube)
AudioSpiral    basedrum/snare linked to red/green emitters
MSGEQtest5     one channel 7 band spectrum analyzer (spiral fadeout)
MSGEQtest6     classic analyzer, slow falldown
MSGEQtest7     spectrum mandala, color linked to low frequencies
MSGEQtest8     spectrum mandala, color linked to osci
MSGEQtest9     falling spectogram
CopyTest
Audio1
Audio2
Audio3
Audio4
CaleidoTest1
Caleidotest2
Audio5
Audio6
-------------------------------------------------------------------
*/

// all examples together
void AutoRun() {

	// all oscillator based:
	/* Serial.println("dots1");
	for(int i = 0; i < 300; i++) {Dots1();}
	for(int i = 0; i < 300; i++) {Dots2();}
	Serial.println("slowmandala");
	SlowMandala();
	Serial.println("slowmandala2");
	SlowMandala2();
	Serial.println("slowmandala3");
	SlowMandala3();
	Serial.println("Mandala8");
	for(int i = 0; i < 300; i++) {Mandala8();}

	// all MSGEQ7 based:
	Serial.println("noise ");
	for(int i = 0; i < 500; i++) {MSGEQtest();}
	for(int i = 0; i < 500; i++) {MSGEQtest2();}
	for(int i = 0; i < 500; i++) {MSGEQtest3();}
	for(int i = 0; i < 500; i++) {MSGEQtest4();}
	for(int i = 0; i < 500; i++) {AudioSpiral();}
	for(int i = 0; i < 500; i++) {MSGEQtest5();}
	for(int i = 0; i < 500; i++) {MSGEQtest6();}
	for(int i = 0; i < 500; i++) {MSGEQtest7();}
	for(int i = 0; i < 500; i++) {MSGEQtest8();}
	for(int i = 0; i < 500; i++) {MSGEQtest9();}
	for(int i = 0; i < 500; i++) {CopyTest();}
	for(int i = 0; i < 500; i++) {Audio1();}
	for(int i = 0; i < 500; i++) {Audio2();}
	for(int i = 0; i < 500; i++) {Audio3();}
	for(int i = 0; i < 500; i++) {Audio4();}
	for(int i = 0; i < 500; i++) {CaleidoTest1();}
	for(int i = 0; i < 500; i++) {CaleidoTest2();}
	for(int i = 0; i < 500; i++) {Audio5();}
	for(int i = 0; i < 500; i++) {Audio6();}

	for(int i = 0; i < 500; i++) {
	NoiseExample1();
	}
	for(int i = 0; i < 500; i++) {
	NoiseExample2();
	}
	for(int i = 0; i < 500; i++) {
	NoiseExample3();
	}
	//SpeedTest();
	for(int i = 0; i < 500; i++) {
	NoiseExample4();
	}
	Serial.println("noise5 ");
	for(int i = 0; i < 500; i++) {NoiseExample5();}
	Serial.println("noise6 ");
	NoiseExample6();
	Serial.println("noise7 ");
	NoiseExample7();

	*/ //reemplazado por las funciones de patterns
}

// red, 4 spirals, one dot emitter
// demonstrates SpiralStream and Caleidoscope
// (psychedelic)
void SlowMandala() {
	for (int i = 0; i < 16; i++) {
		for (int j = 0; j < 16; j++) {
			Pixel(i, j, 1);
			SpiralStream(4, 4, 4, 127);
			Caleidoscope1();
			ShowFrame();
			FastLED.delay(50);
		}
	}
}


/*
Caleidoscope1 mirrors from source to A, B and C

y

|       |
|   B   |   C
|_______________
|       |
|source |   A
|_______________ x

*/
void Caleidoscope1() {
	for (int x = 0; x < WIDTH / 2; x++) {
		for (int y = 0; y < HEIGHT / 2; y++) {
			leds[XY(WIDTH - 1 - x, y)] = leds[XY(x, y)];              // copy to A
			leds[XY(x, HEIGHT - 1 - y)] = leds[XY(x, y)];             // copy to B
			leds[XY(WIDTH - 1 - x, HEIGHT - 1 - y)] = leds[XY(x, y)]; // copy to C
		}
	}
}

/*
Caleidoscope2 rotates from source to A, B and C

y

|       |
|   C   |   B
|_______________
|       |
|source |   A
|_______________ x

*/
void Caleidoscope2() {
	for (int x = 0; x < WIDTH / 2; x++) {
		for (int y = 0; y < HEIGHT / 2; y++) {
			leds[XY(WIDTH - 1 - x, y)] = leds[XY(y, x)];    // rotate to A
			leds[XY(WIDTH - 1 - x, HEIGHT - 1 - y)] = leds[XY(x, y)];    // rotate to B
			leds[XY(x, HEIGHT - 1 - y)] = leds[XY(y, x)];    // rotate to C
		}
	}
}

// adds the color of one quarter to the other 3
void Caleidoscope3() {
	for (int x = 0; x < WIDTH / 2; x++) {
		for (int y = 0; y < HEIGHT / 2; y++) {
			leds[XY(WIDTH - 1 - x, y)] += leds[XY(y, x)];    // rotate to A
			leds[XY(WIDTH - 1 - x, HEIGHT - 1 - y)] += leds[XY(x, y)];    // rotate to B
			leds[XY(x, HEIGHT - 1 - y)] += leds[XY(y, x)];    // rotate to C
		}
	}
}

// add the complete screenbuffer 3 times while rotating
void Caleidoscope4() {
	for (int x = 0; x < WIDTH; x++) {
		for (int y = 0; y < HEIGHT; y++) {
			leds[XY(WIDTH - 1 - x, y)] += leds[XY(y, x)];    // rotate to A
			leds[XY(WIDTH - 1 - x, HEIGHT - 1 - y)] += leds[XY(x, y)];    // rotate to B
			leds[XY(x, HEIGHT - 1 - y)] += leds[XY(y, x)];    // rotate to C
		}
	}
}

// rotate, duplicate and copy over a triangle from first sector into the other half
// (crappy code)
void Caleidoscope5() {
	for (int x = 1; x < 8; x++) {
		leds[XY(7 - x, 7)] += leds[XY(x, 0)];
	} //a
	for (int x = 2; x < 8; x++) {
		leds[XY(7 - x, 6)] += leds[XY(x, 1)];
	} //b
	for (int x = 3; x < 8; x++) {
		leds[XY(7 - x, 5)] += leds[XY(x, 2)];
	} //c
	for (int x = 4; x < 8; x++) {
		leds[XY(7 - x, 4)] += leds[XY(x, 3)];
	} //d
	for (int x = 5; x < 8; x++) {
		leds[XY(7 - x, 3)] += leds[XY(x, 4)];
	} //e
	for (int x = 6; x < 8; x++) {
		leds[XY(7 - x, 2)] += leds[XY(x, 5)];
	} //f
	for (int x = 7; x < 8; x++) {
		leds[XY(7 - x, 1)] += leds[XY(x, 6)];
	} //g
}


void Caleidoscope6() {
	for (int x = 1; x < 8; x++) {
		leds[XY(7 - x, 7)] = leds[XY(x, 0)];
	} //a
	for (int x = 2; x < 8; x++) {
		leds[XY(7 - x, 6)] = leds[XY(x, 1)];
	} //b
	for (int x = 3; x < 8; x++) {
		leds[XY(7 - x, 5)] = leds[XY(x, 2)];
	} //c
	for (int x = 4; x < 8; x++) {
		leds[XY(7 - x, 4)] = leds[XY(x, 3)];
	} //d
	for (int x = 5; x < 8; x++) {
		leds[XY(7 - x, 3)] = leds[XY(x, 4)];
	} //e
	for (int x = 6; x < 8; x++) {
		leds[XY(7 - x, 2)] = leds[XY(x, 5)];
	} //f
	for (int x = 7; x < 8; x++) {
		leds[XY(7 - x, 1)] = leds[XY(x, 6)];
	} //g
}


// rechtangle 0-1 -> 2-3
// NOT WORKING as intended YET!
void Scale(int x0, int y0, int x1, int y1, int x2, int y2, int x3, int y3) {
	for (int y = y2; y < y3 + 1; y++) {
		for (int x = x2; x < x3 + 1; x++) {
			leds[XY(x, y)] = leds[XY(
				x0 + ((x * (x1 - x0)) / (x3 - x1)),
				y0 + ((y * (y1 - y0)) / (y3 - y1)))];
		}
	}
}



/*
-------------------------------------------------------------------
Testcode for mapping the 16*16 calculation buffer to your
custom matrix size
-------------------------------------------------------------------
CUSTOM_WIDTH
CUSTOM_HEIGHT
Fy = kMatrixHeight/CUSTOM_HEIGHT;
Fx = kMatrixWidth/CUSTOM_WIDTH;
*/
// describe your matrix layout here:
// P.S. If you use a 8*8 just remove the */ and /*
void RenderCustomMatrix() {
	/* conversion original reemplazada x resizePixels
	for(int x = 0; x < CUSTOM_WIDTH; x++) {
	for(int y = 0; y < CUSTOM_HEIGHT; y++) {
	// position in the custom array LO HAGO DE A DOS PUES POR CADA LED FISICO HAY DOS LEDS SIMULADOS

	leds2[XY2(x,y)]=leds[XY((x*kMatrixWidth)/CUSTOM_WIDTH,(y*kMatrixHeight)/CUSTOM_HEIGHT)]; // 2 is the relation between heith of logical and phisical matrix , y cada valor en la columna lo saco como promedio  de los 2 pixels verticales

	// para ,apear la matriz de 16*16 en una matriz de 30x8 , repito un punto del eje x
	// y en el eje y interpolo 2 filas generando el promedio de ambas
	// OJO esta conversion es solo valida desde una matrix de 16x16 a una de 30x8
	// falta hacerla generica

	}
	}
	*/
	//    resizePixels(leds, int w1, int h1, int w2, int h2) 
	resizePixels(leds, 16, 16, 20, 24); // hacer flexible 
	// Copy  led colors from leds[src .. src+9] to leds[dest .. dest+9]
	// memmove( &leds[dest], &leds[src], 10 * sizeof( CRGB) );

	memmove8(&leds2[0], &tempLed[0], 480 * sizeof(CRGB));
	// for(int i = 0; i < CUSTOM_NUM_LEDS; i++) {      
	//leds2[i]= PixelBuffer[i] ;
	// }
	//Serial.println(millis()); // debug
}

//*** Prueba de matriz 
void Prueba() {
	Serial.println(" brigtness ");
	// while(Serial.available()<=0){ Serial.print('.');
	// }  // send an ASCII . 
	// LEDS.showColor(CRGB(rawSerial, 0, 0));

	LEDS.showColor(CRGB(0, 255, 0));
	// delay(3000);
	//LEDS.showColor(CRGB(0, 0, 255));
	//delay(3000);
	for (int x = 0; x < CUSTOM_WIDTH; x++) {
		for (int y = 0; y < CUSTOM_HEIGHT; y++) {
			// position in the custom array LO HAGO DE A DOS PUES POR CADA LED FISICO HAY DOS LEDS SIMULADOS

			leds2[XY2(x, y)] = CRGB::White;
			FastLED.show();
			fadeToBlackBy(leds2, CUSTOM_NUM_LEDS, 20);

		}

	}

}

// showFrame ******************************************

void ShowFrame() {
	// when using a matrix different than 16*16 use 
	RenderCustomMatrix();

	FastLED.show();
	LEDS.countFPS();
	//SendToProcessing(); // envia los datos en serie a processing para visualizarlos


}



void fillnoise8() {
	for (int i = 0; i < MAX_DIMENSION; i++) {
		int ioffset = scale * i;
		for (int j = 0; j < MAX_DIMENSION; j++) {
			int joffset = scale * j;
			noise[i][j] = inoise8(x + ioffset, y + joffset, z);
		}
	}
	y += speed;
	//z += 2;
}


void fillnoise82() {
	for (int i = 0; i < MAX_DIMENSION; i++) {
		int ioffset = scale2 * i;
		for (int j = 0; j < MAX_DIMENSION; j++) {
			int joffset = scale2 * j;
			noise2[i][j] = inoise8(x2 + ioffset, y2 + joffset, z2);
		}
	}
	y2 += speed * 3;
	//z += 2;
}

void FillNoise(uint16_t x, uint16_t y, uint16_t z, uint16_t scale) {
	for (int i = 0; i < MAX_DIMENSION; i++) {
		int ioffset = scale * i;
		for (int j = 0; j < MAX_DIMENSION; j++) {
			int joffset = scale * j;
			noise[i][j] = inoise8(x + ioffset, y + joffset, z);
		}
	}
}


void SpeedTest() {
	ReadAudio();
	ShowFrame();
}


void ReadAudio2() {
	digitalWrite(MSGEQ7_RESET_PIN, HIGH);
	digitalWrite(MSGEQ7_RESET_PIN, LOW);
	for (byte band = 0; band < 7; band++) {
		digitalWrite(MSGEQ7_STROBE_PIN, LOW);
		delayMicroseconds(30);
		left[band] = analogRead(AUDIO_LEFT_PIN) / 4;
		right[band] = analogRead(AUDIO_RIGHT_PIN) / 4;
		digitalWrite(MSGEQ7_STROBE_PIN, HIGH);
	}
}

void ChangePalettePeriodically()
{
	uint8_t secondHand = (millis() / 1000) % 60;
	static uint8_t lastSecond = 99;

	if (lastSecond != secondHand) {
		lastSecond = secondHand;
		if (secondHand == 0) {
			currentPalette = RainbowColors_p;
			currentBlending = LINEARBLEND;
		}
		if (secondHand == 10) {
			currentPalette = RainbowStripeColors_p;
			currentBlending = NOBLEND;
		}
		if (secondHand == 15) {
			currentPalette = RainbowStripeColors_p;
			currentBlending = LINEARBLEND;
		}
		if (secondHand == 20) {
			SetupPurpleAndGreenPalette();
			currentBlending = LINEARBLEND;
		}
		if (secondHand == 25) {
			SetupTotallyRandomPalette();
			currentBlending = LINEARBLEND;
		}
		if (secondHand == 30) {
			SetupBlackAndWhiteStripedPalette();
			currentBlending = NOBLEND;
		}
		if (secondHand == 35) {
			SetupBlackAndWhiteStripedPalette();
			currentBlending = LINEARBLEND;
		}
		if (secondHand == 40) {
			currentPalette = CloudColors_p;
			currentBlending = LINEARBLEND;
		}
		if (secondHand == 45) {
			currentPalette = PartyColors_p;
			currentBlending = LINEARBLEND;
		}
		if (secondHand == 50) {
			currentPalette = myRedWhiteBluePalette_p;
			currentBlending = NOBLEND;
		}
		if (secondHand == 55) {
			currentPalette = myRedWhiteBluePalette_p;
			currentBlending = LINEARBLEND;
		}
	}
}

void SetupTotallyRandomPalette()
{
	for (int i = 0; i < 16; i++) {
		currentPalette[i] = CHSV(random8(), 255, random8());
	}
}

// This function sets up a palette of black and white stripes,
// using code.  Since the palette is effectively an array of
// sixteen CRGB colors, the various fill_* functions can be used
// to set them up.
void SetupBlackAndWhiteStripedPalette()
{
	// 'black out' all 16 palette entries...
	fill_solid(currentPalette, 16, CRGB::Black);
	// and set every fourth one to white.
	currentPalette[0] = CRGB::White;
	currentPalette[4] = CRGB::White;
	currentPalette[8] = CRGB::White;
	currentPalette[12] = CRGB::White;

}

// This function sets up a palette of purple and green stripes.
void SetupPurpleAndGreenPalette()
{
	CRGB purple = CHSV(HUE_PURPLE, 255, 255);
	CRGB green = CHSV(HUE_GREEN, 255, 255);
	CRGB black = CRGB::Black;

	currentPalette = CRGBPalette16(
		green, green, black, black,
		purple, purple, black, black,
		green, green, black, black,
		purple, purple, black, black);
}


// This example shows how to set up a static color palette
// which is stored in PROGMEM (flash), which is almost always more 
// plentiful than RAM.  A static PROGMEM palette like this
// takes up 64 bytes of flash.
const TProgmemPalette16 myRedWhiteBluePalette_p PROGMEM =
{
	CRGB::Red,
	CRGB::Gray, // 'white' is too bright compared to red and blue
	CRGB::Blue,
	CRGB::Black,

	CRGB::Red,
	CRGB::Gray,
	CRGB::Blue,
	CRGB::Black,

	CRGB::Red,
	CRGB::Red,
	CRGB::Gray,
	CRGB::Gray,
	CRGB::Blue,
	CRGB::Blue,
	CRGB::Black,
	CRGB::Black
};


void NoiseExample8() {
	ChangePalettePeriodically();
	x++;
	z++;
	FillNoise(x * 3, x * 3, z, sin8(x) >> 1);
	for (int i = 0; i < kMatrixWidth; i++) {
		for (int j = 0; j < kMatrixHeight; j++) {
			leds[XY(i, j)] = ColorFromPalette(currentPalette, noise[i][j], 255, currentBlending);
		}
	}
	ShowFrame();
}

// *********************************************************************
// CRGB* resizePixels(CRGB* pixels, int w1, int h1, int w2, int h2) {
// transforma cualquier matriz de tama�o w1 h1 a otra de w2 h2 .
// *********************************************************************

void resizePixels(CRGB* pixels, int w1, int h1, int w2, int h2) {
	//	CRGB * temp; // codigo original no consegui que funciones con los punteros
	//	temp = new CRGB[w2 * h2];
	double x_ratio = w1 / (double)w2;
	double y_ratio = h1 / (double)h2;
	double px, py;
	//Serial.print(millis());// debug
	//Serial.println("in ");// debug
	for (int i = 0; i<h2; i++) {
		for (int j = 0; j<w2; j++) {
			px = floor(j*x_ratio);
			py = floor(i*y_ratio);
			/* Serial.print("px ");
			Serial.print(px);
			Serial.print(",");
			Serial.print("py ");
			Serial.print(py);
			Serial.print("x ");
			Serial.print(j);
			Serial.print("y ");
			Serial.print(i);
			Serial.println(",");
			*/
			//temp[(i*w2) + j] = pixels[(int)((py*w1) + px)];
			tempLed[XY2(j, i)] = pixels[(int)(XY(px, py))];

		}
	}
	//Serial.print("out ");// debug
	//	return temp;
	return;

}

void increaseLedMode()//two colors
{
	EVERY_N_MILLISECONDS(8000) { //new random palette every 8 seconds. Might have to wait for the first one to show up
		ledMode++ % 10;
	}
}



