

/*
-------------------------------------------------------------------
Basic Helper functions:

XY_Chico                  translate 2 dimensional coordinates into an index
BresenhamLine       draw a line
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

int XY_Chico(int x, int y) {
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
// para la matriz de height width
//NOTE: if i use cled class, this conversion is not needed
// es usado en 
uint16_t XY(uint8_t x, uint8_t y) {
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

		if (MATRIX_TYPE != VERTICAL_ZIGZAG_MATRIX) {
			i = (x * HEIGHT) + y;
		}

		if (MATRIX_TYPE == VERTICAL_ZIGZAG_MATRIX) {
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
/*
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

*/
//*************************************************
// Bresenham line algorythm based on 2 coordinates
void BresenhamLine(int x0, int y0, int x1, int y1, byte color) {
	int dx = abs(x1 - x0), sx = x0 < x1 ? 1 : -1;
	int dy = -abs(y1 - y0), sy = y0 < y1 ? 1 : -1;
	int err = dx + dy, e2;
	for (;;) {
		leds[XY_Chico(x0, y0)] = CHSV(color, 255, 255);
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
	leds[XY_Chico(x, y)] = CHSV(color, 255, 255);
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
			leds[XY_Chico(i, y - d)] += leds[XY_Chico(i + 1, y - d)];   // lowest row to the right
			leds[XY_Chico(i, y - d)].nscale8(dim);
		}
		for (int i = y - d; i <= y + d; i++) {
			leds[XY_Chico(x + d, i)] += leds[XY_Chico(x + d, i + 1)];   // right colum up
			leds[XY_Chico(x + d, i)].nscale8(dim);
		}
		for (int i = x + d; i >= x - d; i--) {
			leds[XY_Chico(i, y + d)] += leds[XY_Chico(i - 1, y + d)];   // upper row to the left
			leds[XY_Chico(i, y + d)].nscale8(dim);
		}
		for (int i = y + d; i >= y - d; i--) {
			leds[XY_Chico(x - d, i)] += leds[XY_Chico(x - d, i - 1)];   // left colum down
			leds[XY_Chico(x - d, i)].nscale8(dim);
		}
	}
}

// give it a linear tail to the side
void HorizontalStream(byte scale)
{
	for (int x = 1; x < WIDTH; x++) {
		for (int y = 0; y < HEIGHT; y++) {
			leds[XY_Chico(x, y)] += leds[XY_Chico(x - 1, y)];
			leds[XY_Chico(x, y)].nscale8(scale);
		}
	}
	for (int y = 0; y < HEIGHT; y++)
		leds[XY_Chico(0, y)].nscale8(scale);
}

// give it a linear tail downwards
void VerticalStream(byte scale)
{
	for (int x = 0; x < WIDTH; x++) {
		for (int y = 1; y < HEIGHT; y++) {
			leds[XY_Chico(x, y)] += leds[XY_Chico(x, y - 1)];
			leds[XY_Chico(x, y)].nscale8(scale);
		}
	}
	for (int x = 0; x < WIDTH; x++)
		leds[XY_Chico(x, 0)].nscale8(scale);
}

// just move everything one line down
void VerticalMove() {
	for (int y = 15; y > 0; y--) {
		for (int x = 0; x < 16; x++) {
			leds[XY_Chico(x, y)] = leds[XY_Chico(x, y - 1)];
		}
	}
}

// copy the rectangle defined with 2 points x0, y0, x1, y1
// to the rectangle beginning at x2, x3
void Copy(byte x0, byte y0, byte x1, byte y1, byte x2, byte y2) {
	for (int y = y0; y < y1 + 1; y++) {
		for (int x = x0; x < x1 + 1; x++) {
			leds[XY_Chico(x + x2 - x0, y + y2 - y0)] = leds[XY_Chico(x, y)];
		}
	}
}

// rotate + copy triangle (8*8)
void RotateTriangle() {
	for (int x = 1; x < 8; x++) {
		for (int y = 0; y<x; y++) {
			leds[XY_Chico(x, 7 - y)] = leds[XY_Chico(7 - x, y)];
		}
	}
}

