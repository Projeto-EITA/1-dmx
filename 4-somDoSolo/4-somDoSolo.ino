/*
  EITA!
  Experimentos e Iterações com Tecnologia Aberta

  O som do solo: representação sonora dos dados com MIDI.

  No exemplo foram usados os seguintes dispositivos:
  - Arduino Uno;

  - firmware MocoLufa de morecat_lab, para transformar o Arduino em dispostivo MIDI nativo,
  com "dual boot".  GitHub do projeto: https://github.com/kuwatay/mocolufa.
  Foi usada a versão Pimp my mocoLUFA de Yaeltex / pablofullanado
  pois permite nomear o dispositivo MIDI: https://moco-lufa-web-client.herokuapp.com

  - Atmel Flip para fazer a atualização do firmware - Device Firmware Upgrade (DFU)
  https://www.microchip.com/en-us/development-tool/flip
  O Atmel Flip é para Windows, para outros sistemas operacionais veja o tópico
  "Download a DFU Programmer" aqui https://www.arduino.cc/en/Hacking/DFUProgramming8U2

  - Sensor capacitivo de umidade do solo (Capacitive soil moisture sensor v1.2);

  - Módulo TM1637 de display de sete segmentos com 4 dígitos;

  - Anel de 16 leds endereçáveis com três linhas (GND, VCC, e dados),
  semelhantes aos leds Neopixels da Adafruit
  (https://learn.adafruit.com/adafruit-neopixel-uberguide/the-magic-of-neopixels);

  - biblioteca FastLED (https://github.com/FastLED/FastLED);

  - biblioteca EWMA (Exponentially Weighted Moving Average)de jonnieZG,
  https://github.com/jonnieZG/EWMA;

  - biblioteca TM1637 do Seed Studio (https://github.com/Seeed-Studio/Grove_4Digital_Display).

  - biblioteca MIDI de Francois Best (https://github.com/FortySevenEffects/arduino_midi_library).

  Desenvolvido por Artur Cordeiro
  Cariri, CE, 08.10.2021.
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
int percentualAntigo; // Variável para detectar mudança na leitura.
int cor; // Variável para a cor dos leds.
int oitavaMin, oitavaMax; // Variáveis para definir as notas mínima e máxima da oitava (musical).

/*
  Neste sketch é usada a escala das notas pretas do piano para a composição musical,
  ou mais especificamente, a escala pentatônica maior em Fá sustenido (F#, G#, A#, C# e D#),
  que também poderia ser a escala pentatônica menor em Ré sustenido (D#, F#, G#, A#, C#),
  em ambos os casos, são todas as notas pretas do piano.
  Veja mais informações aqui: https://pt.wikipedia.org/wiki/Escala_pentat%C3%B4nica.

  As notas MIDI são referenciadas por uma numeração padronizada. Veja mais aqui:
  https://www.inspiredacoustics.com/en/MIDI_note_numbers_and_center_frequencies.
  Neste sketch são usadas 4 oitavas, a primeira nota é o F#2, que corresponde a nota MIDI 42,
  e a última é o F#6, nota MIDI 90.
  A numeração de uma oitava na escala pentatônica maior segue fórmula:
  nota raiz, + 2, + 2, + 3, + 2, + 3.
  Portanto, a primeira oitava é formada pelas notas
  42 (F#), 44 (G#), 46 (A#), 49 (C#), 51 (D#) e 54 (F#).

*/

// Notas pretas do piano ou escala petatônica em F# maior (4 oitavas, 21 notas).
int notas[] = {42, 44, 46, 49, 51, 54, 56, 58, 61, 63, 66, 68, 70, 73, 75, 78, 80, 82, 85, 87, 90};
int tempos[] = {250, 500, 750, 1000}; // Divisão de tempo para pausa entre notas.

int indiceNotas, indiceTempo; // Variável dos indices das notas e dos tempos.

unsigned long relogio = millis(); // Variável para fazer a contagem do tempo.
boolean tocou = false; // Variável para saber quando a nota foi pressionada (note on).

#include <MIDI.h> // Inclue a biblioteca MIDI.
MIDI_CREATE_DEFAULT_INSTANCE(); // Cria a instância MIDI da biblioteca.

