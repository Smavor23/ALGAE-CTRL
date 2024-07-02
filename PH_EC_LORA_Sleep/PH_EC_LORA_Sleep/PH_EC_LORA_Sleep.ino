#include <Rak3172_Canopus.h>  // Include the Rak3172_Canopus library header file.
#include "LoRa_SL.h"
#include <string>
#define Serial_Canopus Serial1
#define REDE_RS485 -1
#define  RS485_Tranmit digitalWrite(REDE_RS485, HIGH) 
#define  RS485_Receive digitalWrite(REDE_RS485, LOW)


//SoftwareSerial SerialCanopus(RX_PIN, TX_PIN);
byte Temp[] = {0x01, 0x03, 0x00, 0x00, 0x00, 0x01, 0x84, 0x0a};
byte EC[] = {0x01, 0x03, 0x00, 0x02, 0x00, 0x01, 0x25, 0xca};
byte Salinity[] = {0x01, 0x03, 0x00, 0x03, 0x00, 0x01, 0x74, 0x0a};
byte TDS[] = {0x01, 0x03, 0x00, 0x04, 0x00, 0x01, 0xc5, 0xcb};
byte PH[] = {0x02, 0x04, 0x00, 0x01, 0x00, 0x01, 0x60, 0x39};
byte Chlorophyll[] = {0x10, 0x03, 0x00, 0x00, 0x00, 0x02, 0xc7, 0x4a};

byte responseBuffer[50]; // Taille du tampon de réponse
float Value, Val1;
uint8_t frame[50]; // Taille suffisamment grande pour contenir la chaîne convertie
int Previous_time = 0;
long Reboot_time = 86400000; // 1 jour  

void setup() {
  Previous_time = millis();
  Serial.begin(115200);
  Serial_Canopus.begin(9600, SERIAL_8N1);
  pinMode(PA1, OUTPUT);
  digitalWrite(PA1, PWR_ON);  //On power Vrs485 in V2
  init_lora(868000000);
  Serial.println("**************************************************************************************");
  Serial.println("******************************* ------------------ ***********************************");
  Serial.println("*******************************| SMART ALGAE CTRL |***********************************");
  Serial.println("******************************* ------------------ ***********************************");
  Serial.println("*********************************************************************************SL***");
}


int temp(){
  RS485_Tranmit;
  Serial_Canopus.write(Temp, sizeof(Temp));// Envoyer la requête au capteur

  // Lire la réponse
  RS485_Receive;
  delay(200);
  int responseLength = Serial_Canopus.available();
  if (responseLength > 0) {
    for (int i = 0; i < responseLength; i++) {
      responseBuffer[i] = Serial_Canopus.read();
    }
      Value = (responseBuffer[3] << 8) | responseBuffer[4];
      Serial.print("temperature : ");
      Serial.print(Value/100);
      Serial.println(" °C");
  } else {
    Serial.println("Aucune réponse reçue.");
    Value = 0;
  }

  return Value;
}

int conductivity(){
   
  RS485_Tranmit;
  Serial_Canopus.write(EC, sizeof(EC));// Envoyer la requête au capteur

  // Lire la réponse
  RS485_Receive;
  delay(200);
  int responseLength = Serial_Canopus.available();
  if (responseLength > 0) {
    for (int i = 0; i < responseLength; i++) {
      responseBuffer[i] = Serial_Canopus.read();
    }
      Value = (responseBuffer[3] << 8) | responseBuffer[4];
      Serial.print("Conductivity : ");
      Serial.print(Value);
      Serial.println(" us/cm");
  } else {
    Serial.println("Aucune réponse reçue.");
    Value = 0;
  }

  return Value;
}

int salinity(){
   
  RS485_Tranmit;
  Serial_Canopus.write(Salinity, sizeof(Salinity));// Envoyer la requête au capteur

  // Lire la réponse
  RS485_Receive;
  delay(200);
  int responseLength = Serial_Canopus.available();
  if (responseLength > 0) {
    for (int i = 0; i < responseLength; i++) {
      responseBuffer[i] = Serial_Canopus.read();
    }
      Value = (responseBuffer[3] << 8) | responseBuffer[4];
      Serial.print("Salinity : ");
      Serial.print(Value);
      Serial.println(" mg/L");
  } else {
    Serial.println("Aucune réponse reçue.");
    Value = 0;
  }

  return Value;
}

int tds(){
  RS485_Tranmit;
  Serial_Canopus.write(TDS, sizeof(TDS));// Envoyer la requête au capteur
  // Lire la réponse
  RS485_Receive;
  delay(200);
  int responseLength = Serial_Canopus.available();
  if (responseLength > 0) {
    for (int i = 0; i < responseLength; i++) {
      responseBuffer[i] = Serial_Canopus.read();
    }
      Value = (responseBuffer[3] << 8) | responseBuffer[4];
      Serial.print("TDS : ");
      Serial.print(Value);
      Serial.println(" mg/L");
  } else {
    Serial.println("Aucune réponse reçue.");
    Value = 0;
  }
  return Value;
}


