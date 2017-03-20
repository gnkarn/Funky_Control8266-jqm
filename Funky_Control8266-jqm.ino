/*
TODO:
 
 example show oscis+p
 document caleidoscopes better
 write better caleidoscopes...
 improve and document emitters and oszillators
 explaining one example step by step:
 goal? what? how? why?
 gridmapping for rotation + zoom
 good interpolation for other matrix dimensions than 16*16
 more move & stream functions
 layers
 palettes
 link effects to areas
 1D examples
 2d example with more than 2 sines
 speed up MSGEQ7 readings
 
 
 DONE:
 25.6.      creating basic structure 
 setting up basic examples 
 26.6.      MSGEQ7 Support
 wrote more examples
 27.6.      improved documentation
 added Move
 added AutoRun
 TODO list
 Copy
 29.6.      rotate+mirror triangle
 more examples
 30.6.      RenderCustomMatrix
 added more comments
 alpha version released
 
 
/*
 
/*
 Funky Clouds Compendium (alpha version)
 by Stefan Petrick
 
 An ever growing list of examples, tools and toys 
 for creating one- and twodimensional LED effects.
 
 Dedicated to the users of the FastLED v2.1 library
 by Daniel Garcia and Mark Kriegsmann.
 
 Provides basic and advanced helper functions.
 Contains many examples how to creatively combine them.
 
 Tested @ATmega2560 (runs propably NOT on an Uno or
 anything else with less than 4kB RAM)
 */

// modificado por GNK para matriz de 20x24 
// la fncion de conversion interpola 2 valores de x para generar un x nuevo, pues son mas leds fisicos en x
// y promedia dos valores de Y para calcular un Y en la matriz de salida , pues son menos
// sin terminar incopora lectura de port serie para cambiar parametros, ver listas de lectura
// http://robotic-controls.com/learn/arduino/reading-numbers-serial
// https://gist.github.com/kasperkamperman/6c7c256f7042a3ca5118
// y para excritura desde teensy a processing ver https://github.com/marmilicious/FastLED_2_Processing/blob/master/test_serial_from_arduino_v10.pde
// oct-2016 modificado para 8266 e incorpora manejo de paterns
// funciona ok con sonido
// 21-8 integra lecdcontrol8266 matriz jqm, con funkyCloudsApa102-8266 este ultimo, funciona ok, con sonido, no tiene integrado la interface de aplicacion web.

//ver // https://github.com/wvdv2002/ESP8266-LED-Websockets/blob/master/arduino/ledcontrol/ledcontrol.ino
//falta incluir los efectos de ESP8266 FastledNoise


#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include <FastLED.h>
#include <Hash.h>
#include <EEPROM.h>
#include <WebSockets.h>
#include <WebSocketsServer.h>
#include "SettingsServer.h"
#include <ESP8266HTTPUpdateServer.h>
#include <ESP8266SSDP.h>



#include <ESP8266WebServer.h>
#include <FastLED_GFX\FastLED_GFX.h>



int  ledState = LOW;

#define USE_SERIAL Serial

extern "C" {
  #include "user_interface.h"
}

// define your LED hardware setup here
#define DATA_PIN    4// D2sale x gpio 04
#define CLK_PIN   5//D1 sale por gpio 05
#define LED_TYPE    APA102
#define COLOR_ORDER BGR

int rawSerial;  // recibe el int desde slider de processing
boolean testing = false;  // Default is false. [Ok to change for testing.]
  // Can temporarily change testing to true to check output in serial monitor.
  // Must set back to false to allow Processing to connect and receive data.

boolean linkedUp = false;  // Initially set linkup status false. [Do Not change.]
/****End of variables needed for sending Processing. */


// Param for different pixel layouts
const bool    kMatrixSerpentineLayout = true;

// set master brightness 0-255 here to adjust power consumption
// and light intensity
#define BRIGHTNESS  31

// enter your custom matrix size if it is NOT a 16*16 and
// check in that case the setup part and
// RenderCustomMatrix() and
// ShowFrame() for more comments
const uint8_t CUSTOM_WIDTH  = 20;
const uint8_t CUSTOM_HEIGHT = 24;

// MSGEQ7 wiring on spectrum analyser shield
#define MSGEQ7_STROBE_PIN 4//antes 2
#define MSGEQ7_RESET_PIN  12// antes 1
#define AUDIO_LEFT_PIN    0
#define AUDIO_RIGHT_PIN   0


// all 2D effects will be calculated in this matrix size
// do not touch
const uint8_t WIDTH  = 16;// 
const uint8_t HEIGHT = 16;

// number of LEDs based on fixed calculation matrix size
// do not touch
#define NUM_LEDS (WIDTH * HEIGHT)
#define CUSTOM_NUM_LEDS (CUSTOM_WIDTH * CUSTOM_HEIGHT)

