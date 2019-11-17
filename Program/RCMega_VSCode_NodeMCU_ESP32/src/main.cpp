#include <WebSocketsServer.h>

// Replace these with your WiFi network settings
const char* ssid = "FB_Lakas_Nyomtat"; //replace this with your WiFi network name
const char* password = "unusual01"; //replace this with your WiFi network password

WebSocketsServer WS_Server(81);

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
      //Serial.printf("WStype_BIN\n");
      //Serial.printf((String("accInt=") + payload[0] + "\n").c_str());
      //Serial.printf((String("steerInt=") + payload[1] + "\n").c_str());
			break;
    }
    default: {
      Serial.printf("WStype_BIN\n");
      break;
    }
  }
}

void setup()
{
  delay(1000);
  Serial.begin(115200);
 
  WiFi.begin(ssid, password);

  Serial.println();
  Serial.print("Connecting");
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }

  Serial.println("success!");
  Serial.print("IP Address is: ");
  Serial.println(WiFi.localIP());

  // Creating WebServer
  WS_Server.onEvent(webSocketEvent);
  WS_Server.begin();
}

void loop()
{
  WS_Server.loop();
}