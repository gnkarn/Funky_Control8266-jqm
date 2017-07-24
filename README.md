# Funky_Control8266-jqm
apa102  fastled esp8266 and jqm
please see : https://github.com/gnkarn/Funky_Control8266-jqm/wiki
It is a LED matrix  20 x 24 , using APA102 leds , 
Effects : multiple effects can be selected from a Web App , hosted on the same ESP8266, form any web browser
Also parameters and other status info is exchanged using Websockets Async

A new video effect was added  : a web app hosted on a Heroku server running a Nodejs server capture a live video from the computer cam , and sends  the low resolution frame using websockets .
Heroku broadcast each frame to any connected client , The ESP responds to it by transfering the image to the LED matrix , and sending back an ack to Heroku , asking for other frame .

TODO

make corrections on video colors adjusting Gamma
clean code , delete unused parts
improve code comments .
Adjust each effect and its parameters
send the parameters correct range by effect , to the WEB app, so min and max makes sense for each effect
change parameters name according to the effect on the Web app ( dynamic HTML)
Replace color picker , correct HSV calculation , see this one http://www.dematte.at/colorPicker/ .
The web socket server and client , are not reliable now , so improve connect and disconnect code .
include all the other effects.
some effects , are developed on a 16 by 16 matrix and then translated to the custom matrix , change , in order to accept the custom matrix directly
verify sound parameters
include microphone
include parameters , for speed , and noise adjustment


