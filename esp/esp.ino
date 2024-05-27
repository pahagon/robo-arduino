#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <WebSocketsServer.h>

ESP8266WebServer server(80);
WebSocketsServer webSocket = WebSocketsServer(8765);

// HTML content to be served
const char* htmlPage = R"=====(
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
  server.send(200, "text/html", htmlPage);
}

void webSocketEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t length) {
  switch(type) {
    case WStype_DISCONNECTED:
      Serial.printf("[%u] Disconnected!\n", num);
      break;
    case WStype_CONNECTED:
      {
        IPAddress ip = webSocket.remoteIP(num);
        Serial.printf("[%u] Connection from %s\n", num, ip.toString().c_str());
        webSocket.sendTXT(num, "Hello from ESP8266");
      }
      break;
    case WStype_TEXT:
      Serial.printf("[%u] Received text: %s\n", num, payload);
      if (strcmp((const char*)payload, "ping") == 0) {
        webSocket.sendTXT(num, "pong");
      } else {
        webSocket.sendTXT(num, payload);
      }
      break;
    case WStype_BIN:
      Serial.printf("[%u] Received binary data\n", num);
      break;
  }
}

// Replace with your network credentials
const char* ssid = "Ahagon-Vivo";
const char* password = "0123456789Paulo";

void setup() {
  Serial.begin(115200);
  delay(10);

  // Connect to Wi-Fi
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  // Start the server
  server.on("/", handleRequest);
  server.begin();
  Serial.println("HTTP server started");

  // Start the websocket
  webSocket.begin();
  webSocket.onEvent(webSocketEvent);
  Serial.println("websocket server started");
}

void loop() {
  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("WiFi connected");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());
  } else {
    Serial.println(":(");
  }

  server.handleClient();
  webSocket.loop();
}