int ph(){
  RS485_Tranmit;
  Serial_Canopus.write(PH, sizeof(PH));// Envoyer la requête au capteur
  // Lire la réponse
  RS485_Receive;
  delay(200);
  int responseLength = Serial_Canopus.available();
  if (responseLength > 0) {
    for (int i = 0; i < responseLength; i++) {
      responseBuffer[i] = Serial_Canopus.read();
    }
      Value = (responseBuffer[3] << 8) | responseBuffer[4];
      Serial.print("PH : ");
      Serial.print(Value/100);
      Serial.println(" ph");
  } else {
    Serial.println("Aucune réponse reçue.");
    Value = 0;
  }
  return Value;
}


// Fonction pour convertir une valeur 32 bits IEEE 754 en flottant
float convertIEEE754(uint32_t value) {
  union {
    uint32_t i;
    float f;
  } u;
  u.i = value;
  return u.f;
}

float chlorophyll() {
  // Envoyer la requête au capteur
  RS485_Tranmit;
  Serial_Canopus.write(Chlorophyll, sizeof(Chlorophyll));

  // Lire la réponse
  RS485_Receive;
  delay(2000);
  int responseLength = Serial_Canopus.available();
  if (responseLength > 0) {
    for (int i = 0; i < responseLength; i++) {
      responseBuffer[i] = Serial_Canopus.read();
    }
    
    // Vérifier la longueur de la réponse et extraire les données
    if (responseLength >= 9) {
      uint32_t combinedData = (responseBuffer[3] << 24) | (responseBuffer[4] << 16) | (responseBuffer[5] << 8) | responseBuffer[6];
      Value = convertIEEE754(combinedData);
      Serial.print("Chlorophyll : ");
      Serial.print(Value);
      Serial.println(" ug/L");
      
    } else {
      Serial.println("Réponse invalide.");
      Value = 0;
    }
  } else {
    Serial.println("Aucune réponse reçue.");
    Value = 0;
  }

  return Value;
}

void loop() {
  int EndFrame = 101010; // c'est pour éviter le bruit dans la fin de la trame
  //for (int repoot_index = 0 ; repoot_index < 473 ; repoot_index++){
  for(int sensor_index = 0 ; sensor_index < 4 ; sensor_index++){
  Serial.println("________________________");
  int val1 = temp();
  delay(1000); 
     
  int val2 = conductivity();
  delay(1000);

  int val3 = salinity();
  delay(1000);

  int val4 = tds();
  delay(1000);

  int val5 = ph();
  delay(1000);
  
  float val6 = chlorophyll();
  delay(2000);

  //****** Convertir chaque valeur en une chaîne de caractères et les concaténer ********
  char str[50]; // Taille suffisamment grande pour contenir les valeurs et les séparateurs
  sprintf(str, "%d,%d,%d,%d,%d,%f,%d", val1, val2, val3, val4, val5, val6, EndFrame);
  //____________________________________________________________________________________

  //*************** Convertir la chaîne de caractères en un tableau uint8_t*************
  
  for (int i = 0; str[i] != '\0'; ++i) {
      frame[i] = (uint8_t)str[i];
  }
  //____________________________________________________________________________________
  }
  for(int Lora_index = 0 ; Lora_index < 30 ; Lora_index++){
  //************************* afficher la trame à envoyer *******************************
    Serial.println("************************");
    Serial.print("Trame à envoyer : [");
    for (int i = 0; i < sizeof(frame) / sizeof(frame[0]); i++) {
    Serial.print(frame[i]);
      if (i < sizeof(frame) / sizeof(frame[0]) - 1) {
          Serial.print(", "); // Ajoute une virgule après chaque élément sauf le dernier
      }
    }
    Serial.println("]");
    //___________________________________________________________________________________

    
    //****************************** envoyer la trame*************************************
    bool send_result = api.lorawan.psend(sizeof(frame), frame);
    if (send_result) {
    Serial.println("+++++++++++++++P2P send Success+++++++++++++++");
    delay(1000);
    } else {
    Serial.println("/////////////////P2P send Failed/////////////////");
    delay(1000);
    }                                                                        
    Serial.println("************************");
    //___________________________________________________________________________________
    }

    //********************** mettre en veille pendant 5 min ******************************
    Serial.print("The timestamp before sleeping: ");
    Serial.print(millis());
    Serial.println(" ms");
    Serial.println("(Wait 5 min or Press any RESET to wakeup)");
    api.system.sleep.all(300000);
    Serial.print("The timestamp after sleeping: ");
    Serial.print(millis());
    Serial.println(" ms");
    //___________________________________________________________________________________

    if ((millis() - Previous_time) >= Reboot_time){
      api.system.reboot();
    }
  
  }
  