// mirror + copy triangle (8*8)
void MirrorTriangle() {
	for (int x = 1; x < 8; x++) {
		for (int y = 0; y<x; y++) {
			leds[XY_Chico(7 - y, x)] = leds[XY_Chico(7 - x, y)];
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
			leds[XY_Chico(WIDTH - 1 - x, y)] = leds[XY_Chico(x, y)];              // copy to A
			leds[XY_Chico(x, HEIGHT - 1 - y)] = leds[XY_Chico(x, y)];             // copy to B
			leds[XY_Chico(WIDTH - 1 - x, HEIGHT - 1 - y)] = leds[XY_Chico(x, y)]; // copy to C
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
			leds[XY_Chico(WIDTH - 1 - x, y)] = leds[XY_Chico(y, x)];    // rotate to A
			leds[XY_Chico(WIDTH - 1 - x, HEIGHT - 1 - y)] = leds[XY_Chico(x, y)];    // rotate to B
			leds[XY_Chico(x, HEIGHT - 1 - y)] = leds[XY_Chico(y, x)];    // rotate to C
		}
	}
}

// adds the color of one quarter to the other 3
void Caleidoscope3() {
	for (int x = 0; x < WIDTH / 2; x++) {
		for (int y = 0; y < HEIGHT / 2; y++) {
			leds[XY_Chico(WIDTH - 1 - x, y)] += leds[XY_Chico(y, x)];    // rotate to A
			leds[XY_Chico(WIDTH - 1 - x, HEIGHT - 1 - y)] += leds[XY_Chico(x, y)];    // rotate to B
			leds[XY_Chico(x, HEIGHT - 1 - y)] += leds[XY_Chico(y, x)];    // rotate to C
		}
	}
}

// add the complete screenbuffer 3 times while rotating
void Caleidoscope4() {
	for (int x = 0; x < WIDTH; x++) {
		for (int y = 0; y < HEIGHT; y++) {
			leds[XY_Chico(WIDTH - 1 - x, y)] += leds[XY_Chico(y, x)];    // rotate to A
			leds[XY_Chico(WIDTH - 1 - x, HEIGHT - 1 - y)] += leds[XY_Chico(x, y)];    // rotate to B
			leds[XY_Chico(x, HEIGHT - 1 - y)] += leds[XY_Chico(y, x)];    // rotate to C
		}
	}
}

// rotate, duplicate and copy over a triangle from first sector into the other half
// (crappy code)
void Caleidoscope5() {
	for (int x = 1; x < 8; x++) {
		leds[XY_Chico(7 - x, 7)] += leds[XY_Chico(x, 0)];
	} //a
	for (int x = 2; x < 8; x++) {
		leds[XY_Chico(7 - x, 6)] += leds[XY_Chico(x, 1)];
	} //b
	for (int x = 3; x < 8; x++) {
		leds[XY_Chico(7 - x, 5)] += leds[XY_Chico(x, 2)];
	} //c
	for (int x = 4; x < 8; x++) {
		leds[XY_Chico(7 - x, 4)] += leds[XY_Chico(x, 3)];
	} //d
	for (int x = 5; x < 8; x++) {
		leds[XY_Chico(7 - x, 3)] += leds[XY_Chico(x, 4)];
	} //e
	for (int x = 6; x < 8; x++) {
		leds[XY_Chico(7 - x, 2)] += leds[XY_Chico(x, 5)];
	} //f
	for (int x = 7; x < 8; x++) {
		leds[XY_Chico(7 - x, 1)] += leds[XY_Chico(x, 6)];
	} //g
}

void Caleidoscope6() {
	for (int x = 1; x < 8; x++) {
		leds[XY_Chico(7 - x, 7)] = leds[XY_Chico(x, 0)];
	} //a
	for (int x = 2; x < 8; x++) {
		leds[XY_Chico(7 - x, 6)] = leds[XY_Chico(x, 1)];
	} //b
	for (int x = 3; x < 8; x++) {
		leds[XY_Chico(7 - x, 5)] = leds[XY_Chico(x, 2)];
	} //c
	for (int x = 4; x < 8; x++) {
		leds[XY_Chico(7 - x, 4)] = leds[XY_Chico(x, 3)];
	} //d
	for (int x = 5; x < 8; x++) {
		leds[XY_Chico(7 - x, 3)] = leds[XY_Chico(x, 4)];
	} //e
	for (int x = 6; x < 8; x++) {
		leds[XY_Chico(7 - x, 2)] = leds[XY_Chico(x, 5)];
	} //f
	for (int x = 7; x < 8; x++) {
		leds[XY_Chico(7 - x, 1)] = leds[XY_Chico(x, 6)];
	} //g
}

