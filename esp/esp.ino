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
    <div><canvas id="canvas" name="game" ></canvas></div>
    <div id="response"></div>
    <script>
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
              response.innerText = `Server says: ${event.data}`;
          };
          ws.onclose = () => {
              console.log('Disconnected');
              stopHeartbeat();
              setTimeout(connect, 1000);  // Try to reconnect after 1 second
          };
          ws.onerror = (error) => {
              console.error('WebSocket error:', error);
          };
      }

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

      var canvas, ctx, width, height, radius, x_orig, y_orig, largura, altura;
      function resize() {

          largura = window.innerWidth
              || document.documentElement.clientWidth
              || document.body.clientWidth;
          altura = window.innerHeight
              || document.documentElement.clientHeight
              || document.body.clientHeight;

          if (largura > altura) {
              width = largura * 0.3646;
              radius = width * 0.2;
              height = altura * 0.65;
          }

          if (altura > largura) {
              width = largura;
              radius = width * 0.23
              height = largura;
          }

          ctx.canvas.width = width;
          ctx.canvas.height = height;
          background();
          joystick(width / 2, height / 2);
      }

      function background() {
          x_orig = width / 2;
          y_orig = height / 2;
          ctx.beginPath();
          ctx.arc(x_orig, y_orig, radius + 20, 0, Math.PI * 2, true);
          ctx.fillStyle = '#0e37cd';
          ctx.fill();
      }

      function joystick(width, height) {
          ctx.beginPath();
          ctx.arc(width, height, radius, 0, Math.PI * 2, true);
          ctx.fillStyle = '#ffffff';
          ctx.fill();
          ctx.strokeStyle = '#4464d9';
          ctx.lineWidth = 8;
          ctx.stroke();
      }

      window.addEventListener("load", () => {
          (canvas = document.getElementById("canvas")),
          (ctx = canvas.getContext("2d")),
          resize(),
          document.addEventListener("mousedown", startDrawing),
          document.addEventListener("mouseup", stopDrawing),
          document.addEventListener("mousemove", Draw),
          document.addEventListener("touchstart", startDrawing),
          document.addEventListener("touchend", stopDrawing),
          document.addEventListener("touchcancel", stopDrawing),
          document.addEventListener("touchmove", Draw),
          window.addEventListener("resize", resize);
      });

      let coord = { x: 0, y: 0 };
      let paint = !1;

      function getPosition(event) {
          e = window.event || e;
          var mouse_x = e.clientX || e.touches[0].clientX;
          var mouse_y = e.clientY || e.touches[0].clientY;
          coord.x = mouse_x - canvas.offsetLeft;
          coord.y = mouse_y - canvas.offsetTop;
      }

      function is_it_in_the_circle() {
          var current_radius = Math.sqrt(Math.pow(coord.x - x_orig, 2) + Math.pow(coord.y - y_orig, 2));
          if (radius >= current_radius) return true
          else return false
      }

      function startDrawing(event) {
          paint = true;
          getPosition(event);
          if (is_it_in_the_circle()) {
              ctx.clearRect(0, 0, canvas.width, canvas.height);
              background();
              joystick(coord.x, coord.y);
              Draw();
          }
      }

      function stopDrawing() {
          paint = false;
          ctx.clearRect(0, 0, canvas.width, canvas.height);
          background();
          joystick(width / 2, height / 2);
          if (ws.readyState === WebSocket.OPEN) {
              ws.send("0,0");
          } else {
              console.error("WebSocket connection is not open.");
          }
      }

      function Draw(t) {
          if (paint) {
              var velocidade, angulo_graus, angulo_rad, x_abs, y_abs;
              ctx.clearRect(0, 0, canvas.width, canvas.height), background();
              angulo_rad = Math.atan2(coord.y - y_orig, coord.x - x_orig);

              (angulo_graus = -1 == Math.sign(angulo_rad) ? Math.round((180 * -angulo_rad) / Math.PI)
               : Math.round(360 - (180 * angulo_rad) / Math.PI)),
              is_it_in_the_circle() ? (joystick(coord.x, coord.y), (x_abs = coord.x), (y_abs = coord.y))
                  : joystick((x_abs = radius * Math.cos(angulo_rad) + x_orig), (y_abs = radius * Math.sin(angulo_rad) + y_orig)),
              getPosition(t);

              velocidade = Math.round((100 * Math.sqrt(Math.pow(x_abs - x_orig, 2) + Math.pow(y_abs - y_orig, 2))) / radius);

              if (ws.readyState === WebSocket.OPEN) {
                  ws.send(`${angulo_graus},${velocidade}`);
              } else {
                  console.error("WebSocket connection is not open.");
              }
          }
      }
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
        Serial.flush();
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