// seteo de variables desde ledcontrol8266-matriz-jqm
// revisar redindancia y luego optimizar

#define MILLI_AMPERE      1000    // IMPORTANT: set here the max milli-Amps of your power supply 5V 2A = 2000
#define FRAMES_PER_SECOND  200    // here you can control the speed. 
int ledMode = 4;                  // this is the starting palette
const uint8_t kMatrixWidth  = WIDTH;
const uint8_t kMatrixHeight = HEIGHT;

#define MAX_DIMENSION ((CUSTOM_WIDTH>CUSTOM_HEIGHT) ? CUSTOM_WIDTH : CUSTOM_HEIGHT)

// The 16 bit version of our coordinates
static uint16_t x;
static uint16_t y;
static uint16_t z;


// We're using the x/y dimensions to map to the x/y pixels on the matrix.  We'll
// use the z-axis for "time".  speed determines how fast time moves forward.  Try
// 1 for a very slow moving effect, or 60 for something that ends up looking like
// water.
uint8_t speed = 20; // speed is set dynamically once we've started up

// Scale determines how far apart the pixels in our noise matrix are.  Try
// changing these values around to see how it affects the motion of the display.  The
// higher the value of scale, the more "zoomed out" the noise iwll be.  A value
// of 1 will be so zoomed in, you'll mostly see solid colors.
uint8_t scale = 30; // scale is set dynamically once we've started up

// This is the array that we keep our computed noise values in
uint8_t noise[MAX_DIMENSION][MAX_DIMENSION];
CRGBPalette16 currentPalette(CRGB::Black);

 CRGBPalette16 targetPalette( CRGB::Black );
uint8_t       colorLoop = 1;

//Some Variables
byte myEffect = 1;                  //what animation/effect should be displayed

byte myHue = 33;                    //I am using HSV, the initial settings display something like "warm white" color at the first start
byte mySaturation = 168;
byte myValue = 255;
String  myrgb = "(0,0,0)";            // color desde websockets color picker
unsigned int myparameter1=0;			// usef for speed change on noise effect as a global var
byte rainbowHue = myHue;            //Using this so the rainbow effect doesn't overwrite the hue set on the website

int flickerTime = random(200, 400);
int flickerLed;
int flickerValue = 110 + random(-3, +3); //70 works nice, too
int flickerHue = 33;

bool eepromCommitted = true;      

unsigned long currentTime = 0;
unsigned long previousTime = 0;
unsigned long lastChangeTime = 0;
unsigned long currentChangeTime = 0;



// fin seteo de variables del ledcontrol-8266 m,atriz-jqm



// the rendering buffer (16*16)
// do not touch
CRGB leds[NUM_LEDS]; // matriz render

// your display buffer for your not 16*16 setup
CRGB leds2[CUSTOM_NUM_LEDS]; // matriz real
CRGBSet ledSet(leds, NUM_LEDS);    //Trying LEDSet from FastLED

CRGB  tempLed[CUSTOM_WIDTH*CUSTOM_HEIGHT];// debug

// the oscillators: linear ramps 0-255
// modified only by MoveOscillators()
byte osci[4]; 

// sin8(osci) swinging between 0 - 15
// modified only by MoveOscillators()
byte p[4];

// storage of the 7 10Bit (0-1023) audio band values
// modified only by AudioRead()
int left[7];    
int right[7];



// noise stuff
//uint16_t speed = 10;
//uint16_t scale = 50;
uint16_t scale2 = 30;



// #define MAX_DIMENSION ((kMatrixWidth>kMatrixHeight) ? kMatrixWidth : kMatrixHeight)
//uint8_t noise[MAX_DIMENSION][MAX_DIMENSION];
uint8_t noise2[MAX_DIMENSION][MAX_DIMENSION];



static uint16_t x2;
static uint16_t y2;
static uint16_t z2;
//palette stuff
//CRGBPalette16 currentPalette;
TBlendType    currentBlending;
extern CRGBPalette16 myRedWhiteBluePalette;
extern const TProgmemPalette16 myRedWhiteBluePalette_p PROGMEM;

uint8_t gCurrentPatternNumber = 0; // Index number of which pattern is current

#include "LEDanimations.h"
#include "LEDWebsockets.h"


// factores para mapear la matriz de 16x16 en la matriz fisica
uint8_t Fy = kMatrixHeight/CUSTOM_HEIGHT;
uint8_t Fx = kMatrixWidth/CUSTOM_WIDTH;

long Duracion ;// para timer de funcion
long Previous_millis=0;

