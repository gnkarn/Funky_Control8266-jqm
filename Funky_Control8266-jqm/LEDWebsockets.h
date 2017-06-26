/* Based on the websocket example and
the code here http://www.whatimade.today/esp8266-on-websockets-mdns-ota-and-leds/ */

WebSocketsServer webSocket = WebSocketsServer(81);

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

		if (text.startsWith("x")) {      // on off
			String xVal = (text.substring(text.indexOf("x") + 1, text.length()));
			if (myOnOff != xVal.toInt()) {
				myOnOff = xVal.toInt();
				//EEPROM.write(1, myOnOff);
				lastChangeTime = millis();
				eepromCommitted = false;
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

void  wsVideoEvent(WStype_t type, uint8_t * payload, size_t length) {
	// this to receive a message from the Heroku Server 
	//const char* myjson = 
	
	DynamicJsonBuffer jsonBuffer(SENSORDATA_JSON_SIZE);
	const char* json = (char *)&payload[0];
	JsonObject& root = jsonBuffer.parseObject(json);

	const char* wsmsgName = root["msgName"]; // "msgVideo"
	int wstype = root["type"]; // 3
	JsonArray& wsmessage = root["message"];

	USE_SERIAL.printf("1 [WSc] get text: %s\n", root["message"]);

	String text = String((char *)&payload[0]);
	// do Json parse
	
	// 
	Serial.print("2 Received from Heroku (type): ");
	Serial.println( type );
	Serial.print("3 root[type]: ");
	Serial.println(int( root["type"]) );

	switch (type) {
	case WStype_TEXT:

		USE_SERIAL.printf("3 [%u] Received from Heroku (video): %s\n", &payload, &payload);
		String text = String((char *)&payload[0]);
		USE_SERIAL.printf("4 [%u] Received from Heroku (video): %s\n", length, text.c_str());
		// if current efect is video continue else jump

		if (text.startsWith("Y")) {      // video message
			String xVal = (text.substring(text.indexOf("Y") + 1, text.length()));

		}
		//parse JSON objet

		// transfer objet to ledMatrix

		//end

		// send message to client
		// webSocket.sendTXT(num, "message here");

		// send data to all connected clients
		// webSocket.broadcastTXT("message here");
		break;
	}
}

void  wsTimeEvent(const char * payload, size_t lenght) {
	yield();
	char text = payload[0];
	USE_SERIAL.printf("[%u] Received from Heroku (Time): %s\n", text);
}

void  wsConnectEvent(const char * payload, size_t lenght) {

	yield();
	char text = payload[0];
	USE_SERIAL.printf("[%u] Received from Heroku (connect): %s\n", text);
}

void wsDisconnectedEvent(const char * payload, size_t lenght) {
	char text = payload[0];
	USE_SERIAL.printf("[%u] disconnected from Heroku (disconnected): %s\n", text);
}
