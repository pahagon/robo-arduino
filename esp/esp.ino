#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <WebSocketsServer.h>

WebSocketsServer webSocket(8765);
ESP8266WebServer server(80);

// HTML content to be served
const char* indexHtmlPage = R"=====(
<!DOCTYPE html>
<html lang="en">
  <head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Robô Arthur e Thomas</title>
  </head>
  <body>
    <h1>Robô Arthur e Thomas</h1>
    <button id="forwards">Go forwards</button>
    <button id="backwards">Go backwards</button>
    <div id="response"></div>
    <script>
      const forwards = document.getElementById('forwards');
      const backwards = document.getElementById('backwards');
      const response = document.getElementById('response');

      let ws;
      let pingInterval;
      const pingTimeout = 5000;  // 5 seconds for example

      function connect() {
          ws = new WebSocket(`ws://${window.location.hostname}:8765`);
          ws.onopen = () => {
              console.log('Connected');
              startHeartbeat();
          };
          ws.onmessage = (event) => {
              if (event.data === "pong") {
                  console.log("Received pong from server.");
              } else {
                  response.innerText = `Server says: ${event.data}`;
              }
          };
          ws.onclose = () => {
              console.log('Disconnected');
              stopHeartbeat();
              setTimeout(connect, 1000);  // Try to reconnect after 1 second
          };
      }

      forwards.addEventListener('click', () => {
          if (ws.readyState === WebSocket.OPEN) {
              ws.send(1);
          } else {
              console.error("WebSocket connection is not open.");
          }
      });

      backwards.addEventListener('click', () => {
          if (ws.readyState === WebSocket.OPEN) {
              ws.send(0);
          } else {
              console.error("WebSocket connection is not open.");
          }
      });

      function startHeartbeat() {
          pingInterval = setInterval(function() {
              if (ws.readyState === WebSocket.OPEN) {
                  ws.send('ping');
              }
          }, pingTimeout);
      }

      function stopHeartbeat() {
          clearInterval(pingInterval);
      }
      
      connect();
    </script>
  </body>
</html>
)=====";

void handleRequest() {
  server.send(200, "text/html", indexHtmlPage);
}

void webSocketEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t length) {
  switch(type) {
    case WStype_DISCONNECTED:
      break;
    case WStype_CONNECTED:
      {
        IPAddress ip = webSocket.remoteIP(num);
        webSocket.sendTXT(num, "Hello from ESP8266");
      }
      break;
    case WStype_TEXT:
      if (strcmp((const char*)payload, "ping") == 0) {
        webSocket.sendTXT(num, "pong");
      } else {
        Serial.println((const char*)payload);
        webSocket.sendTXT(num, payload);
      }
      break;
    case WStype_BIN:
      break;
  }
}

const char* ssid = "";
const char* password = "";

const IPAddress local_IP(192,168,15,14);
const IPAddress gateway(192,168,15,1);
const IPAddress subnet(255,255,255,0);

void setup() {
  Serial.begin(115200);
  delay(10);

  if (WiFi.config(local_IP, gateway, subnet)) {

    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
      delay(500);
    }
  }

  server.on("/", handleRequest);
  server.begin();

  webSocket.begin();
  webSocket.onEvent(webSocketEvent);
}

void loop() {
  server.handleClient();
  webSocket.loop();
}