// List of patterns to cycle through.  Each is defined as a separate function below.
// Each pattern is defined as a function that takes two uint8_t's; all of the pattern functions
// have to have the same signature in this setup, and two (hardcoded!) parameters can be 
// passed to each one: ( mark kriesgman demo reel arg2)
typedef void(*TwoArgumentPattern)(uint8_t arg1, uint8_t arg2);
typedef struct {
	TwoArgumentPattern mPattern;
	uint8_t mArg1;
	uint8_t mArg2;
} TwoArgumentPatterWithArgumentValues;

TwoArgumentPatterWithArgumentValues gPatternsAndArguments[] = {
	{ Dots1			,	1	/*color1 */	, 1 /*color2 */ },
	{ Dots2			,	125 /*scale*/	, 0 /*no se usa*/},
	{ SlowMandala2,		127 /*dim*/		, 0 /*no se usa*/},
	{ SlowMandala3,		127 /*dim*/		, 0 /*no se usa*/},
	{ Mandala8		,	110 /*dim*/		, 0 /*no se usa*/},
	{ MSGEQtest		,	120 /* scale*/	, 0 /*no se usa*/ },
	{ MSGEQtest2	,	127 /*scale*/	, 200 /*color*/},
	{ MSGEQtest3	,	120 /*scale*/	, 100 /*color*/ },
	{ MSGEQtest4	,	240 /* dim*/	, 10 /* hmult*/},
	{ AudioSpiral	,	130 /*color1*/	, 122 /*color*/},
	{ MSGEQtest5	,	120 /*dim*/		, 10 /*hmult*/},
	{ MSGEQtest6	,	170 /*dim*/		, 10 /*hmult*/ },
	{ MSGEQtest7	,	240 /*dim*/		, 10 /*hmult*/ },// revisar
	{ MSGEQtest8	,	240 /*dim*/		, 10 /*hmult*/ },// revisar
	{ MSGEQtest9	,	27 /*dim*/		, 255 /*s*/},
	{ CopyTest		,	200 /* dim*/	, 10 /*hmult*/ },
	{ CopyTest2		,	200 /* dim*/	, 10 /*hmult*/ },
	{ Audio1		,	10 /*dim*/		,15 /*hmult*/},
	{ Audio2		,	120 /*dim*/		,15 /*hmult*/ },
	{ Audio3		,	255 /*dim*/		,27 /*hmult*/ },
	{ Audio4		,	12 /*dim*/		,32 /*hdiv*/ },
	{ CaleidoTest2	,	240 /* dim*/	,150 /* color_ofset*/},
	{ CaleidoTest1	,	240 /* dim*/	,32 /* hdiv*/ },
	{ Audio5		,	9 /*dim*/		,30 /* hmult*/},
	{ Audio6		,	200 /*dim*/		,10 /* hmult*/ },
	{ NoiseExample1	,	1 /* hpow2*/	,255 /* s*/ },
	{ NoiseExample2	,	100/*noisez*/	,100 /* scale*/},
	{ NoiseExample3	,	100/*noisez*/	,100 /* scale*/ },
	{ NoiseExample4	,	100/*noisez*/	,100 /* scale*/ },
	{ NoiseExample5	,	100/*noisez*/	,254 /* scale300*/ },// 29
	{ NoiseExample6	,	50/*hofset*/	,200 /* scale*/ },
	{ NoiseExample7	,	0/*nada*/		,100 /* scale*/ },
	{ LedsNoise		,	0/* nada*/		,0	 /* nada1*/ },//32

	//las funciones de  demoreel 100 adaptarlas para matriz e incluir
	
};





/*
-------------------------------------------------------------------
 Init Inputs and Outputs: LEDs and MSGEQ7
 -------------------------------------------------------------------
 */
