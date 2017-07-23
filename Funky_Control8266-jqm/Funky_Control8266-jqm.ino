#include <Arduino.h>

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
//esta version incorpora el modo ASYNC de webserver y de websockets
// https://github.com/me-no-dev/ESPAsyncWebServer#libraries-and-projects-that-use-asyncwebserver
//wifi manager async  https://github.com/btomer/WiFiManager




#include <ArduinoOTA.h>
// NOTE: This requires btomer's fork of the WiFiManager library (https://github.com/btomer/WiFiManager)
#define WIFI_MANAGER_USE_ASYNC_WEB_SERVER
#include <WiFiManager.h>

#include <ESP8266WiFi.h>
#include <DNSServer.h>
#include <WiFiUdp.h>
#include <FastLED.h>
#include <Hash.h>
#include <EEPROM.h>
#include <ESPAsyncWebServer.h>   // async mod
#include <WebSockets.h>
#include <WebSocketsServer.h>
#include "SettingsServer.h"
#include <ESP8266HTTPUpdateServer.h>
#include <ESP8266SSDP.h>
//#include <ESP8266WebServer.h> // incluido en la version async de wifimanager
#include <ArduinoJson.h>

#include <FastLED_GFX.h>
// para integrar MAtrix example-APA102
#include <LEDMatrix.h>  
// ver https://github.com/AaronLiddiment/LEDMatrix/wiki
#include <coordinates.h> // ahora incluido como una libreria

// json setup ofr the Video matrix json object
//sized using  https://bblanchon.github.io/ArduinoJson/assistant/index.html
#define SENSORDATA_JSON_SIZE (JSON_ARRAY_SIZE(480) + 2 * JSON_OBJECT_SIZE(0) + 479 * JSON_OBJECT_SIZE(3) + 8740)



int  ledState = LOW;

#define USE_SERIAL Serial

extern "C" {
#include "user_interface.h"
}

// websocket client library , used to connect with Heroku ws server
#include "WebSocketsClient.h"

//#include <SocketIoClient\SocketIoClient.h>
// tested with this library but ends with exception 28 https://github.com/timum-viw/socket.io-client

WebSocketsClient herokuWs;
#define HEARTBEAT_INTERVAL 25000
uint64_t heartbeatTimestamp = 0;
bool isConnectedH = false; // connected to Heroku?
#define  ESP_HeartBeat "2"
String herokuHost = "app-gnk-p5js.herokuapp.com"; // for websocketclien
int herokuport = 80;
const char*  msgName = ""; //Heroku message name

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
#define BRIGHTNESS  250

// enter your custom matrix size if it is NOT a 16*16 and
// check in that case the setup part and
// RenderCustomMatrix() and
// ShowFrame() for more comments
const uint8_t CUSTOM_WIDTH = 20;
const uint8_t CUSTOM_HEIGHT = 24;

uint8_t hue; // para cambiar color en efectos dentro del loop

// MSGEQ7 wiring on spectrum analyser shield
#define MSGEQ7_STROBE_PIN 4//antes 2
#define MSGEQ7_RESET_PIN  12// antes 1
#define AUDIO_LEFT_PIN    0
#define AUDIO_RIGHT_PIN   0


// all 2D effects will be calculated in this matrix size
// do not touch
const uint8_t WIDTH = 16;// 
const uint8_t HEIGHT = 16;

// number of LEDs based on fixed calculation matrix size
// do not touch
#define NUM_LEDS (WIDTH * HEIGHT)
#define CUSTOM_NUM_LEDS (CUSTOM_WIDTH * CUSTOM_HEIGHT)

#define MATRIX_WIDTH   20  // Set this negative if physical led 0 is opposite to where you want logical 0
#define MATRIX_HEIGHT  24  // Set this negative if physical led 0 is opposite to where you want logical 0
#define MATRIX_TYPE    VERTICAL_ZIGZAG_MATRIX  // See top of LEDMatrix.h for matrix wiring types
#define MATRIX_SIZE		MATRIX_WIDTH*MATRIX_HEIGHT
cLEDMatrix<MATRIX_WIDTH, MATRIX_HEIGHT, MATRIX_TYPE> c_leds;

