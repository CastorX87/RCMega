#include <WebSockets.h>
#include <WebSocketsClient.h>
#include <WebSocketsServer.h>
#include <ESP8266WebServer.h>
#include <Arduino.h>
#include <Servo.h>

#define NODE_MCU_PIN_D5 14
#define NODE_MCU_PIN_D6 12
#define NODE_MCU_PIN_D7 13

//#define WIFI_MODE_AP

#ifndef WIFI_MODE_AP
  const char *ssid = "FB_Lakas_Nyomtat";
  const char *pass = "unusual01";
#else
  const char *ssid = "RCMega";
  const char *pass = "123456789";
#endif



Servo SteeringServo;
unsigned int LoopNo = 0;


ESP8266WebServer webServer(80); //Server on port 80
WebSocketsServer webSocket(81);

void SetMotorSpeed(float val) // val = [0,1]
{
  if(val == 0)
  {
    analogWrite(NODE_MCU_PIN_D6, 0);
    analogWrite(NODE_MCU_PIN_D5, 0);
    return;
  }

  bool forward = val > 0;
  val = min((float)fabs(val), 1.0f);
  int intVal = (int)(val * PWMRANGE);

  if(forward)
  {

    analogWrite(NODE_MCU_PIN_D6, 0);
    analogWrite(NODE_MCU_PIN_D5, intVal);
  } else
  {

    analogWrite(NODE_MCU_PIN_D5, 0);
    analogWrite(NODE_MCU_PIN_D6, intVal);
  }
}

void WSEventHandler(uint8_t num, WStype_t type, uint8_t * payload, size_t length)
{
  if(type == WStype_CONNECTED)
  {
    Serial.println("WSEventHandler - Client connected\n");
  }
  if (type == WStype_TEXT)
  {
    String s = String((char*)payload);
    Serial.println(s);
    String cmd = s.substring(0, 1);
    float param = s.substring(1).toFloat();
    Serial.println(String("CMD=") + cmd + " PARAM=" + param);
    if(cmd == String("A"))
    {
      if(fabs(param) < 0.2 )
        param = 0;
      SetMotorSpeed(param * 1.5f);
    }
    if(cmd == String("S"))
    {
      int valToWrite = (int)(90.0f + (80 * param));
      SteeringServo.write(valToWrite);
    }
  }
}

