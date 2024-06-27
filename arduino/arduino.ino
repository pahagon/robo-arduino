#include <SoftwareSerial.h>

#define DEBUG

const uint8_t in1 = 3;
const uint8_t in2 = 5;
const uint8_t in3 = 9;
const uint8_t in4 = 10;

SoftwareSerial rxTx(0, 1); // RX, TX

void setup() {
  Serial.begin(115200);
  rxTx.begin(115200); // ESP8266 baud rate

  while (!Serial) {};

  pinMode(in1, OUTPUT);
  pinMode(in2, OUTPUT);

  pinMode(in3, OUTPUT);
  pinMode(in4, OUTPUT);

  digitalWrite(in1, LOW);
  digitalWrite(in2, LOW);
  digitalWrite(in3, LOW);
  digitalWrite(in4, LOW);
}

void loop() {
  if (rxTx.available() <= 0) {
    return;
  }

  byte enginesSpeed[8];
  rxTx.readBytes(enginesSpeed, 8);  // Lê os 4 bytes da serial
  rxTx.flush();

  uint8_t leftEngineSpeedIn1 = enginesSpeed[0];
  uint8_t leftEngineSpeedIn2 = enginesSpeed[1];
  uint8_t rightEngineSpeedIn1 = enginesSpeed[2];
  uint8_t rightEngineSpeedIn2 = enginesSpeed[3];

  #ifdef DEBUG
  Serial.print(F("leftEngineSpeedIn1("));
  Serial.print(leftEngineSpeedIn1);
  Serial.print(F("),"));

  Serial.print(F("leftEngineSpeedIn2("));
  Serial.print(leftEngineSpeedIn2);
  Serial.print(F("),"));

  Serial.print(F("rightEngineSpeedIn1("));
  Serial.print(rightEngineSpeedIn1);
  Serial.print(F("),"));

  Serial.print(F("rightEngineSpeedIn2("));
  Serial.print(rightEngineSpeedIn2);
  Serial.print(F(")"));

  Serial.println(F(""));
  #endif

  analogWrite(in1, leftEngineSpeedIn1);
  analogWrite(in2, leftEngineSpeedIn2);

  analogWrite(in3, rightEngineSpeedIn1);
  analogWrite(in4, rightEngineSpeedIn2);
}