uint8_t h;// color para funciones

// seteo de variables desde ledcontrol8266-matriz-jqm
// revisar redindancia y luego optimizar

#define MILLI_AMPERE      1500    // IMPORTANT: set here the max milli-Amps of your power supply 5V 2A = 2000
#define FRAMES_PER_SECOND  64    // here you can control the speed. 
int ledMode = 4;                  // this is the starting palette
const uint8_t kMatrixWidth = WIDTH;
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

CRGBPalette16 targetPalette(CRGB::Black);
uint8_t       colorLoop = 1;

//Some Variables
byte myOnOff = 0;					// general on off leds status
byte myEffect = 1;                  //what animation/effect should be displayed

byte myHue = 33;                    //I am using HSV, the initial settings display something like "warm white" color at the first start
byte mySaturation = 168;
byte myValue = 255;
String  myrgb = "(0,0,0)";            // color desde websockets color picker
unsigned int myparameter1 = 0;			// usef for speed change on noise effect as a global var
byte rainbowHue = myHue;            //Using this so the rainbow effect doesn't overwrite the hue set on the website
byte	myBrightness = BRIGHTNESS; // global brightness changable from screen

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
CRGBSet ledSet(leds2, CUSTOM_NUM_LEDS);    //Trying LEDSet from FastLED

// CRGB  tempLed[CUSTOM_WIDTH*CUSTOM_HEIGHT];// debug eliminado

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

// estas clases , para efectos deberia pasarlas a un .h separado
class Circulo {
	// Class Member Variables
	// These are initialized at startup
private:
	unsigned long mpreviousMillis, mupdatet; 	// will store last time LED was updated
	uint8_t mcrad, mcrpm, mcxc, mcyc;
	bool mcdir;
	uint8_t mcx, mcy;// pixel coordinates
	uint16_t  mcrev;
	CHSV mccolor;
	uint8_t mcphi; // angulo del pixel

public:
	// cxc,cyc,crpm,crev,crad,ccolor,cdir : center coords, rpm, num of revs, radious , color ,dir
	// Constructor - creates a Circulo  
	// and initializes the member variables and state

	Circulo(uint8_t cxc, uint8_t cyc, uint16_t crpm, uint8_t crev, uint8_t crad, CHSV ccolor, bool cdir)
	{
		mcxc = cxc;
		mcyc = cyc;
		mcrpm = crpm;
		mcrev = crev;
		mcrad = crad;
		mcphi = 0;
		mccolor = ccolor;
		mcdir = cdir;// 0 horario 1 anti
		mpreviousMillis = 0;  	// will store last time LED was updated
		mupdatet = 256 / (crpm); // time in msecs  that justifies and update of 1 degree , based on rpm and matrix resolution , other alternative =7500 / (crpm*crad)
	}

	uint8_t getrad() { return mcrad; };
	uint8_t getrpm() { return mcrpm; };
	uint8_t getxc() { return mcxc; };
	uint8_t getycrev() { return mcrev; };
	void setxc(uint8_t cxc) { mcxc = cxc; };
	void setyc(uint8_t cyc) { mcyc = cyc; };
	void  setColor(CHSV ccolor) { mccolor = ccolor; };



	//void Start() {};
	//void Stop() {};
	void ChgDir() {
		mcdir = !mcdir;
	}

