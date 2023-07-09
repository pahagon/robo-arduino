/* @autor: Eletrogate
   @licença: GNU GENERAL PUBLIC LICENSE Version 3 */

#define DEBUG

const uint8_t in1 = 3;
const uint8_t in2 = 5;
const uint8_t in3 = 9;
const uint8_t in4 = 10;
const uint8_t pinIn = 4;
const uint8_t pinOut = 2;
const char caractereInicio = ':';
const char caractereSepara = ' ';
const char caractereFinal = '\n';

bool dado_novo;                                       // declara as
uint8_t v, a, r, vel_int, val_mA, val_mB, quadrante;  // variaveis que serao
char vel[4], angulo[4], recebido[16], c;              // utilizada ao longo
unsigned  ang_int;                                    // do programa

bool caractereValido(char c)  {
  return ((c >= '0' && c <= '9') || c == ' ');  // verifica se o caractere recebido do ESP é um número ou um espaço
}

#ifdef DEBUG
void builtInLedBlink() {
  digitalWrite(LED_BUILTIN, HIGH);
  delay(500);
  digitalWrite(LED_BUILTIN, LOW);
  delay(500);
}
#endif

void setup() {
  delay(500);                 // aguarda o sistema estabilizar
  Serial.begin(9600);         // inicia a serial
  pinMode(pinIn, INPUT);      // configura e inicia
  pinMode(pinOut, OUTPUT);    // as entradas
  digitalWrite(pinOut, LOW);  // e saídas
  #ifdef DEBUG
  pinMode(LED_BUILTIN, OUTPUT);
  #endif
}

void loop() {

  if(!digitalRead(pinIn)) {           // se detectar nova conexão à página
    digitalWrite(pinOut, LOW);        // avisa que detectou
    delay(100);                       // aguarda 100 milisegundos
    while(Serial.available())
      Serial.read();                  // esvazia o buffer
    digitalWrite(pinOut, HIGH);       // prepara para a proxima conexão
  }

  if(Serial.available() && Serial.read() == caractereInicio) {  // se há dados e o primeiro é caractereInicio
    r = -1;                                           // prepara o indice de recebido
    while((c = Serial.read()) != caractereFinal)      // enquanto nao for o caractere finalizador
      if(caractereValido(c))                          // se for um caractere valido
        recebido[++ r] = c;                           // o armazena a incrementa o indice

    for(v = 0; recebido[v] != caractereSepara; v ++)  // do primeiro caractere até caractereSepara
      vel[v] = recebido[v];                           // copia-o para vel
    vel[v] = '\0';                                    // insere o caractere nulo na posição posterior à do ultimo copiado

    for(a = v + 1; a <= r; a ++)                      // do primeiro após caractereSepara até o ultimo caractere valido de recebido
      angulo[a - v - 1] = recebido[a];                // copia-o para angulo - v - 1 é a diferença entre a posição de recebido e a de angulo

    angulo[r - v - 1 < 3 ?      // angulo recebeu até 3 caracteres?
            r - v : 3] = '\0';  // se sim, insere o caractere nulo na posição posterior à do ultimo copiado. se não, o insere na ultima posição de angulo

    vel_int = atoi(vel); vel_int = map(vel_int, 0, 100, 0, 255); // transforma a velocidade em um inteiro e, então, o mapeia entre 0 e 255
    ang_int = atoi(angulo);                                      // transforma o angulo em um inteiro

    if(ang_int < 90) quadrante = 1; else if(ang_int < 180) quadrante = 2; else if(ang_int < 270) quadrante = 3; else if(ang_int < 360) quadrante = 4; // determina em qual quadrante o joystick está
    
    if(quadrante == 1 || quadrante == 4) {                                      // se estiver na direita
      val_mB = (uint8_t) vel_int;                                               // o motor B recebe a velocidade indicada no joystick
      val_mA = (uint8_t) (vel_int * (1.0 - cos(ang_int * PI / 180.0) / 2.0)); } // o motor A recebe esta velocidade reduzida proporcionalmente ao cosseno do ângulo
    else {                                                                      // caso não
      val_mB = (uint8_t) (vel_int * (1.0 + cos(ang_int * PI / 180.0) / 2.0));   // o motor B recebe a velocidade acrescida proporcionalmente ao cosseno (que será negativo) do ângulo
      val_mA = (uint8_t) vel_int; }                                             // o motor A recebe a velocidade indicada no joystick

    if(quadrante < 3) {                                 // se estiver em cima/na frente
      analogWrite(in2, val_mA); analogWrite(in1, 0);    // aciona os motores
      analogWrite(in4, val_mB); analogWrite(in3, 0);  } // para frente
    else {                                              // senão
      analogWrite(in2, 0); analogWrite(in1, val_mA);    // aciona os motores
      analogWrite(in4, 0); analogWrite(in3, val_mB);  } // para trás
  }
}
