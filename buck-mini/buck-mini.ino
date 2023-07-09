void setup() { Serial.begin(9600); }
void loop() {
  static uint8_t i; static const uint8_t tamFiltro = 255;
  static unsigned leituraFiltrada, medicoes[tamFiltro];
  static unsigned long tempo, soma;
  for(i = 0, soma = 0; i < tamFiltro - 1; medicoes[i] = medicoes[i + 1], i ++)
    soma += medicoes[i];
  leituraFiltrada = (soma + (medicoes[tamFiltro - 1] = analogRead(A0))) / tamFiltro;
  if(millis() - tempo > 100) {
    tempo = millis();
    Serial.print(leituraFiltrada * 5.0 / 1024); Serial.flush(); Serial.println(" V");
  }
}