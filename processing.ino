/****The below two functions are needed for sending to Processing. */
// Copy from here to the bottom.
//--------------------
// Waits for Processing to respond and then sends the number of pixels.
void firstContact() {
  uint16_t nLEDS = NUM_LEDS;  // Number to send to Processing.  (Allows up to 65,535 pixels)
    while (Serial.available() <= 0) {  // Repeats until Processing responds. Hello?
      Serial.print('A');  // send an ASCII A (byte of value 65)
      delay(100);
    }
    // Once Processing responds send the number of pixels as two bytes.
    rawSerial= Serial.read();  // Send the high byte to Processing.
    Serial.write(rawSerial);  // Send the low byte to Processing.
    Serial.print('#');  // send an ASCII # (byte of value 35) as a flag for Processing.
    linkedUp = true;  // Now that Processing knows number of pixels set linkedUp to true.
    delay(500);
  
}

//--------------------
// This function sends ALL the pixel data to Processing.
void SendToProcessing() {
 /* if (testing == true){  // Print pixel data. If NUM_LEDS is large this will be a lot of data!
    Serial.println("-------------------------------------------------------");
    for (uint16_t d=0; d < NUM_LEDS; d++){
      Serial.print("p: "); Serial.print(d);
      Serial.print("\tr: "); Serial.print(leds[d].r);
      Serial.print("\tg: "); Serial.print(leds[d].g);
      Serial.print("\tb: "); Serial.println(leds[d].b);
    }
    Serial.println(" ");
    delay(500);  // Add some extra delay while testing.
  }
  */
    // Send ALL the pixel data to Processing!
    for (uint16_t d=0; d < NUM_LEDS; d++){
      //Serial.write(d);          // Pixel number
      //Serial.write(leds[d].r);  // Red channel data
      //Serial.write(leds[d].g);  // Green channel data
      //Serial.write(leds[d].b);  // Blue channel data
    }
}

//--------------------
/****End of the functions needed for sending to Processing.*/

void ReadFromProcessing(){
  if (Serial.available()>0){
    rawSerial=Serial.read();
    FastLED.setBrightness(rawSerial );
  }
  }