	void Update()
	{
		// check to see if it's time to change the state of the LED
		unsigned long currentMillis = millis();

		if (currentMillis - mpreviousMillis >= mupdatet)
		{
			Coordinates cpoint = Coordinates();

			cpoint.fromPolar(mcrad, mcphi, mcxc, mcyc); // x + ((p_start + p_length)*(1+cos(p_angle))/2);
														//y1 = y + ((p_start + p_length)*(1+sin(p_angle))/2);
			uint8_t x1 = cpoint.getX(); // coordenadas punto 
			uint8_t y1 = cpoint.getY();
			c_leds(x1, y1) = mccolor;
			//Serial.println(  x1, y1);

			mpreviousMillis = currentMillis;  // Remember the time
			mcphi = mcphi + 1 * (mcdir)-1 * (!mcdir); // incrementa en la direccion indicada
			FastLED.show();

		}
	}

};
class CircleBeat { //cxc,cyc,bpm,crad,ccolor, status, cdir 
				   // Class Member Variables
				   // These are initialized at startup
private:
	unsigned long mpreviousMillis, mupdatet; 	// will store last time LED was updated
	uint8_t mcrad, mcbpm, mcxc, mcyc; // radio, BPM, center coords
	bool mcdir;
	uint8_t mcx, mcy;// pixel coordinates
	bool  mcstatus;// 0 detenido , 1 activo
	CHSV mccolor;


public:
	// cxc,cyc,crpm,crev,crad,ccolor,cdir : center coords, rpm, num of revs, radious , color ,dir
	// Constructor - creates a Circulo  
	// and initializes the member variables and state

	CircleBeat(uint8_t cxc, uint8_t cyc, uint16_t cbpm, uint8_t crad, CHSV ccolor, bool cbstatus, bool cdir)
	{
		mcxc = cxc;
		mcyc = cyc;
		mcbpm = cbpm;
		mcrad = crad;
		mccolor = ccolor;
		mcdir = cdir;// 0 horario 1 anti
		mpreviousMillis = 0;  	// will store last time LED was updated
		mcstatus = cbstatus = 0;

	}

	uint8_t getrad() { return mcrad; };
	uint8_t getbpm() { return mcbpm; };
	uint8_t getxc() { return mcxc; };
	bool  getstatus() { return mcstatus; };
	bool  getdir() { return mcdir; };
	void setxc(uint8_t cxc) { mcxc = cxc; };
	void setyc(uint8_t cyc) { mcyc = cyc; };
	void  setColor(CHSV ccolor) { mccolor = ccolor; };
	void setdir(bool cdir) { mcdir = cdir; };


	void Start() { mcstatus = 1; };
	void ChangeStatus() { mcstatus = !mcstatus; };
	void Stop() { mcstatus = 0; };
	void ChgDir() { mcdir = !mcdir; }

	void Update()
	{
		// check to see if it's time to change the state of the LED
		unsigned long currentMillis = millis();

		if (currentMillis - mpreviousMillis >= mupdatet)
		{
			// Use two out-of-sync sine waves
			//incorporar centro y radio para ubicarlo en cualquier posicion, pasarlo a clase
			//uint8_t  r = beatsin8(5, 0, 19, 0, 0);

			uint8_t  i = beatsin8(mcbpm, mcxc - mcrad, mcxc + mcrad, 0, 0);
			uint8_t  j = beatsin8(mcbpm, mcyc - mcrad, mcyc + mcrad, 0, 64 * mcdir + 192 * !mcdir);// la misma frecuencia hace un circulo con y defasado 64 gira horario y defasado192 en anti
			c_leds(i, j) = mccolor;

			blur2d(c_leds[0], MATRIX_WIDTH, MATRIX_HEIGHT, 16);

		}
	}

};

// defino tres objetos para efectos
// --------------------------------------------------------------
// Circulo( cxc,  cyc,  crpm,  crev,  crad,  ccolor,  cdir)
Circulo cir1(10, 12, 255, 2, 6, CHSV(0, 255, 255), 0);
//cxc,cyc,bpm,crad,ccolor, status, cdir 
CircleBeat circleB1(10, 12, 20, 8, CHSV(200, 255, 255), 1, 0);
CircleBeat circleB2(10, 12, 32, 4, CHSV(120, 255, 255), 1, 1);
// ---------------------------------------------------------------



#include "LEDanimations.h"



