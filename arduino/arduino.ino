#include <SoftwareSerial.h>

const uint8_t in1 = 3;
const uint8_t in2 = 5;
const uint8_t in3 = 9;
const uint8_t in4 = 10;

int forwards  = 1;
int backwards = 0;
int stop = 2;

SoftwareSerial rxTx(0, 1); // RX, TX

void move(int desired_direction) {
  if (desired_direction == forwards) {
    digitalWrite(in1, HIGH);
    digitalWrite(in2, LOW);
    digitalWrite(in3, HIGH);
    digitalWrite(in4, LOW);

  } else if (desired_direction == backwards) {
    digitalWrite(in1, LOW);
    digitalWrite(in2, HIGH);
    digitalWrite(in3, LOW);
    digitalWrite(in4, HIGH);

  } else {
    digitalWrite(in1, LOW);
    digitalWrite(in2, LOW);
    digitalWrite(in3, LOW);
    digitalWrite(in4, LOW);
  }
}

void setup() {
  Serial.begin(115200);
  rxTx.begin(115200); // ESP8266 baud rate

  pinMode(in1, OUTPUT);
  pinMode(in2, OUTPUT);

  pinMode(in3, OUTPUT);
  pinMode(in4, OUTPUT);

  move(stop);
}

void loop() {
  while (rxTx.available() <= 0) {
    delay(500);
    Serial.print(".");
  }

  String received = rxTx.readString();
  if(received.toInt() == forwards) {
    move(forwards);
  } else if (received.toInt() == backwards) {
    move(backwards);
  } else {
    move(stop);
  }

  Serial.println("");
  Serial.println("Received: " + received);

  delay(2500);
  move(stop);
}
