/*
  EITA!
  Experimentos e Iterações com Tecnologia Aberta

  Sensoriando o solo: monitoramento da umidade do solo.

  No exemplo foram usados os seguintes dispositivos:
  - Arduino Uno;
  - Sensor capacitivo de umidade do solo (Capacitive soil moisture sensor v1.2);
  - Módulo TM1637 de display de sete segmentos com 4 dígitos;
  - Anel de 16 leds endereçáveis com três linhas (GND, VCC, e dados),
  semelhantes aos leds Neopixels da Adafruit
  (https://learn.adafruit.com/adafruit-neopixel-uberguide/the-magic-of-neopixels);
  - biblioteca FastLED (https://github.com/FastLED/FastLED);
  - biblioteca EWMA (Exponentially Weighted Moving Average)de jonnieZG,
  https://github.com/jonnieZG/EWMA;
  - biblioteca TM1637 do Seed Studio (https://github.com/Seeed-Studio/Grove_4Digital_Display).

  Desenvolvido por Artur Cordeiro
  Cariri, CE, 03.10.2021.
*/

#include "TM1637.h" // Inclui a biblioteca do display de sete segmentos.
const int CLK = 6; // Define o pino de conexão CLK do módulo.
const int DIO = 7; // Define o pino de conexão DIO do módulo.
TM1637 tm1637(CLK, DIO); // Cria uma instância do objeto com as definições de CLK e DIO.


#include <FastLED.h> // Inclui a biblioteca FastLED.
#define numLeds 16 // Especifica a quantidade de leds.
#define DATA_PIN 5 // Especifica o pino de conexão dos leds.
CRGB leds[numLeds]; // Configura o bloco de memória para armazenar e manipular os leds.


#include <Ewma.h>  // Inclui a biblioteca Ewma.
Ewma fator(0.06);  // Cria o filtro e estabelece o fator de suavização (varia de 0 a 1).
// A biblioteca Ewma diminui o ruído na leitura do sensor,
// suavizando as variações de leitura de acordo com o fator especificado.
// Quanto menor o fator, maior a suavização.


int sensor = A0; // Variável que define o pino de conexão do sensor de umidade.
int umidade; // Variável para receber os valores do sensor.
int umidadeFiltro; // Variável para calcular a umidade com o filtro Ewma.
int percentual; // Variável para transformar a umidade em uma escala percentual.
int cor; // Variável para a cor dos leds.


/*
  A tensão de operação do sensor capacitivo de umidade do solo é de 3.3 a 5.0V,
  porém, a saída de tensão do pino analógico do sensor é de 0 a 3.0V.
  As entradas analógicas do Arduino operam na faixa de tensão de 0 a 5.0V,
  resultando na respectiva faixa de valores de 0 a 1023.
  Portanto, o sensor capacitivo de umidade do solo no Arduino Uno
  resulta na faixa de valores de 0 a 613.

  Para correta visualização dos dados, o sensor deve ser calibrado
  usando como referência os valores de umidade mínimo e máximo obtidos
  do solo que será monitorado.

  Obs.: o valor do sensor capacitivo é maior quando a umidade diminui.
*/

void setup() {
  Serial.begin(9600); // Inicia a comunicação seria.

  tm1637.init(); // Inicia o módulo display.
  tm1637.set(7); // Configura o brilho do display (varia de 0 a 7).

  FastLED.addLeds<NEOPIXEL, DATA_PIN>(leds, numLeds); // Especifica as configurações dos leds.
  FastLED.setBrightness(20); // Define o brilho máximo geral dos leds (varia de 0 a 255).
}

void loop() {
  umidade = analogRead(sensor); // Atualiza a variável com a leitura do sensor de umidade.
  umidadeFiltro = fator.filter(umidade); // Aplica o filtro Ewma e atualiza umidadeFiltro.

  // Calcula o valor percentual baseado nos valores de umidade mínimo e máximo.
  percentual = int(map(umidadeFiltro, 620, 300, 0, 100));
  percentual = constrain(percentual, 0, 100); // Restringe o percentual na faixa de 0 a 100.

  cor = int(map(percentual, 0, 100, 0, 160)); // Calcula o valor correpondente da faixa de cor.

  // Exibe os valores das varíaveis no Monitor Serial
  Serial.print("Umidade: ");
  Serial.print(umidade);
  Serial.print(" | Umidade filtrada: ");
  Serial.print(umidadeFiltro);
  Serial.print(" | Percentual: ");
  Serial.print(percentual);
  Serial.print(" | Cor: ");
  Serial.println(cor);

  // Atualiza os leds com a cor correspondente ao valor percentual.
  for (int i = 0; i < numLeds; i++) {
    leds[i].setHSV(cor, 255, 255); // A função setHSV define a tonalidade, saturação e brilho.
    FastLED.show();
  }

// O teste condicional abaixo é um improviso para garantir que o display
// exiba o valor 0 quando o valor do percentual também for 0.
// Pois a função displayNum não exibe o 0, e o display fica "em branco", apagado.
  if (percentual == 0) {
    tm1637.displayStr("   0"); // Exibe a string de texto "   0".
  }
  else {
    tm1637.displayNum(percentual); // Exibe o número do percentual.
  }
  
  delay(50);
}
