#include <ESP8266WiFi.h>
#include <WebSocketsServer.h>
#include <ESP8266WebServer.h>
#include <Servo.h>

const uint8_t SERVO_CONTROL_PIN = D0;

// Replace these with your WiFi network settings
const char* ssid = "RCMega"; //replace this with your WiFi network name
const char* password = "123456789"; //replace this with your WiFi network password

ESP8266WebServer webServer(80);
WebSocketsServer WS_Server(81);
Servo SteeringServo;

float ControlValueToServoValue(uint8_t ctrlVal) { return (((float)ctrlVal) / 128 - 0.5f) * 2.0f; }
float ControlValueToMotorValue(uint8_t ctrlVal) { return (((float)ctrlVal) / 128 - 0.5f) * 2.0f; }

void SetServoPosition(float val) // val = [0,1]
{
  SteeringServo.write(90 + val * 65);
}

void SetMotorSpeed(float val) // val = [0,1]
{
  if(val == 0)
  {
    analogWrite(D6, 0);
    analogWrite(D5, 0);
    return;
  }

  bool forward = val > 0;
  val = min((float)fabs(val), 1.0f);
  int intVal = (int)(val * PWMRANGE);

  if(forward)
  {
    analogWrite(D5, 0);
    analogWrite(D6, intVal);
  } else
  {
    analogWrite(D6, 0);
    analogWrite(D5, intVal);
  }
}

void webSocketEvent(uint8_t clientId, WStype_t type, uint8_t * payload, size_t length)
{
	switch(type)
  {
		case WStype_DISCONNECTED: {
      Serial.printf("WStype_DISCONNECTED\n");
			break;
    }
		case WStype_CONNECTED: {
      Serial.printf("WStype_CONNECTED\n");
      break;
	  }
		case WStype_TEXT: {
      Serial.printf("WStype_TEXT\n");
			break;
    }
		case WStype_BIN: {
      float speedMotorVal = ControlValueToMotorValue(payload[0]);
      float steerServoVal = ControlValueToServoValue(payload[1]);

      SetMotorSpeed(speedMotorVal);
      SetServoPosition(steerServoVal);
			break;
    }
    default: {
      Serial.printf("WStype_BIN\n");
      break;
    }
  }
}

