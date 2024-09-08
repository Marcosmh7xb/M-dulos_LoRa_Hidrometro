#include <SoftwareSerial.h> // Utilizado para fazer a recomfiguração dos pinos Tx e Rx 
#include "EBYTE.h" // Biblioteca utilizada para fazer a comunicação dos radios lora 

// Definições dos pinos do modulo lora
#define PIN_RX 3
#define PIN_TX 4
#define PIN_M0 6
#define PIN_M1 5
#define PIN_AX 2

struct HIDRO {  //Estrutura de dados
  int Id;
  int Pulsos;
  float Litros;
};

int Chan;
HIDRO Myhidro;
unsigned long Last;

// Relação da comunicação serial em um sistema embarcado
SoftwareSerial ESerial(PIN_RX, PIN_TX);
EBYTE Transceiver(&ESerial, PIN_M0, PIN_M1, PIN_AX);

void setup() {
  Serial.begin(9600);            // Inicia a comunicação serial para monitoramento dos dados 
  ESerial.begin(9600);           // Inicia a comunicação serial LoRa


  // Configura o recepitor de dados Lora 
  if (Transceiver.init()) {
    Serial.println("Trasmissão iniciado com sucesso.");
  } else {
    Serial.println("Falha ao iniciar o trasmissão.");
    while (true); // Para o código se não conseguir iniciar o trasmissão
  }

  // Indicar os parametros de fucionamneto da radios lora 
  Transceiver.init();
  Transceiver.PrintParameters();

  Last = millis(); // Inicializa o último tempo de verificação
  Serial.println("Tramissão completa. Aguardando dados...");
}

void loop() {
  // Verifica se há dados disponíveis para leitura 
  if (Transceiver.available()) {
    HIDRO receivedData;

    // Recebe a estrutura de dados
    if (Transceiver.GetStruct(&Myhidro, sizeof(Myhidro))) {
      
      // Exibe os dados recebidos no monitor serial
      Serial.print("Id Hidrometro: ");
      Serial.println(Myhidro.Id);
      Serial.print("Pulsos: ");
      Serial.println(Myhidro.Pulsos);
      Serial.print("Volume total de água: ");
      Serial.print(Myhidro.Litros);
      Serial.println(" litros");

      // Atualiza o último tempo de verificação
      Last = millis();
    } else {
      Serial.println("Falha ao receber dados.");
    }
  } else {
    // Verifica se passou mais de 1 segundo sem receber dados
    if ((millis() - Last) > 1000) {
      Serial.println("Searching: ");
      Last = millis();
    }
  }

  // Verificação adicional de dados recebidos através de ESerial
  if (ESerial.available()) {
    // Sugere-se enviar dados usando estruturas e não métodos de análise de dados
    Serial.println("Dados recebidos pela ESerial, aguardando processamento...");
  }

  // Pausa por um curto período antes da próxima verificação
  delay(1000);
}