void handleRoot() {
  webServer.send(200, "text/html", "<!DOCTYPE HTML>\n<html>\n   <head>\n      \n      <script type = \"text/javascript\">\n\t\tfunction handleStart(evt)\n\t\t{\n\t\t\tvar touches = evt.changedTouches;\n\t\t\tvar el = document.getElementsByTagName(\"canvas\")[0];\n\t\t\t\n\t\t\tvar acc = -(touches[0].pageY - el.height / 2.0) / el.height * 2;\n\t\t\tvar steer = (touches[0].pageX - el.width / 2.0) / el.width * 2;\n\t\t\t\n\t\t\tSendCommand('A', acc.toFixed(2));\n\t\t\tSendCommand('S', steer.toFixed(2));\n\t\t}\n\t\tfunction handleMove(evt)\n\t\t{\n\t\t\tvar touches = evt.changedTouches;\n\t\t\tvar el = document.getElementsByTagName(\"canvas\")[0];\n\t\t\t\n\t\t\tvar acc = -(touches[0].pageY - el.height / 2.0) / el.height * 2;\n\t\t\tvar steer = (touches[0].pageX - el.width / 2.0) / el.width * 2;\n\t\t\t\n\t\t\tSendCommand('A', acc.toFixed(2));\n\t\t\tSendCommand('S', steer.toFixed(2));\n\t\t\t\n\t\t\tevt.preventDefault();\n\t\t}\nfunction startup() {\n  var el = document.getElementsByTagName(\"canvas\")[0];\n  el.addEventListener(\"touchstart\", handleStart, false);\n  //el.addEventListener(\"touchend\", handleEnd, false);\n  //el.addEventListener(\"touchcancel\", handleCancel, false);\n  el.addEventListener(\"touchmove\", handleMove, false);\n  // Get the canvas element form the page\n\n \n/* Rresize the canvas to occupy the full page, \n   by getting the widow width and height and setting it to canvas*/\n \nel.width  = window.innerWidth;\nel.height = window.innerHeight - 100;\n \n  console.log(\"initialized.\");\n}\n\t\tfunction draw()\n\t\t{\n\t\t\talert(\"aaa\");\n\t\t\tvar canvas = document.getElementById('mycanvas');\n \n\t\t\tif(canvas.getContext)\n\t\t\t{\n\t\t\t\tvar ctx = canvas.getContext(\"2d\");\n\t\t\t\tctx.fillRect (event.touches[0].pageX, event.touches[0].pageY, 5, 5);\n\t\t\t}\n\t\t}\n\t  \n\t\t var ws = null;\n\t\t \n\t\t function SendWSMessage(msgToSend)\n\t\t {\n\t\t\tws.send(msgToSend);\n\t\t }\n\t\t \n         function SendCommand(cmd, param)\n\t\t {\n            var toSend = cmd.concat(param);\n\t\t\tconsole.log(toSend);\n            if (\"WebSocket\" in window)\n\t\t\t{\n\t\t\t\t// Let us open a web socket\n\t\t\t    if(ws == null)\n\t\t\t    {\n\t\t\t\t\tws = new WebSocket(\"ws://192.160.178.29:81\");\n\t\t\t\t\n\t\t\t\t\tws.onopen = function()\n\t\t\t\t\t{\n\t\t\t\t\t};\n\t\t\t\t\t\n\t\t\t\t\tws.onmessage = function (evt)\n\t\t\t\t\t{\n\t\t\t\t\t\tvar received_msg = evt.data;\n\t\t\t\t\t};\n\t\t\t\t\t\n\t\t\t\t\tws.onclose = function()\n\t\t\t\t\t{ \n\t\t\t\t\t\talert(\"Connection is closed...\"); \n\t\t\t\t\t};\n\t\t\t\t}\n\t\t\t\tws.send(toSend);\n            }\n\t\t\telse\n\t\t\t{\n               alert(\"WebSocket NOT supported by your Browser!\");\n            }\n         }\n      </script>\n\t\t\n   </head>\n   \n   <body>\n\t\t<div id = \"sse\">\n\t\t\t<a href = \"javascript:SendCommand('F', '50')\">--------------</a>\n\t\t</div>\n<canvas id=\"canvas\" style=\"border:solid black 1px;\">\n  Your browser does not support canvas element.\n</canvas>\n<br>\n<button onclick=\"startup()\">Initialize</button>\n<br>\nLog: <pre id=\"log\" style=\"border: 1px solid #ccc;\"></pre>\n  </body>\n   </body>\n</html>");
}


void setup()
{
  // Remove cached WiFi data
  ESP.eraseConfig();
  WiFi.persistent(false);

  // Init serial connection
  Serial.begin(115200);
  while(!Serial);
  Serial.printf("------------------------------------------\n");

  // Wifi Init
  #ifdef WIFI_MODE_AP
    WiFi.mode(WIFI_AP);
    WiFi.softAP(ssid, pass);
  #else
    WiFi.begin(ssid, pass);
    Serial.printf("Connecting to %s: ", ssid);
    while (WiFi.status() != WL_CONNECTED) { delay(500);  Serial.printf("."); }
  #endif

  Serial.printf((String("  - LocalIP: ") + WiFi.localIP().toString()).c_str());
  Serial.printf((String("  - SoftAPIP: ") + WiFi.softAPIP().toString()).c_str());

  pinMode(NODE_MCU_PIN_D5, OUTPUT);
  pinMode(NODE_MCU_PIN_D6, OUTPUT);
  SetMotorSpeed(0);
  SteeringServo.attach(NODE_MCU_PIN_D7);

  SteeringServo.write(90);

  webServer.on("/", handleRoot);
  webSocket.onEvent(WSEventHandler);
  webServer.begin();
  analogWriteFreq(4000);
}

void loop()
{
  webSocket.loop();
  webServer.handleClient();
}