void handleRoot()
{
  String htmlText = "<!DOCTYPE HTML>\n<html>\n   <head>\n      \n      <script type = \"text/javascript\">\n\t\tfunction HANDLE_MOV(evt)\n\t\t{\n\t  \t\tvar touches = evt.changedTouches;\n\t\t\tvar el = document.getElementsByTagName(\"canvas\")[0];\n\t\t\tvar acc = -(touches[0].pageY - el.height / 2.0) / el.height * 2.3;\t\t\t\n\t\t\t\n\t\t\tif(acc > 1) acc = 1;\n\t\t\tif(acc < -1) acc = -1;\n\t\t\t\n\t\t\tif(touches[0].pageY > el.height)\n\t\t\t\tacc = -1;\n\t\t\t\n\t\t\tvar steer = (touches[0].pageX - el.width / 2.0) / el.width * 2;\n\t\t\tvar accInt = parseInt((acc / 2.0) * 128 + 64);\n\t\t\tvar steerInt = parseInt((steer / 2.0) * 128 + 64);\n\t\t\tSendCommandBin(accInt, steerInt);\n\t\t\tevt.preventDefault();\n\t\t}\n\t\t\n\t\tfunction handleStart(evt)\n\t\t{\n\t\t\tHANDLE_MOV(evt);\n\t\t}\n\t\t\n\t\tfunction handleMove(evt)\n\t\t{\n\t\t\tHANDLE_MOV(evt);\n\t\t}\n\t\tfunction startup()\n\t\t{\n\t\t\tvar el = document.getElementsByTagName(\"canvas\")[0];\n\t\t\tel.addEventListener(\"touchstart\", handleStart, false);\n\t\t\tel.addEventListener(\"touchmove\", handleMove, false);\n\t\t\tel.width  = window.innerWidth;\n\t\t\tel.height = window.innerHeight - 100;\n\t\t}\n\t\tfunction draw()\n\t\t{\n\t\t\talert(\"aaa\");\n\t\t\tvar canvas = document.getElementById('mycanvas');\n \n\t\t\tif(canvas.getContext)\n\t\t\t{\n\t\t\t\tvar ctx = canvas.getContext(\"2d\");\n\t\t\t\tctx.fillRect (event.touches[0].pageX, event.touches[0].pageY, 5, 5);\n\t\t\t}\n\t\t}\n\t  \n\t\t var ws = null;\n\t\t \n\t\t function SendWSMessage(msgToSend)\n\t\t {\n\t\t\tws.send(msgToSend);\n\t\t }\n\t\t \n         function SendCommandBin(accIntVal, steerIntVal)\n\t\t {\n            if (\"WebSocket\" in window)\n\t\t\t{\n\t\t\t    if(ws == null)\n\t\t\t    {\n\t\t\t\t\tws = new WebSocket(\"ws://<<IP_ADDRESS>>:81\");\n\t\t\t\t\tws.onopen = function() { };\n\t\t\t\t\tws.onmessage = function (evt) { var received_msg = evt.data; };\n\t\t\t\t\tws.onclose = function() { ws = null; };\n\t\t\t\t}\n\t\t\t\t\n\t\t\t\tvar myArray = new ArrayBuffer(2);\n\t\t\t\tvar longUInt8View = new Uint8Array(myArray);\n\t\t\t\tlongUInt8View[0] = accIntVal;\n\t\t\t\tlongUInt8View[1] = steerIntVal;\n\t\t\t\tws.send(myArray);\n            }\n\t\t\telse\n\t\t\t{\n               alert(\"WebSocket NOT supported by your Browser!\");\n            }\n         }\n\t\t \n         function SendCommand(cmd, param)\n\t\t {\n            var toSend = cmd.concat(param);\n            if (\"WebSocket\" in window)\n\t\t\t{\n\t\t\t\t// Let us open a web socket\n\t\t\t    if(ws == null)\n\t\t\t    {\n\t\t\t\t\tws = new WebSocket(\"ws://<<IP_ADDRESS>>:81\");\n\t\t\t\t\n\t\t\t\t\tws.onopen = function()\n\t\t\t\t\t{\n\t\t\t\t\t};\n\t\t\t\t\t\n\t\t\t\t\tws.onmessage = function (evt)\n\t\t\t\t\t{\n\t\t\t\t\t\tvar received_msg = evt.data;\n\t\t\t\t\t};\n\t\t\t\t\t\n\t\t\t\t\tws.onclose = function()\n\t\t\t\t\t{ \n\t\t\t\t\t\tws = null;\n\t\t\t\t\t\t//alert(\"Connection is closed...\"); \n\t\t\t\t\t};\n\t\t\t\t}\n\t\t\t\tws.send(toSend);\n            }\n\t\t\telse\n\t\t\t{\n               alert(\"WebSocket NOT supported by your Browser!\");\n            }\n         }\n      </script>\n\t\t\n   </head>\n   \n   <body>\n\t<canvas id=\"canvas\" style=\"border:solid black 1px;\">\n\t\tYour browser does not support canvas element.\n\t</canvas>\n\t<br>\n\t<button onclick=\"startup()\">Initialize</button>\n<br>\nLog: <pre id=\"log\" style=\"border: 1px solid #ccc;\"></pre>\n  </body>\n   </body>\n</html>";
  htmlText.replace("<<IP_ADDRESS>>", "192.168.4.1");
  webServer.send(200, "text/html", htmlText);
}


void setup()
{
  ESP.eraseConfig();
  WiFi.persistent(false);

  delay(1000);
  Serial.begin(115200);
 
  WiFi.mode(WIFI_AP);
  WiFi.softAP(ssid, password);

  //WiFi.begin(ssid, password);
  //Serial.println();
  //Serial.print("Connecting");
  //while (WiFi.status() != WL_CONNECTED)
  //{
    //delay(500);
    //Serial.print(".");
  //}

  Serial.println("success!");
  Serial.print("IP Address is: ");
  Serial.println(WiFi.localIP());

  // Creating WenServer
  WS_Server.onEvent(webSocketEvent);
  WS_Server.begin();

  // Setting up Servo
  SteeringServo.attach(SERVO_CONTROL_PIN);
  SteeringServo.write(90);

  // Setting up Motor Out
  pinMode(D5, OUTPUT);
  pinMode(D6, OUTPUT);
  analogWriteFreq(20000);

    // Creating WebServer
  webServer.on("/", handleRoot);
  webServer.begin();
}

void loop()
{
  WS_Server.loop();
  webServer.handleClient();
}