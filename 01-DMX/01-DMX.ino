/*
  EITA!
  Experimentos e iterações com tecnologia aberta

  Conectando um refletor led RGB DMX com Arduino Uno

  No exemplo foram usados os seguintes dispositivos:
  - Arduino Uno;
  - módulo DMX VMA432 da Velleman (https://www.velleman.eu/products/view/?id=439222);
  - refletor Ibiza Light PAR-MINI-RGB3;
  - biblioteca DmxSimple (https://code.google.com/archive/p/tinkerit/wikis/DmxSimple.wiki).

  O refletor usado no sketch suporta 7 canais DMX,
  no caso espécífico do modelo usado, os canais são:
  - canal 1 seleciona funções automáticas (fade, strobe som);
  - canal 2 seleciona cores predefinidas;
  - canal 3 ajusta a velocidade (para as funçoes fade e strobe);
  - canal 4 ajusta o brilho (brightness) geral das cores RGB;
  - canal 5 ajusta a intensidade da cor vemelha (red);
  - canal 6 ajusta a intensidade da cor verde (green);
  - canal 7 ajusta a intensidade da cor azul (blue).

  Desenvolvido por Artur Cordeiro
  Cariri, CE, 2021.
*/

#include <DmxSimple.h> // Inclui a biblioteca

void setup() {
  DmxSimple.usePin(3); // Define o pino de saída do módulo DMX.
  DmxSimple.maxChannel(7); // Define o número máximo de canais que serão enviados.
  // O protocolo DMX suporta 512 canais, mas por limitação de memória da placa,
  // a biblioteca SimpleDmx suporta apenas 128 canais.
}

void loop() {
  // DmxSimple.write(canal, valor)
  // O canal é de acordo com o equipamento usado,
  // e o valor varia de 0 a 255.

  DmxSimple.write(4, 255);      // Define o brilho geral das cores RGB.
  DmxSimple.write(5, 255);      // Define a intensidade de vermelho (red).
  DmxSimple.write(6, 0);        // Define a intensidade de verde (green).
  DmxSimple.write(7, 0);        // Define a intensidade de azul (azul).
  delay(1000);

  DmxSimple.write(4, 0);        // Define o brilho geral das cores RGB.
  delay(1000);
}