// factores para mapear la matriz de 16x16 en la matriz fisica
uint8_t Fy = kMatrixHeight / CUSTOM_HEIGHT;
uint8_t Fx = kMatrixWidth / CUSTOM_WIDTH;

long Duracion;// para timer de funcion
long Previous_millis = 0;



// List of patterns to cycle through.  Each is defined as a separate function below.
// Each pattern is defined as a function that takes two uint8_t's; all of the pattern functions
// have to have the same signature in this setup, and two (hardcoded!) parameters can be 
// passed to each one: ( mark kriesgman demo reel arg2)
typedef void(*TwoArgumentPattern)(uint8_t arg1, uint8_t arg2);
typedef struct {
	TwoArgumentPattern mPattern;	// nombre de la funcion a llamar
	String mName;					// nombre que se mostrara del efecto
	uint8_t mArg1;
	uint8_t mArg2;
} TwoArgumentPatterWithArgumentValues;

TwoArgumentPatterWithArgumentValues gPatternsAndArguments[] = {
	{ OnOff			,"OnOff",		0	/*nada */	, 0 /*nada */ },
	{ Dots1			,"Dots1",		1	/*color1 */	, 1 /*color2 */ },
	{ Dots2			,"Dots2",		125 /*scale*/	, 0 /*no se usa*/},
	{ SlowMandala2,	"SlowMandala2",	127 /*dim*/		, 0 /*no se usa*/},
	{ SlowMandala3,	"SlowMandala3",	127 /*dim*/		, 0 /*no se usa*/},
	{ Mandala8		,"Mandala8",	110 /*dim*/		, 0 /*no se usa*/},
	{ MSGEQtest		,"MSGEQtest",	120 /* scale*/	, 0 /*no se usa*/ },
	{ MSGEQtest2	,"MSGEQtest2",	127 /*scale*/	, 200 /*color*/},
	{ MSGEQtest3	,"MSGEQtest3",	120 /*scale*/	, 100 /*color*/ },
	{ MSGEQtest4	,"MSGEQtest4",	240 /* dim*/	, 10 /* hmult*/},
	{ AudioSpiral	,"AudioSpiral",	130 /*color1*/	, 122 /*color*/},
	{ MSGEQtest5	,"MSGEQtest5",	120 /*dim*/		, 10 /*hmult*/},
	{ MSGEQtest6	,"MSGEQtest6",	170 /*dim*/		, 10 /*hmult*/ },
	{ MSGEQtest7	,"MSGEQtest7",	240 /*dim*/		, 10 /*hmult*/ },// revisar
	{ MSGEQtest8	,"MSGEQtest8",	240 /*dim*/		, 10 /*hmult*/ },// revisar
	{ MSGEQtest9	,"MSGEQtest9",	27 /*dim*/		, 255 /*s*/},
	{ CopyTest		,"CopyTest",	200 /* dim*/	, 10 /*hmult*/ },
	{ CopyTest2		,"CopyTest2",	200 /* dim*/	, 10 /*hmult*/ },
	{ Audio1		,"Audio1",		10 /*dim*/		,15 /*hmult*/},
	{ Audio2		,"Audio2",		120 /*dim*/		,15 /*hmult*/ },
	{ Audio3		,"Audio3",		255 /*dim*/		,27 /*hmult*/ },
	{ Audio4		,"Audio4",		12 /*dim*/		,32 /*hdiv*/ },
	{ CaleidoTest2	,"CaleidoTest2",	240 /* dim*/	,150 /* color_ofset*/},
	{ CaleidoTest1	,"CaleidoTest1",	240 /* dim*/	,32 /* hdiv*/ },
	{ Audio5		,"Audio5",		9 /*dim*/		,30 /* hmult*/},
	{ Audio6		,"Audio6",		200 /*dim*/		,10 /* hmult*/ },
	{ NoiseExample1	,"NoiseExample1",	1 /* hpow2*/	,255 /* s*/ },
	{ NoiseExample2	,"NoiseExample2",	100/*noisez*/	,100 /* scale*/},
	{ NoiseExample3	,"NoiseExample3",	100/*noisez*/	,100 /* scale*/ },
	{ NoiseExample4	,"NoiseExample4",	100/*noisez*/	,100 /* scale*/ },
	{ NoiseExample5	,"NoiseExample5",	100/*noisez*/	,254 /* scale300*/ },// 29
	{ NoiseExample6	,"NoiseExample6",	50/*hofset*/	,200 /* scale*/ },
	{ NoiseExample7	,"NoiseExample7",	0/*nada*/		,100 /* scale*/ },
	{ LedsNoise		,"LedsNoise",	0/* nada*/		,0	 /* nada1*/ },//32
	{ circulo1		,"Circulo1",	0/* nada*/		,0	 /* nada1*/ },
	{ anillos		,"anillos",	0/* nada*/		,0	 /* nada1*/ },
	{ HorizontalStripes		,"HorizontalStripes",	0/* nada*/		,0	 /* nada1*/ },
	{ diagonalFill		,"diagonalFill",	0/* nada*/		,0	 /* nada1*/ },
	{ HorMirror		,"HorMirror",	0/* nada*/		,0	 /* nada1*/ },
	{ VertMirror		,"VertMirror",	0/* nada*/		,0	 /* nada1*/ },
	{ QuadMirror		,"QuadMirror",	0/* nada*/		,0	 /* nada1*/ },
	{ circulo2		,"circulo2",	0/* nada*/		,0	 /* nada1*/ },
	{ one_color_allHSV ,"one_color_allHSV",	41/* h*/		,180	 /* b*/ },
	{ video				,"video"	,0 /* nada1*/		,0 /* nada2 */ }	,


	//las funciones de  demoreel 100 adaptarlas para matriz e incluir

};
const int numberOfPatterns = sizeof(gPatternsAndArguments) / sizeof(gPatternsAndArguments[0]);
#include "LEDWebsockets.h"



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
	// to enable arduino debuger
	Serial.setDebugOutput(true);

	// -------------Solo un ejemplo aun no lo estoy usando ----------------
	// Step 1: DEfinir Json buffer :Reserve memory space
	//
	StaticJsonBuffer<200> jsonBuffer;// Step 1: Reserve memory space

  // Step 2: Build object tree in memory

	JsonObject& root = jsonBuffer.createObject();
	root["sensorico1"] = "gps";
	root["time"] = 1351824120;

	JsonArray& miJarray = root.createNestedArray("JSONarray");
	miJarray.add(48.756080, 6);  // 6 is the number of decimals to print
	miJarray.add(2.302038, 6);   // if not specified, 2 digits are printed
	// FIN ejemplo de JSON ------------------------------------------------



   // firstContact();  // Connect with Processing. Hello, is anyone out there? (eliminado pues no uso el port serie con processing)

	// use the following line only when working with a 16*16
	// and delete everything in the function RenderCustomMatrix() 
	// at the end of the code; edit XY() to change your matrix layout
	// right now it is doing a serpentine mapping
	//FastLED.addLeds<CHIPSET, LED_PIN, COLOR_ORDER>(leds2, CUSTOM_NUM_LEDS);

	  //FastLED.addLeds<APA102,11,13,RGB,DATA_RATE_MHZ(10) >(leds,NUM_LEDS);
	 //FastLED.addLeds<LED_TYPE,DATA_PIN,CLK_PIN,COLOR_ORDER,DATA_RATE_MHZ(10)>(leds2, CUSTOM_NUM_LEDS).setCorrection(TypicalLEDStrip);

	LEDS.addLeds<LED_TYPE, DATA_PIN, CLK_PIN, COLOR_ORDER>(c_leds[0], c_leds.Size()).setCorrection(TypicalSMD5050);

	FastLED.setBrightness(BRIGHTNESS);
	set_max_power_in_volts_and_milliamps(5, MILLI_AMPERE);
	LEDColorCorrection{ TypicalSMD5050 };
	//FastLED.setDither(1);
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
	myparameter1 += EEPROM.read(5) * 256;


	delay(100);                                         //Delay needed, otherwise showcolor doesn't light up all leds or they produce errors after turning on the power supply - you will need to experiment
	LEDS.showColor(CHSV(myHue, mySaturation, myValue));
	// fin setup desde ledcontrol.8266 matriz-jqm

	// Si quiero siempre activar el AP entonces habiloitar esta linea
	//WiFi.disconnect(); // Borra ssid y password 

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

	setupWiFi(); // ejecuta el WIFIManager
	startSettingsServer();
	webSocket.begin();
	webSocket.onEvent(webSocketEvent);

	Serial.print("Programa FunkyControl JQM-Async  Iniciado  ");// debug

	FastLED.setBrightness(127);
	FastLED.clear(true);
	delay(200);
	FastLED.showColor(CRGB::Red);
	delay(200);
	FastLED.showColor(CRGB::Lime);
	delay(200);
	FastLED.showColor(CRGB::Blue);
	delay(200);
	FastLED.showColor(CRGB::White);
	delay(200);

	drawEstrella(10, 12, 2, 0, 10, CHSV(HUE_RED, 255, 255));
	blur2d((c_leds[0]), MATRIX_WIDTH, MATRIX_HEIGHT, 32);

	delay(2000);
	FastLED.clear(true);
	for (uint16_t i = 0; i <= 200; i++)
	{
		//DrawVentilador(10, 12, 5, i, CHSV(0, 255, 255));
		drawLineByAngle(10, 12, i, 0, 7, CHSV(i, 255, 255));
		drawLineByAngle(10, 12, i + 128, 0, 7, CHSV(i, 255, 255));
		FastLED.show();
		FastLED.delay(2);
		fadeToBlackBy(c_leds[0], 480, 32);
	}
	for (uint16_t i = 0; i <= 500; i++)
	{
		circleBeat();
		FastLED.show();
		FastLED.delay(1);
		fadeToBlackBy(c_leds[0], 480, 32);
	}


	// ---------------------------------

   // Actualiza la lista de efectos en la pagina web
	sendAll();

	Serial.println(" fin Setup ");

	// Websocket client setup , to connect with Heroku server
	// matrix data will come from this server
	// pending to implement a Heart beat with Heroku
	// herokuWs.beginSocketIO(herokuHost , 80); // for the  websocket client library

	//herokuWs.beginSocketIO("app-gnk-p5js.herokuapp.com", 80); // for socket.io  client library
	//herokuWs.beginSocketIO(herokuHost, 80);
	herokuWs.begin(herokuHost, herokuport);
   //webSocket.setAuthorization("user", "Password"); // HTTP Basic Authorization
	herokuWs.onEvent(wsVideoEvent);


}


/*
-------------------------------------------------------------------
 The main program
 -------------------------------------------------------------------
 */
void loop()
{
	EVERY_N_MILLISECONDS(500) {                           // FastLED based non-blocking delay to update/display the sequence.
		ledState = !ledState;
		digitalWrite(LED_BUILTIN, ledState);

	}
	h = hue;
	settingsServerTask();
	webSocket.loop();// handles websockets
	EVERY_N_SECONDS(5) {
		sendHeartBeat(); // envia el json de HB} al cliente WEB
	}


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
	//hue += 4;

	// send the 'leds' array out to the actual LED strip
   // FastLED.show(); // los tengo dentro de las funciones
	// insert a delay to keep the framerate modest
  //  FastLED.delay(1000 / 120); // about sixty FPS

	EVERY_N_SECONDS(101) { nextPattern(); } // change patterns periodically

	if (WiFi.status() == WL_CONNECTED) {
		herokuWs.loop(); // handles Heroku socket.io loop
		herokuHeartBeat(); // send HB to Heroku WS server
	}
	else {
		herokuWs.disconnect();
	} 

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

