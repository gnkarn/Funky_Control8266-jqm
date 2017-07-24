# Funky_Control8266-jqm
apa102  fastled esp8266 and jqm
It is a LED matrix  20 x 24 , using APA102 leds , 
Effects : multiple effects can be selected from a Web App , hosted on the same ESP8266, form any web browser
Also parameters and other status info is exchanged using Websockets Async

A new video effect was added  : a web app hosted on a Heroku server running a Nodejs server capture a live video from the computer cam , and sends  the low resolution frame using websockets .
Heroku broadcast each frame to any connected client , The ESP responds to it by transfering the image to the LED matrix , and sending back an ack to Heroku , asking for other frame .

TODO