int efeitoMidi; // Variável para enviar controle MIDI número 1 (Modulation).
// Veja aqui a numeração dos controles MIDI:
// http://midi.teragonaudio.com/tech/midispec/ctllist.htm

void setup() {
  MIDI.begin(); // Inicia a biblioteca MIDI.

  tm1637.init(); // Inicia o módulo display.
  tm1637.set(7); // Configura o brilho do display (varia de 0 a 7).

  FastLED.addLeds<NEOPIXEL, DATA_PIN>(leds, numLeds); // Especifica as configurações dos leds.
  FastLED.setBrightness(20); // Define o brilho máximo geral dos leds (varia de 0 a 255).

  indiceTempo = int(random(4)); // Seleciona um número de 0 a 3.
}


void loop() {
  umidade = analogRead(sensor); // Atualiza a variável com a leitura do sensor de umidade.
  umidadeFiltro = fator.filter(umidade); // Aplica o filtro Ewma e atualiza umidadeFiltro.

  // Calcula o valor percentual baseado nos valores de umidade mínimo e máximo.
  percentual = int(map(umidadeFiltro, 620, 300, 0, 100));
  percentual = constrain(percentual, 0, 100); // Restringe o percentual na faixa de 0 a 100.

  /*
    Para representar os valores do sensor de umidade do solo,
    foi estabelecida uma relação de terra seca, notas agudas,
    então, quanto mais úmida a terra, mais grave são os tons.

    Os valores de oitavaMin e oitavaMax são índices do array notas[],
    o índice 15 (notas[15]) corresponde a nota MIDI 78 (F#5)
    o índice 0 (notas[0]) corresponde a nota MIDI 42 (F#2).
  */
  oitavaMin = int(map(percentual, 0, 100, 15, 0));
  oitavaMax = oitavaMin + 5;

  // Teste condicional para identificar se houve mudança na leitura,
  // caso positivo é enviado o Controle MIDI 1 Modulation (geralmente é um vibrato).
  // Observação: no protocolo MIDI a faixa de valores é de 0 a 127.
  if (percentual < percentualAntigo || percentual > percentualAntigo) {
    efeitoMidi = int(random(60, 127)); // Sorteia um número entre 60 e 127.

    // Envia o Controle MIDI número 1, com o valor efeitoMidi, no canal 1.
    MIDI.sendControlChange(1,  efeitoMidi, 1);
  }
  else {
    // Envia o Controle MIDI número 1, com o valor 0 (desligado), no canal 1.
    MIDI.sendControlChange(1, 0, 1);
  }

  cor = int(map(percentual, 0, 100, 0, 160)); // Calcula o valor correpondente da faixa de cor.

  // Teste condicional do tempo millis() para marcar a execução e pausa das notas,
  // evitando usar delay(), pois trava o Arduino, impedindo respostas em tempo real.
  if (millis() - relogio > tempos[indiceTempo] && tocou == false) {
    relogio = millis(); // Resetar o tempo do relógio.
    indiceNotas = int(random(oitavaMin, oitavaMax)); //  Seleciona uma nota entre as oitavas.

    // Envia o Controle MIDI Sustain número 64, com valor 0 (desligado), no canal 1.
    MIDI.sendControlChange(64, 0, 1);

    // Envia o Controle MIDI Sustain número 64, com valor 127 (ligado), no canal 1.
    MIDI.sendControlChange(64, 127, 1);

    // Envia nota pressionada (NoteOn), a nota especificada pelo índice,
    // com velocidade que foi pressionada no valor entre 80 e 127, no canal 1.
    MIDI.sendNoteOn(notas[indiceNotas], int(random(80, 127)), 1);

    tocou = true; // Atualiza a variável, sinalizando que a nota foi pressioanda.
  }
  // Teste condicional para enviar NoteOff, para "tirar o dedo da tecla".
  else if (millis() - relogio > ( tempos[indiceTempo] - 30) && tocou)   {
    // Envia nota solta (NoteOff), a nota especificada pelo índice,
    // com velocidade 0, no canal 1.
    MIDI.sendNoteOff(notas[indiceNotas], 0, 1);
    tocou = false; // Atualiza a variável, sinalizando que a tecla foi liberada.
    indiceTempo = int(random(4)); // Seleciona um índice do tempo[].
  }

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
  percentualAntigo = percentual; // Atualiza o percentaul para detectar mudança no sensor.
}
