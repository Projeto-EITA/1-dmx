/*
  EITA!
  Experimentos e Iterações com Tecnologia Aberta

  Arduino é MIDI!

  No exemplo foram usados os seguintes dispositivos:
  - Arduino Uno;

  - firmware mocoLUFA de morecat_lab, para transformar o Arduino em dispostivo MIDI nativo,
  com "dual boot".  GitHub do projeto: https://github.com/kuwatay/mocolufa.
  Foi usada a versão Pimp my mocoLUFA de Yaeltex / pablofullanado
  pois permite nomear o dispositivo MIDI: https://moco-lufa-web-client.herokuapp.com

  - Atmel Flip para fazer a atualização do firmware - Device Firmware Upgrade (DFU)
  https://www.microchip.com/en-us/development-tool/flip
  O Atmel Flip é para Windows, para outros sistemas operacionais veja o tópico
  "Download a DFU Programmer" aqui https://www.arduino.cc/en/Hacking/DFUProgramming8U2

  - biblioteca MIDI de Francois Best (https://github.com/FortySevenEffects/arduino_midi_library).

  Desenvolvido por Artur Cordeiro
  Cariri, CE, 06.01.2022.
*/

/*
  As notas MIDI são referenciadas por uma numeração padronizada. Veja mais aqui:
  https://www.inspiredacoustics.com/en/MIDI_note_numbers_and_center_frequencies.
  Neste sketch são usadas 3 notas:
  - Dó (C4), corresponde a nota MIDI número 60;
  - Mi (E4), corresponde a nota MIDI número 64;
  - Sol (G4), corresponde a nota MIDI número 67;
*/

// Notas musicais.
int notaDo = 60;
int notaMi = 64;
int notaSol = 67;

#include <MIDI.h> // Inclue a biblioteca MIDI.
MIDI_CREATE_DEFAULT_INSTANCE(); // Cria a instância MIDI da biblioteca.

void setup() {
  MIDI.begin(); // Inicia a biblioteca MIDI.
}

void loop() {
  // Envia nota Dó pressionada (NoteOn),
  // com velocidade 100, no canal 1.
  MIDI.sendNoteOn(notaDo, 100, 1);
  delay(250);

  // "Tira o dedo da tecla" da nota Dó, envia nota solta (NoteOff),
  // com velocidade 0, no canal 1.
  MIDI.sendNoteOff(notaDo, 0, 1);

  // Envia nota Mi pressionada (NoteOn),
  // com velocidade 100, no canal 1.
  MIDI.sendNoteOn(notaMi, 100, 1);
  delay(250);

  // "Tira o dedo da tecla" da nota Mi, envia nota solta (NoteOff),
  // com velocidade 0, no canal 1.
  MIDI.sendNoteOff(notaMi, 0, 1);

  // Envia nota Sol pressionada (NoteOn),
  // com velocidade 100, no canal 1.
  MIDI.sendNoteOn(notaSol, 100, 1);
  delay(250);

  // "Tira o dedo da tecla" da nota Sol, envia nota solta (NoteOff),
  // com velocidade 0, no canal 1.
  MIDI.sendNoteOff(notaSol, 0, 1);
}
