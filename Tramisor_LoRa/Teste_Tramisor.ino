#include <SoftwareSerial.h> // Utilizado para fazer a recomfiguração dos pinos Tx e Rx 
#include "EBYTE.h" // Biblioteca utilizada para fazer a comunicação dos radios lora 

// Definições dos pinos do modulo lora
#define PIN_RX 3
#define PIN_TX 4
#define PIN_M0 6
#define PIN_M1 5
#define PIN_AX 7
#define PIN_PULSE 2 // Define o pino do medidor de pulsos

struct HIDRO {     //Estrutura de dados
  int Id;
  volatile int Pulsos;
  float Litros;
};

HIDRO Myhidro;

// Relação da comunicação serial em um sistema embarcado
SoftwareSerial ESerial(PIN_RX, PIN_TX);
EBYTE Transceiver(&ESerial, PIN_M0, PIN_M1, PIN_AX);

// Contador de pulsos 
volatile int pulseCount = 0;  
float totalLiters = 0.0;  // Total acumulado de litros medidos

void pulseCounter() {
  pulseCount++;
}

void setup() {
  Serial.begin(9600);            // Inicia a comunicação serial 
  ESerial.begin(9600);           // Inicia a comunicação serial LoRa
  
  pinMode(PIN_PULSE, INPUT_PULLUP);     // Configura o pino do sensor como entrada com pull-up interno
  attachInterrupt(digitalPinToInterrupt(PIN_PULSE), pulseCounter, RISING);  // Configura a interrupção

  // Inicializa os valores da estrutura
  Myhidro.Id = 1;
  Myhidro.Pulsos = 0;
  Myhidro.Litros = 0.0;

  // Valida a trasmição via LoRa
  if (Transceiver.init()) {
    Serial.println("Trasmição iniciado com sucesso.");
  } else {
    Serial.println("Falha ao iniciar a trasmição.");
    while (true); // Para o código se não conseguir iniciar a trasmição
  }

  Serial.println("Setup completo. Monitorando o sensor de fluxo de água...");
}

void loop() {
  // Desabilita interrupções temporariamente para fazer uma leitura estável do contador de pulsos
  noInterrupts();
  int pulses = pulseCount;
  pulseCount = 0;  // Zera o contador de pulsos
  interrupts();    // Reabilita interrupções

  // Calcula o volume de água em litros
  float liters = pulses / 450.0;  // Valor de  1 litro que aquevale a 450 pulsos refertente ao modelo do hidrometro
  totalLiters += liters;  // Adiciona o volume atual ao total acumulado

  // Atualiza os valores obitidos na estrutura 
  Myhidro.Pulsos = pulses;
  Myhidro.Litros = totalLiters;

  // Envia a estrutura via LoRa
  if (Transceiver.SendStruct(&Myhidro, sizeof(Myhidro))) {
    Serial.println("Dados enviados com sucesso via LoRa.");
  } else {
    Serial.println("Falha ao enviar dados via LoRa.");
  }

  // Mostra os litros calculado, o id do hidrometro e a quantidade de pulsos no monitor serial
  Serial.print("Id Hidrometro: ");
  Serial.println(Myhidro.Id);
  Serial.print("Pulsos: ");
  Serial.println(Myhidro.Pulsos);
  Serial.print("Volume total de água: ");
  Serial.print(Myhidro.Litros);
  Serial.println(" litros");

  // Pausa por um segundo antes da próxima leitura
  delay(1000);
}
