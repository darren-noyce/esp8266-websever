// http://arduino.esp8266.com/stable/package_esp8266com_index.json

#include <Arduino.h>
#ifdef ESP32
#include <WiFi.h>
#include <AsyncTCP.h>
#elif defined(ESP8266)
#include <ESP8266WiFi.h>
#include <ESPAsyncTCP.h>
#endif
#include <ESPAsyncWebServer.h>
#include "Wire.h"
#include "Adafruit_PWMServoDriver.h"

#define MIN_PULSE_WIDTH 600
#define MAX_PULSE_WIDTH 2600
#define FREQUENCY 50

Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver(0x40);


const char* ssid = "NoyceIoT_main";
const char* password = "0183769177";

const int output = 5;

String sliderValue = "0";

const char* PARAM_SERVO = "servo";
const char* PARAM_ANGLE = "angle";

AsyncWebServer server(80);

const char index_html[] PROGMEM = R"rawliteral(

<!DOCTYPE HTML><html>
<head>
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <title>ESP8266 Web PWM Control Server</title>
  <style>
    html {font-family: Arial; display: inline-block;  margin: 0px auto; text-align: center;}
    h2 {font-size: 2.3rem;}
    p {font-size: 1.9rem;}
    body {max-width: 500px; margin:0px auto; padding-bottom: 25px;}
    .slidecontainer {
        width: 100%;
      }
      .slider {
        -webkit-appearance: none;
        width: 350px;
        height: 15px;
        background: grey;
        outline: none;
        opacity: 0.5;
        -webkit-transition: .2s;
        transition: opacity .2s;
      }
      .slider:hover {
        opacity: 2;
      }
      .slider::-webkit-slider-thumb {
        -webkit-appearance: none;
        appearance: none;
        width: 25px;
        height: 25px;
        background: magenta;
        cursor: pointer;
      }
      .slider::-moz-range-thumb {
        width: 25px;
        height: 25px;
        background: #4CAF50;
        cursor: pointer;
      }
  </style>
</head>
<body>
  <h2>ESP8266 Web PWM Control Server</h2>
  <p><span id="textSliderValue">%SLIDERVALUE%</span></p>
  <p><input type="range" onchange="updateSliderPWM(this)" id="pwmSlider" min="0" max="180" value="%SLIDERVALUE%" step="1" class="slider"></p>
<script>
function updateSliderPWM(element) {
  var sliderValue = document.getElementById("pwmSlider").value;
  document.getElementById("textSliderValue").innerHTML = sliderValue;
  console.log(sliderValue);
  var xhr = new XMLHttpRequest();
  xhr.open("GET", "/slider?value="+sliderValue, true);
  xhr.send();
}
</script>
</body>
</html>
)rawliteral";
String processor(const String& var){
  //Serial.println(var);
  if (var == "SLIDERVALUE"){
    return sliderValue;
  }
  return String();

}


void setup() {
    pwm.begin();
    pwm.setPWMFreq(FREQUENCY);
    Serial.begin(115200);
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, password);
    if (WiFi.waitForConnectResult() != WL_CONNECTED) {
        Serial.printf("WiFi Failed!\n");
        return;
    }
  analogWrite(output, sliderValue.toInt());
    Serial.print("IP Address: ");
    Serial.println(WiFi.localIP());

   
  
server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/html", index_html, processor);

   
        String servo;
        String angle;
        int servo_int;
        int angle_int;
        if (request->hasParam(PARAM_SERVO)) 
        {
          if (request->hasParam(PARAM_ANGLE)) 
          {
            servo = request->getParam(PARAM_SERVO)->value();
            angle = request->getParam(PARAM_ANGLE)->value();
            Serial.println(servo);
            Serial.println(angle);
            servo_int = servo.toInt();
            angle_int = angle.toInt();
            String message = "Set servo "+servo+" to "+angle+" degrees.";
            request->send(200, "text/plain", message);
          }
        }
        

   

  });
  server.on("/slider", HTTP_GET, [] (AsyncWebServerRequest *request) {

    String inputMessage;

    // GET input1 value on <ESP_IP>/slider?value=<inputMessage>

    if (request->hasParam(PARAM_SERVO)) {

      inputMessage = request->getParam(PARAM_ANGLE)->value();

      sliderValue = inputMessage;

      analogWrite(output, sliderValue.toInt());

    }

    else {

      inputMessage = "No message sent";

    }

    Serial.println(inputMessage);

    request->send(200, "text/plain", "OK");
        /**
         
        if (request->hasParam(PARAM_SERVO)) 
        {
          if (request->hasParam(PARAM_ANGLE)) 
          {
            servo = request->getParam(PARAM_SERVO)->value();
            angle = request->getParam(PARAM_ANGLE)->value();
            pwm.setPWM(servo, 0, pulseWidth(angle));
            String message = "Set servo "+servo+" to "+angle+" degrees.";
            request->send(200, "text/plain", message);
          }
          else 
          {
            request->send(200, "text/plain", "Missing angle.");
          }
        } 
        else 
        {
          request->send(200, "text/plain", "Missing servo.");
        }
        *
         */
    });

    
}

void loop (){
}




 
