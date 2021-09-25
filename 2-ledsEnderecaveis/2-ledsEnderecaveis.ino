/*
  EITA!
  Experimentos e Iterações com Tecnologia Aberta

  Leds endereçáveis com Arduino Uno

  No exemplo foram usados os seguintes dispositivos:
  - Arduino Uno;
  - leds endereçáveis com três linhas (GND, VCC, e dados) de conexão,
  com chipset WS2812B, que são semelhantes aos leds Neopixels da Adafruit
  (https://learn.adafruit.com/adafruit-neopixel-uberguide/the-magic-of-neopixels).
  - biblioteca FastLED (https://github.com/FastLED/FastLED).

  Desenvolvido por Artur Cordeiro
  Cariri, CE, 23.09.2021.
*/

#include <FastLED.h> // Inclui a biblioteca.

#define numLeds 44 // Especifica a quantidade de leds.
#define DATA_PIN 6 // Especifica o pino de conexão dos leds.

CRGB leds[numLeds]; // Configura o bloco de memória para armazenar e manipular os leds.

void setup() {
  FastLED.addLeds<NEOPIXEL, DATA_PIN>(leds, numLeds); // Especifica as configurações dos leds.
}

void loop() {

  // Existem algumas formas de especificar a cor dos leds com a biblioteca FastLED.
  // No exemplo abaixo é usado o modo setRGB:
  // leds[número do led].setRGB(cor vermelha, cor verde, cor azul).
  // Após especificar as cores é necessário atualizar o led com a função show().

  for (int i = 0; i < numLeds; i++) {
   leds[i].setRGB(255, 0, 255);
    FastLED.show();
    delay(200);

    leds[i].setRGB(0, 0, 0);
    FastLED.show();
    delay(10);
  }
}
