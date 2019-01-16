#include <WebSockets.h>
#include <WebSocketsClient.h>
#include <WebSocketsServer.h>
#include <Arduino.h>
#include <Servo.h>

#define NODE_MCU_PIN_D5 14
#define NODE_MCU_PIN_D6 12

Servo SteeringServo;
unsigned int LoopNo = 0;

WebSocketsServer NMcuWebSocketServer(81);
WiFiClient NMcuWifiClient;

const char *ssid = "FB_Lakas"; // The name of the Wi-Fi network that will be created
const char *pass = "UNUSUAL01";   // The password required to connect to it, leave blank for an open networ


WebSocketsServer webSocket = WebSocketsServer(81);


void WSEventHandler(uint8_t num, WStype_t type, uint8_t * payload, size_t length)
{
  if(type == WStype_CONNECTED)
  {
    Serial.println("WSEventHandler - Client connected\n");
  }
   if (type == WStype_TEXT){
    for(int i = 0; i < length; i++) Serial.print((char) payload[i]);
    Serial.println();
   }
}


void setup()
{
  ESP.eraseConfig();
  WiFi.persistent(false);
    
  Serial.begin(115200);
  while(!Serial);
  Serial.printf("------------------------------------------\n");
  Serial.printf("+ Executing setup\n");
  Serial.printf("  - Serial connection opened\n");

  // Servo and motors
  SteeringServo.attach(5);
  pinMode(NODE_MCU_PIN_D5, OUTPUT);
  pinMode(NODE_MCU_PIN_D6, OUTPUT);
  SetMotorSpeed(0);
  Serial.printf("  - Servo and motor are connected\n");

  // Connecting to WiFi
  Serial.printf("  - Connecting to Wireless network: ");
  WiFi.begin(ssid, pass);

  while (WiFi.status() != WL_CONNECTED) { delay(500); Serial.print("."); }
  Serial.printf("Connected\n");
  
  webSocket.begin();
  
  webSocket.onEvent(WSEventHandler);
  Serial.printf("  - Websocket event handler registered\n");
}

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

void loop()
{
  webSocket.loop();
}