// rechtangle 0-1 -> 2-3
// NOT WORKING as intended YET!
void Scale(int x0, int y0, int x1, int y1, int x2, int y2, int x3, int y3) {
	for (int y = y2; y < y3 + 1; y++) {
		for (int x = x2; x < x3 + 1; x++) {
			leds[XY_Chico(x, y)] = leds[XY_Chico(
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
	
	//    resizePixels(leds, int w1, int h1, int w2, int h2) 
	resizePixels(leds, WIDTH, HEIGHT, CUSTOM_WIDTH, CUSTOM_HEIGHT); // hacer flexible 

	// Copy  led colors from leds[src .. src+9] to leds[dest .. dest+9]
	// memmove( &leds[dest], &leds[src], 10 * sizeof( CRGB) );
	//memmove8(&leds2[0], &tempLed[0], CUSTOM_NUM_LEDS * sizeof(CRGB)); // eliminado paso directamente a leds2 sin templed
	
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
			c_leds(x, y) = CRGB::White;
			FastLED.show();
			fadeToBlackBy(c_leds[0], CUSTOM_NUM_LEDS, 20);
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
			leds[XY_Chico(i, j)] = ColorFromPalette(currentPalette, noise[i][j], 255, currentBlending);
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
	
	for (int i = 0; i<h2; i++) {
		for (int j = 0; j<w2; j++) {
			px = floor(j*x_ratio); // Round down value
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
			c_leds(j,i) = pixels[(int)(XY_Chico(px, py))]; // antes templed
		}
	}
	//Serial.print("out ");// debug
	
	return;

}

void increaseLedMode()//two colors
{
	EVERY_N_MILLISECONDS(8000) { //new random palette every 8 seconds. Might have to wait for the first one to show up
		ledMode++ % 10;
	}
}


void drawLineByAngle(uint8_t x, uint8_t y, uint16_t angle, uint8_t length, CHSV  color)
{
	float p_angle = angle, p_length = length, p_start = 0, x1, y1;
	//#define PI 3.14159265

	/*leds.DrawLine(
	x,
	y,
	x + length*cos8(angle ),
	y + length*sin8_C(angle ), color);// version de 8 bits
	*/
	x1 = x + ((p_start + p_length)*cos(p_angle*PI / 180));
	y1 = y + ((p_start + p_length)*sin(p_angle*PI / 180));

	c_leds.DrawLine(

		uint8_t(x + (p_start*(cos(p_angle*PI / 180)))),
		uint8_t(y + (p_start*(sin(p_angle*PI / 180)))),
		uint16_t(x1),
		uint16_t(y1), color
	);
}

void drawLineByAngle(uint8_t xc, uint8_t yc, uint8_t angle, uint8_t start, uint8_t length, CHSV color)
{
	//float p_angle = angle, p_length = length, p_start = start,

	Coordinates point = Coordinates();


	point.fromPolar(start + length, angle, xc, yc); // x + ((p_start + p_length)*(1+cos(p_angle))/2);
													//y1 = y + ((p_start + p_length)*(1+sin(p_angle))/2);
	uint8_t x1 = point.getX(); // coordenadas punto final
	uint8_t y1 = point.getY();
	point.fromPolar(start, angle, xc, yc);
	DrawWuLine(
		point.getX(),
		point.getY(),
		x1,
		y1, color, 255, 8);
	//Serial.print("x1: "); Serial.print(x1); Serial.print("y1: "); Serial.println(y1);// debug

}

void DrawWuLine(uint8_t X0, uint8_t Y0, uint8_t X1, uint8_t Y1,
	CHSV BaseColor, uint8_t NumLevels, uint8_t  IntensityBits)
{
	uint8_t IntensityShift, ErrorAdj, ErrorAcc;
	uint8_t ErrorAccTemp, Weighting, WeightingComplementMask;
	uint8_t DeltaX, DeltaY, Temp, XDir1, XDir2;

	/* Make sure the line runs top to bottom */
	if (Y0 > Y1) {
		Temp = Y0; Y0 = Y1; Y1 = Temp;
		Temp = X0; X0 = X1; X1 = Temp;
	}
	/* Draw the initial pixel, which is always exactly intersected by
	the line and so needs no weighting */
	DrawPixel(X0, Y0, BaseColor);
	if (X1 >= X0) {
		DeltaX = X1 - X0;
		XDir1 = 1; XDir2 = 0;//pendiente positiva
							 //Serial.print("inicial x1 "); Serial.println(X1);// debug
							 //Serial.print("; inicial y1 "); Serial.println(Y1);// debug

	}

	else {
		XDir1 = 0; XDir2 = 1;	//pendiente negativa XDir = -1  because i dont use negative numbers;
		DeltaX = X0 - X1; /* make DeltaX positive */
	}
	/* Special-case horizontal, vertical, and diagonal lines, which
	require no weighting because they go right through the center of
	every pixel */

	DeltaY = Y1 - Y0;


	if (DeltaY == 0) {
		/* Horizontal line */


		while (DeltaX-- != 0) {
			X0 += XDir1 - XDir2;
			DrawPixel(X0, Y0, BaseColor);
		}
		return;
	}
	if (DeltaX == 0) {
		/* Vertical line */
		do {
			Y0++;
			DrawPixel(X0, Y0, BaseColor);
		} while (--DeltaY != 0);
		return;
	}
	if (DeltaX == DeltaY) {
		/* Diagonal line */
		do {
			X0 += XDir1 - XDir2;
			Y0++;
			DrawPixel(X0, Y0, BaseColor);
		} while (--DeltaY != 0);
		return;
	}

	/* Line is not horizontal, diagonal, or vertical */


	ErrorAcc = 0;  /* initialize the line error accumulator to 0 */
				   /* # of bits by which to shift ErrorAcc to get intensity level */
	IntensityShift = 16 - IntensityBits;
	/* Mask used to flip all bits in an intensity weighting, producing the
	result (1 - intensity weighting) */
	WeightingComplementMask = NumLevels - 1;
	/* Is this an X-major or Y-major line? */
	if (DeltaY > DeltaX) {
		/* Y-major line; calculate 8-bit fixed-point fractional part of a
		pixel that X advances each time Y advances 1 pixel, truncating the
		result so that we won't overrun the endpoint along the X axis */
		ErrorAdj = ((int)DeltaX << 8) / (uint8_t)DeltaY;
		/* Draw all pixels other than the first and last */
		while (--DeltaY) {
			ErrorAccTemp = ErrorAcc;   /* remember currrent accumulated error */
			ErrorAcc = ErrorAcc + ErrorAdj;/* calculate error for next pixel */

			if (ErrorAcc <= ErrorAccTemp) {
				/* The error accumulator turned over ( saturated to max 255), so advance the X coord */
				X0 += XDir1 - XDir2;
			}
			Y0++; /* Y-major, so always advance Y */
				  /* The IntensityBits most significant bits of ErrorAcc give us the
				  intensity weighting for this pixel, and the complement of the
				  weighting for the paired pixel */
			Weighting = ErrorAcc;
			DrawPixel(X0, Y0, CHSV(BaseColor.hue, 255, ErrorAcc));

			//Serial.print("pix1 x0 "); Serial.print(X0); Serial.print("; pix1 y0  "); Serial.println(Y0);		// debug


			DrawPixel(X0 + XDir1 - XDir2, Y0, CHSV(BaseColor.hue, 255, 255 - ErrorAcc));

			//Serial.print("xdir1  "); Serial.print((XDir1)); Serial.print("; xdir2  "); Serial.println((XDir2));// debug
		}
		/* Draw the final pixel, which is
		always exactly intersected by the line
		and so needs no weighting */
		DrawPixel(X1, Y1, CHSV(BaseColor.hue, 255, 255));
		return;
	}
	/* It's an X-major line; calculate 16-bit fixed-point fractional part of a
	pixel that Y advances each time X advances 1 pixel, truncating the
	result to avoid overrunning the endpoint along the X axis */
	ErrorAdj = ((int)DeltaY << 8) / (uint8_t)DeltaX;

	//Serial.print("error  "); Serial.println(ErrorAdj);// debug

	/* Draw all pixels other than the first and last */
	while (--DeltaX) {
		ErrorAccTemp = ErrorAcc;   /* remember currrent accumulated error */
								   //ErrorAcc=qadd8(ErrorAcc,ErrorAdj);      /* calculate error for next pixel */
		ErrorAcc = ErrorAcc + ErrorAdj;
		//if (ErrorAcc ==255) {
		if (ErrorAcc <= ErrorAccTemp) {
			/* The error accumulator turned over, so advance the Y coord */
			Y0++;
		}
		X0 += XDir1 - XDir2; /* X-major, so always advance X */
							 /* The IntensityBits most significant bits of ErrorAcc give us the
							 intensity weighting for this pixel, and the complement of the
							 weighting for the paired pixel */
		Weighting = ErrorAcc >> IntensityShift;
		DrawPixel(X0, Y0, CHSV(BaseColor.hue, 255, 255 - ErrorAcc));
		DrawPixel(X0, Y0 + 1, CHSV(BaseColor.hue, 255, ErrorAcc));

		//Serial.print("x0  "); Serial.print(X0); Serial.print(" y0  "); Serial.print(Y0); Serial.print(" err "); Serial.println(ErrorAcc);// debug
	}
	/* Draw the final pixel, which is always exactly intersected by the line
	and so needs no weighting */
	DrawPixel(X1, Y1, CHSV(BaseColor.hue, 255, 255));
}

void DrawPixel(uint8_t X0, uint8_t Y0, CHSV pixel)
{
	c_leds(X0, Y0) = pixel;


}



void circleBeat() {
	// Use two out-of-sync sine waves
	//incorporar centro y radio para ubicarlo en cualquier posicion, pasarlo a clase
	//uint8_t  r = beatsin8(5, 0, 19, 0, 0);
	uint8_t  i = beatsin8(32, 0, 19, 0, 0);
	uint8_t  j = beatsin8(32, 0, 23, 0, 192);// la misma frecuencia hace un circulo con y defasado 64 gira horario y defasado192 en anti
	c_leds(i, j) = CHSV((i + j) * 4, 255, 255);

	blur2d(c_leds[0], MATRIX_WIDTH, MATRIX_HEIGHT, 16);
}





void sendAll()
{
	//Serial.print("SendAll : \t");
	String json = "{";
	json += "\"messageName\":\"js_pat\"";// 
	json += ",\"currentPattern\":{";
	json += "\"index\":" + String(gCurrentPatternNumber);
	json += ",\"name\":\"" + gPatternsAndArguments[gCurrentPatternNumber].mName + "\"}";
	json += ",\"patterns\":[";
	for (uint8_t i = 0; i < numberOfPatterns; i++)
	{
		json += "\"" + gPatternsAndArguments[i].mName + "\"";
		if (i < numberOfPatterns - 1)
			json += ",";
	}
	json += "]";

	json += "}";

	// ESP8266WebServer.send(200, "text/json", json); // para webserver normal
	webSocket.broadcastTXT(json); // para WS
	//USE_SERIAL.printf("json  : \t" ); // debug
	USE_SERIAL.print(json);			// debug
	json = String();
}

void sendHeartBeat()
{
	//Serial.print("HB : \t");

	String json = "{";
	json += "\"messageName\":\"js_hbe\"";
	json += "}";

	// ESP8266WebServer.send(200, "text/json", json); // para webserver normal
	//webSocket.broadcastTXT(json); // para WS
	//USE_SERIAL.printf("json  : \t"); // debug
	//USE_SERIAL.print(json);			// debug
	json = String();
}