void setup() {
  // Open serial connection, 115200 baud
pinMode(LED_BUILTIN, OUTPUT);// led pin as output
pinMode(MSGEQ7_STROBE_PIN, OUTPUT);// strobe pin as output
pinMode(MSGEQ7_RESET_PIN, OUTPUT);// reset  pin as output

pinMode(DATA_PIN, OUTPUT);// led data  pin as output
pinMode(CLK_PIN, OUTPUT);// led clk  pin as output



  Serial.begin(115200);
  

 // firstContact();  // Connect with Processing. Hello, is anyone out there? (eliminado pues no uso el port serie con processing)
  
  // use the following line only when working with a 16*16
  // and delete everything in the function RenderCustomMatrix() 
  // at the end of the code; edit XY() to change your matrix layout
  // right now it is doing a serpentine mapping
  //FastLED.addLeds<CHIPSET, LED_PIN, COLOR_ORDER>(leds2, CUSTOM_NUM_LEDS);
  
    //FastLED.addLeds<APA102,11,13,RGB,DATA_RATE_MHZ(10) >(leds,NUM_LEDS);
   FastLED.addLeds<LED_TYPE,DATA_PIN,CLK_PIN,COLOR_ORDER,DATA_RATE_MHZ(10)>(leds2, CUSTOM_NUM_LEDS).setCorrection(TypicalLEDStrip);

  FastLED.setBrightness(BRIGHTNESS);
  set_max_power_in_volts_and_milliamps(5, MILLI_AMPERE);
  LEDColorCorrection{ TypicalSMD5050 };
  FastLED.setDither(0);
  // just for debugging:
  
  
  InitMSGEQ7();
  // Initialize our noise coordinates to some random values
  x = random16();
  y = random16();
  z = random16();

  x2 = random16();
  y2 = random16();
  z2 = random16();

// setup desde ledcontrol-8266-matriz-jqm
 
 // Reading EEPROM
  myEffect = 1;                         // Only read EEPROM for the myEffect variable after you're sure the animation you are testing won't break OTA updates, make your ESP restart etc. or you'll need to use the USB interface to update the module.
//  myEffect = EEPROM.read(0); //blocking effects had a bad effect on the website hosting, without commenting this away even restarting would not help
  myHue = EEPROM.read(1);
  mySaturation = EEPROM.read(2);
  myValue = EEPROM.read(3);
  myparameter1 = EEPROM.read(4);
  myparameter1 += EEPROM.read(5)*256;

  delay(100);                                         //Delay needed, otherwise showcolor doesn't light up all leds or they produce errors after turning on the power supply - you will need to experiment
  LEDS.showColor(CHSV(myHue, mySaturation, myValue));
 // fin setup desde ledcontrol.8266 matriz-jqm
 


  Serial.println();
  Serial.print(F("Heap: ")); Serial.println(system_get_free_heap_size());
  Serial.print(F("Boot Vers: ")); Serial.println(system_get_boot_version());
  Serial.print(F("CPU: ")); Serial.println(system_get_cpu_freq());
  Serial.print(F("SDK: ")); Serial.println(system_get_sdk_version());
  Serial.print(F("Chip ID: ")); Serial.println(system_get_chip_id());
  Serial.print(F("Flash ID: ")); Serial.println(spi_flash_get_id());
  Serial.print(F("Flash Size: ")); Serial.println(ESP.getFlashChipRealSize());
  Serial.print(F("Vcc: ")); Serial.println(ESP.getVcc());
  Serial.println();

  setupWiFi();
  startSettingsServer();
  webSocket.begin();
  webSocket.onEvent(webSocketEvent);

  Serial.println(" fin Setup ");

 
  
}


/*
-------------------------------------------------------------------
 The main program
 -------------------------------------------------------------------
 */
void loop()
{
EVERY_N_MILLISECONDS(500) {                           // FastLED based non-blocking delay to update/display the sequence.
     ledState=!ledState ;
     digitalWrite(LED_BUILTIN, ledState );
     
    }
   
settingsServerTask();
webSocket.loop();                           // handles websockets


 // AutoRun();
  // Comment AutoRun out and test examples seperately here

  //Dots2();
  //RainbowTriangle();
  //ShowFrame();
  //Caleidoscope1();
  // all oscillator based:
  //Serial.println("dots1");
  //for (int i = 0; i < 300; i++) { Dots1(); }
  //for (int i = 0; i < 300; i++) { Dots2(); }
  //Serial.println("slowmandala");
  //SlowMandala();
  //Serial.println("slowmandala2");
  //SlowMandala2();
  //Serial.println("slowmandala3");
  //SlowMandala3();
  //Serial.println("Mandala8");
  //for (int i = 0; i < 300; i++) { Mandala8(); }

  // For discovering parameters of examples I reccomend to
  // tinker with a renamed copy ...

  // Call the current pattern function once, updating the 'leds' array
  uint8_t arg1 = gPatternsAndArguments[gCurrentPatternNumber].mArg1;
  uint8_t arg2 = gPatternsAndArguments[gCurrentPatternNumber].mArg2;
  TwoArgumentPattern pat = gPatternsAndArguments[gCurrentPatternNumber].mPattern;

  pat(arg1, arg2);


  // send the 'leds' array out to the actual LED strip
 // FastLED.show(); // los tengo dentro de las funciones
  // insert a delay to keep the framerate modest
//  FastLED.delay(1000 / 120); // about sixty FPS

  EVERY_N_SECONDS(100) { nextPattern(); } // change patterns periodically
}

void nextPattern()
{
	// add one to the current pattern number, and wrap around at the end
	const int numberOfPatterns = sizeof(gPatternsAndArguments) / sizeof(gPatternsAndArguments[0]);
	gCurrentPatternNumber = (gCurrentPatternNumber + 1) % numberOfPatterns;
	Serial.print("funcion : \t");
	Serial.println(gCurrentPatternNumber);
	
}



