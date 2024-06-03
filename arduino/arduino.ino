#include <SoftwareSerial.h>

#define DEBUG

const uint8_t in1 = 3;
const uint8_t in2 = 5;
const uint8_t in3 = 9;
const uint8_t in4 = 10;

SoftwareSerial rxTx(0, 1); // RX, TX

struct Direction {
  int quadrant;
  int speedEngineA;
  int speedEngineB;
};

void move(Direction* direction) {
  if(direction->quadrant == 0) {
    stopEngine();
  } else if(direction->quadrant < 3) {
    analogWrite(in2, direction->speedEngineA);
    analogWrite(in1, 0);
    analogWrite(in4, direction->speedEngineB);
    analogWrite(in3, 0);
  } else {
    analogWrite(in2, 0);
    analogWrite(in1, direction->speedEngineA);
    analogWrite(in4, 0);
    analogWrite(in3, direction->speedEngineB);
  }
}

void stopEngine() {
  digitalWrite(in1, LOW);
  digitalWrite(in2, LOW);
  digitalWrite(in3, LOW);
  digitalWrite(in4, LOW);
}

boolean isDigit(char c) {
  return (c >= '0' && c <= '9');
}

boolean isDelimiter(char c) {
  return (c == ',');
}

boolean isTokenValid(String token) {
  return token.length() >= 3 && token.length() <=7 && token.indexOf(',') > 0;
}

void parseEndToken(String &input, int &index) {
  if (index > input.length() - 1)
    return;

  int i = index;
  char c = input.charAt(index);
  while(isDigit(c) == false && index < input.length()) {
    index += 1;
    c = input.charAt(index);
  }

  if (i == index)
    index += 1;
}

void parseNumber(String &input, String &token, String &acc, int &index) {
  if (index > input.length() - 1)
    return;

  int i = index;
  char c = input.charAt(index);
  while(isDigit(c) && index < input.length()) {
    token += c;
    index += 1;
    c = input.charAt(index);
  }

  if (i == index)
    index += 1;
}

boolean parseDelemiter(String &input, String &token, String &acc, int &index) {
  if (index > input.length() - 1)
    return;

  char c = input.charAt(index);
  if( isDelimiter(c) )
    token += c;

  index += 1;
}

void parse(String &received, String &token, String &acc, int &index, int &countTokens) {
  if (received.length() <= 0)
    return;

  if (index > received.length() - 1)
    return;

  parseNumber(received, token, acc, index);
  parseDelemiter(received, token, acc, index);
  parseNumber(received, token, acc, index);
  parseEndToken(received, index);

  if( isTokenValid(token) ) {
    countTokens += 1;
    acc += token;
  }

  if (index < received.length() - 1)
    acc += '|';

  token = "";
  parse(received, token, acc, index, countTokens);
}

String* parseDirections(String received, int &countDirectionsReceived) {
  String token = "";
  String acc = "";
  int index = 0;

  parse(received, token, acc, index, countDirectionsReceived);
  String* directions = new String[countDirectionsReceived];

  #ifdef DEBUG
  Serial.print("acc=(");
  Serial.print(acc);
  Serial.println(")");
  #endif

  int beginIndex = 0;
  for(int i = 0; i < countDirectionsReceived; i++){
    int delimiterIndex = acc.indexOf('|', beginIndex);
    directions[i] = acc.substring(beginIndex, delimiterIndex);
    beginIndex = delimiterIndex + 1;
  }

  return directions;
}

void parseDirection(String received, Direction* &direction) {
  int endIndex = received.indexOf(',');
  String first = received.substring(0, endIndex);
  String second = received.substring(endIndex+1, received.length());

  #ifdef DEBUG
  Serial.print("first[");
  Serial.print(first);
  Serial.print("],second[");
  Serial.print(second);
  Serial.print("] ");
  #endif

  int speed = first.toInt();
  int angle = second.toInt();

  #ifdef DEBUG
  Serial.print("speed[");
  Serial.print(speed);
  Serial.print("],angle[");
  Serial.print(angle);
  Serial.print("] ");
  #endif

  speed = map(speed, 0, 100, 0, 255); // Map the value to the range 0-255

  if(angle < 90) {
    direction->quadrant = 1;
  } else if(angle < 180) {
    direction->quadrant = 2;
  } else if(angle < 270) {
    direction->quadrant = 3;
  } else if(angle < 360) {
    direction->quadrant = 4;
  }

  if(direction->quadrant == 1 || direction->quadrant == 4) {
    direction->speedEngineB = speed;
    direction->speedEngineA = (speed * (1.0 - cos(angle * PI / 180.0) / 2.0));
  } else {
    direction->speedEngineB = (speed * (1.0 - cos(angle * PI / 180.0) / 2.0));
    direction->speedEngineA = speed;
  }

  #ifdef DEBUG
  Serial.print("directions[");
  Serial.print(direction->quadrant);
  Serial.print(",");
  Serial.print(direction->speedEngineA);
  Serial.print(",");
  Serial.print(direction->speedEngineB);
  Serial.println("]");
  #endif
}

void setup() {
  Serial.begin(115200);
  rxTx.begin(115200); // ESP8266 baud rate

  while (!Serial) {};

  pinMode(in1, OUTPUT);
  pinMode(in2, OUTPUT);

  pinMode(in3, OUTPUT);
  pinMode(in4, OUTPUT);

  stopEngine();
}

void loop() {
  if (rxTx.available() <= 0) {
    return;
  }

  String received = "";
  while (rxTx.available()) {
    received += rxTx.readStringUntil('\n');
  }
  rxTx.flush();

  //delay(1500);
  //String received;
  //received = "";
  //received = "100";
  //received = "1,1";
  //received = "10,10";
  //received = "10,011\t\t\n1,2 0,2&900,1 2";

  #ifdef DEBUG
  Serial.print("r=(");
  Serial.print(received);
  Serial.print(")");
  Serial.print(" ");
  #endif

  int numOfDirections = 0;
  String* directions = parseDirections(received, numOfDirections);

  for(int i = 0; i < numOfDirections; i++) {
    Direction* direction = (Direction*) malloc(sizeof(Direction));
    parseDirection(directions[i], direction);
    move(direction);
    free(direction);
  }
}
