#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <WebSocketsServer.h>

//#define DEBUG

WebSocketsServer webSocket(8765);
ESP8266WebServer server(80);

// HTML content to be served
const char* indexHtmlPage = R"=====(
INDEXHTML
)=====";

void handleRequest() {
  server.send(200, "text/html", indexHtmlPage);
}

void webSocketEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t length) {
  switch(type) {
    case WStype_CONNECTED:
      webSocket.sendTXT(num, "Hello from ESP8266");
      break;
    case WStype_DISCONNECTED:
      webSocket.sendTXT(num, "Bye from ESP8266");
      break;
    case WStype_BIN:
      webSocket.sendTXT(num, payload);
      break;
    case WStype_TEXT:
      if (strcmp((const char*)payload, "ping") == 0) {
        webSocket.sendTXT(num, "pong");
      } else {
        String angleAndSpeed = String((char*) payload);
        int endIndex = angleAndSpeed.indexOf(',');
        String first = angleAndSpeed.substring(0, endIndex);
        String second = angleAndSpeed.substring(endIndex+1, angleAndSpeed.length());

        uint8_t angle = first.toInt();
        uint8_t speed = second.toInt();
        speed = map(speed, 0, 100, 0, 255);
        uint speedByAngle = (speed * (1.0 - cos(angle * PI / 180.0) / 2.0));

        uint8_t leftEngineSpeedIn1  = 0;
        uint8_t leftEngineSpeedIn2  = 0;
        uint8_t rightEngineSpeedIn1 = 0;
        uint8_t rightEngineSpeedIn2 = 0;

        int quadrant = 0;
        if(angle < 90) {
          quadrant = 1;
        } else if(angle < 180) {
          quadrant = 2;
        } else if(angle < 270) {
          quadrant = 3;
        } else {
          quadrant = 4;
        }

        if (quadrant == 0) {
          leftEngineSpeedIn1 = 0;
          leftEngineSpeedIn2 = 0;

          rightEngineSpeedIn1 = 0;
          rightEngineSpeedIn2 = 0;
        } else if(quadrant == 1) {
          leftEngineSpeedIn1 = speedByAngle;
          leftEngineSpeedIn2 = 0;

          rightEngineSpeedIn1 = speed;
          rightEngineSpeedIn2 = 0;
        } else if(quadrant == 2) {
          leftEngineSpeedIn1 = speed;
          leftEngineSpeedIn2 = 0;

          rightEngineSpeedIn1 = speedByAngle;
          rightEngineSpeedIn2 = 0;
        } else if(quadrant == 3) {
          leftEngineSpeedIn1 = speed;
          leftEngineSpeedIn2 = 0;

          rightEngineSpeedIn1 = speedByAngle;
          rightEngineSpeedIn2 = 0;
        } else if(quadrant == 4) {
          leftEngineSpeedIn1 = speedByAngle;
          leftEngineSpeedIn2 = 0;

          rightEngineSpeedIn1 = speed;
          rightEngineSpeedIn2 = 0;
        }

        byte enginesSpeed[4] = {
          leftEngineSpeedIn1, leftEngineSpeedIn2,
          rightEngineSpeedIn1, rightEngineSpeedIn2
        };

        #ifdef DEBUG
        Serial.print(F("leftEngineSpeedIn1("));
        Serial.print(leftEngineSpeedIn1);
        Serial.print(F(")"));

        Serial.print(F("leftEngineSpeedIn2("));
        Serial.print(leftEngineSpeedIn2);
        Serial.print(F(")"));

        Serial.print(F("rightEngineSpeedIn1("));
        Serial.print(rightEngineSpeedIn1);
        Serial.print(F(")"));

        Serial.print(F("rightEngineSpeedIn2("));
        Serial.print(rightEngineSpeedIn2);
        Serial.print(F(")"));
        #endif

        Serial.write(enginesSpeed, 4);
        Serial.flush();

        #ifdef DEBUG
        angleAndSpeed +=
          String(leftEngineSpeedIn1)  + "," + String(leftEngineSpeedIn2) + "," +
          String(rightEngineSpeedIn2) + "," + String(rightEngineSpeedIn2);
        #endif
        webSocket.sendTXT(num, angleAndSpeed);
      }
      break;
  }
}

const char* ssid = "SSID";
const char* password = "PASSWD";

const IPAddress local_IP(192,168,15,14);
const IPAddress gateway(192,168,15,1);
const IPAddress subnet(255,255,255,0);

void wiFiShouldConnect() {
  if (WiFi.status() != WL_CONNECTED) {
    while (WiFi.config(local_IP, gateway, subnet) == 0) {
      delay(10);
      Serial.print(F("c"));
    }

    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(F("s"));
    }
  }
}

void setup() {
  Serial.begin(115200);
  while (!Serial) {};

  wiFiShouldConnect();

  server.on("/", handleRequest);
  server.begin();

  webSocket.onEvent(webSocketEvent);
  webSocket.begin();
}

void loop() {
  wiFiShouldConnect();
  server.handleClient();
  webSocket.loop();
}
