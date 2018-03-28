/* Based on the websocket example and
the code here http://www.whatimade.today/esp8266-on-websockets-mdns-ota-and-leds/ */

// also for debiging this line can be enabled on wesockets.h
// #define DEBUG_WEBSOCKETS(...) os_printf( __VA_ARGS__ ) // !!! enabled only for debuging

WebSocketsServer webSocket = WebSocketsServer(81);

void processResponse(const char* response , size_t length) {
	//DynamicJsonBuffer jsonBuffer(SENSORDATA_JSON_SIZE);
	const size_t bufferSize = 64 * JSON_ARRAY_SIZE(3) + JSON_ARRAY_SIZE(64) + JSON_OBJECT_SIZE(3) + 720; 64 * JSON_ARRAY_SIZE(3) + JSON_ARRAY_SIZE(64) + JSON_OBJECT_SIZE(3) + 1000; ; // JSON_OBJECT_SIZE(3) + 50;
	DynamicJsonBuffer jsonBuffer(bufferSize);
	
	// Format of date message 42["time","{\"msgName\":\"time\",\"type\":3,\"message\":\"18:15:13 GMT+0000 (UTC)\"}"]

	//String json = String ((char *)payload[0]);
	String text = String ((char *)response);

	//int pos = text.indexOf('{');      // position of { json object start
	//String json2 = text.substring(pos, text.length() - 2) ;     //* get from "�{" to the jason obj  end
	//json2 = json2.c_str(); // con IO
	String json2 =  (text );

	//Serial.print("json2 & heap:");
	//Serial.println(json2);
	//Serial.print(ESP.getFreeHeap(),DEC);

	//const char* json1 = "{\"msgName\":\"time\",\"type\":3,\"message\":\"22:40:49 GMT+0000 (UTC)\"}"; // for testing only																	 //USE_SERIAL.printf("0 json2: %s\n", text.c_str());

	//JsonArray& root = jsonBuffer.parseArray(response); // anstes json2
	JsonObject& root = jsonBuffer.parseObject(response); // anstes json2
	if (root.success())
	{
		//Serial.println(json1);
		msgName = root["msgName"]; // "Heroku message Name"
		int wstype = root["type"]; // 3
		const char* message = root["message"]; // "22:40:49 GMT+0000 (UTC)"
		USE_SERIAL.printf("a [WSc] name : %s\n", msgName);
		//Serial.println(wstype);
		USE_SERIAL.printf("c [WSc] message: %s\n", message);
		//Serial.print("3 root[type]: ");
		//Serial.println(wstype);
	}
	//JsonObject& root = jsonBuffer.parseObject(json1);// *
	Serial.print(" Parse success ? "); // parse success?
	Serial.println(root.success()); // parse success?
	//USE_SERIAL.printf("[WSc] get text: %s\n", response);
}



void webSocketEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t length) {

	switch (type) {
	case WStype_DISCONNECTED:
		USE_SERIAL.printf("[%u] Disconnected!\n", num);
		break;
	case WStype_CONNECTED:
	{
		IPAddress ip = webSocket.remoteIP(num);
		USE_SERIAL.printf("[%u] Connected from %d.%d.%d.%d url: %s\n", num, ip[0], ip[1], ip[2], ip[3], payload);

		// send message to client
		String websocketStatusMessage = "H" + String(myHue) + ",S" + String(mySaturation) + ",V" + String(myValue) + ",W" + String(myparameter1); //Sends a string with the HSV and white led  values to the client website when the conection gets established
		webSocket.sendTXT(num, websocketStatusMessage);

		String info = ESP.getResetInfo();
		webSocket.sendTXT(num, info); //Handy for debugging
		sendAll(); // en caso de conexion actualiza pagina web con la lista de fectos
	}
	break;
	case WStype_TEXT:
	{
		USE_SERIAL.printf("[%u] get Text: %s\n", num, payload);

		// send message to client
		// webSocket.sendTXT(num, "message here");

		// send data to all connected clients
		// webSocket.broadcastTXT("message here");

		String text = String((char *)&payload[0]);

		if (text.startsWith("x")) {      // on off(x1) or auto switch (x2)
			uint8_t xswitch = (text.substring(text.indexOf("x") + 1, text.length()-1)).toInt(); // if X1 , then xval = "1"
			uint8_t  xVal = (text.substring(text.indexOf("x") + 2, text.length())).toInt(); // value 0 or 1 
			//Serial.println(text); // debug
			//Serial.println(xswitch);
			//Serial.println(xVal);

			switch (xswitch)
			{
			case 1:
				if (myOnOff != xVal) {
					myOnOff = xVal;
					//EEPROM.write(1, myOnOff);
					lastChangeTime = millis();
					eepromCommitted = false;
				}
				break;
			case 2:
				if (myMode != xVal) {
					myMode = xVal;
					//EEPROM.write(1, myMode);
					lastChangeTime = millis();
					eepromCommitted = false;
				}
				break;
			default:
				myMode = 1;
				myOnOff = 0;
			}
		}
		if (text.startsWith("a")) {      // efectos
			String xVal = (text.substring(text.indexOf("a") + 1, text.length()));
			flickerLed = random(0, NUM_LEDS - 1);
			if (myEffect != xVal.toInt()) {   // only do stuff when there was a change
				myEffect = xVal.toInt();
				gCurrentPatternNumber = myEffect; // toma el numero de funcion enviada por websocket
				rainbowHue = myHue;
				// EEPROM.write(0, myEffect);       //stores the variable but needs to be committed to EEPROM before being saved - this happens in the loop
				lastChangeTime = millis();
				eepromCommitted = false;
				sendAll(); // hay que separar la actualizacion de la lista de este feedback 
			}
		}
		if (text.startsWith("b")) {      // Hue
			String xVal = (text.substring(text.indexOf("b") + 1, text.length()));
			if (myHue != xVal.toInt()) {
				myHue = xVal.toInt();
				rainbowHue = myHue;
				// EEPROM.write(1, myHue);
				lastChangeTime = millis();
				eepromCommitted = false;
			}
		}
		if (text.startsWith("c")) {     //Sat
			String xVal = (text.substring(text.indexOf("c") + 1, text.length()));
			if (mySaturation != xVal.toInt()) {
				mySaturation = xVal.toInt();
				//EEPROM.write(2, mySaturation);
				lastChangeTime = millis();
				eepromCommitted = false;
			}
		}
		if (text.startsWith("d")) {     // Value
			String xVal = (text.substring(text.indexOf("d") + 1, text.length()));
			if (myValue != xVal.toInt()) {
				myValue = xVal.toInt();
				//EEPROM.write(3, myValue);
				lastChangeTime = millis();
				eepromCommitted = false;
			}
		}
		if (text.startsWith("e")) {   // parametro de efectos
			String xVal = (text.substring(text.indexOf("e") + 1, text.length()));
			if (myparameter1 != xVal.toInt()) {
				myparameter1 = xVal.toInt();
				//EEPROM.write(4, myparameter1&&255);
				//EEPROM.write(5, myparameter1/256);
				lastChangeTime = millis();
				eepromCommitted = false;
			}
		}

		if (text.startsWith("y")) {   // Brightness
			String xVal = (text.substring(text.indexOf("y") + 1, text.length()));
			if (myBrightness != xVal.toInt()) {
				myBrightness = xVal.toInt();
				FastLED.setBrightness(myBrightness);
				//EEPROM.write(4, myBrightness&&255);
				//EEPROM.write(5, myBrightness/256);
				lastChangeTime = millis();
				eepromCommitted = false;
			}
		}

		if (text.startsWith("w")) {   // wheel color picker aca hacer la conversion del texto hex recibido a rgb , usando strtol()

	 //uint32_t xVal = (uint32_t)strtol((const char *)&payload[1], NULL, 16);
			String xVal = (text.substring(text.indexOf("w") + 1, text.length())); // el string es "w hsv(255,255,255)"
			if (myrgb != xVal) {
				// detecta los tres componentes y los transforma en numero
				myrgb = xVal;

				int ini = xVal.indexOf("(");
				int aux = xVal.indexOf(",", ini);
				myHue = xVal.substring(ini + 1, aux).toInt();

				ini = aux + 1;
				aux = xVal.indexOf(",", aux + 1);
				mySaturation = xVal.substring(ini, aux).toInt();

				ini = aux + 1;
				myValue = xVal.substring(ini, xVal.length() - 1).toInt();
				// convertir rgb a HSV , y escribirlos en my HSV variables .
				USE_SERIAL.printf("[%u] HSV : %u  %u %u\n", num, myHue, mySaturation, myValue);

				//EEPROM.write(4, myrgb&&255);
				//EEPROM.write(5, myrgb/256);
				lastChangeTime = millis();
				eepromCommitted = false;
			}
		}
		/*currentTime = millis();
		 if (currentTime - previousTime > 1000) {
		   String websocketStatusMessage = "H" + String(myHue) + ",S" + String(mySaturation) + ",V" + String(myValue);
		   webSocket.broadcastTXT(websocketStatusMessage);
		   previousTime = currentTime;
		  }*/
		break;
	}
	case WStype_BIN:
		//USE_SERIAL.printf("[%u] get binary lenght: %u\n", num, length);
		hexdump(payload, length);

		// send message to client
		// webSocket.sendBIN(num, payload, lenght);
		break;
	}

}


