const uint8_t in1 = 3;
const uint8_t in2 = 5;
const uint8_t in3 = 9;
const uint8_t in4 = 10;

int forwards  = 1;
int backwards = 0;
int stop = 2;

void builtInLedBlink() {
  digitalWrite(LED_BUILTIN, HIGH);
  delay(500);
  digitalWrite(LED_BUILTIN, LOW);
  delay(500);
}

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
  pinMode(in1, OUTPUT);
  pinMode(in2, OUTPUT);

  pinMode(in3, OUTPUT);
  pinMode(in4, OUTPUT);
}

void loop() {
  move(forwards);
  delay(2500);

  move(backwards);
  delay(2500);
}
