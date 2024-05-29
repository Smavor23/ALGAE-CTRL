#include <stdio.h>
#include "stm32f1xx_hal.h"
#include "usart.h"
#include "sim.h"
#include <stdlib.h>
#include <string.h>
#include "main.h"
uint8_t buffer_sim[100] = {0};
char url[] = "http://app.smav-agro.com:8080/api/v1/0308-01/telemetry";
char apn[100] = "iot.telenet.be";
// Tableau de correspondance des valeurs et des RSSI
 int valueToRSSI[] = {
    -109, // index 2 corresponds to value 2
    -107, // index 3 corresponds to value 3
    -105, // index 4 corresponds to value 4
    -103, // index 5 corresponds to value 5
    -101, // index 6 corresponds to value 6
    -99,  // index 7 corresponds to value 7
    -97,  // index 8 corresponds to value 8
    -95,  // index 9 corresponds to value 9
    -93,  // index 10 corresponds to value 10
    -91,  // index 11 corresponds to value 11
    -89,  // index 12 corresponds to value 12
    -87,  // index 13 corresponds to value 13
    -85,  // index 14 corresponds to value 14
    -83,  // index 15 corresponds to value 15
    -81,  // index 16 corresponds to value 16
    -79,  // index 17 corresponds to value 17
    -77,  // index 18 corresponds to value 18
    -75,  // index 19 corresponds to value 19
    -73,  // index 20 corresponds to value 20
    -71,  // index 21 corresponds to value 21
    -69,  // index 22 corresponds to value 22
    -67,  // index 23 corresponds to value 23
    -65,  // index 24 corresponds to value 24
    -63,  // index 25 corresponds to value 25
    -61,  // index 26 corresponds to value 26
    -59,  // index 27 corresponds to value 27
    -57,  // index 28 corresponds to value 28
    -55   // index 29 corresponds to value 29
};

void SIMTransmit(char *cmd)
{
  memset(buffer_sim,0,sizeof(buffer_sim));
  HAL_UART_Transmit(&huart3,(uint8_t *)cmd,strlen(cmd),1000);
  HAL_UART_Receive (&huart3, buffer_sim, 100, 1000);
}


void sendATCommandAndWaitForResponse(const char* command, const char* expectedResponse, uint32_t responseTimeout_ms) {
    int responseReceived = 0;
    uint32_t previousTick = HAL_GetTick();

    while (!responseReceived && (previousTick + responseTimeout_ms > HAL_GetTick())) {
        // Envoyer la commande AT
        SIMTransmit(command);
        debugPrintln(command);

        // Attendre un court délai pour la réponse
        HAL_Delay(1000);

        // Vérifier si la réponse contient la réponse attendue
        if (strstr((char *)buffer_sim, expectedResponse)) {
            responseReceived = 1;
            debugPrintln(buffer_sim);
        }
    }

    if (!responseReceived) {
        debugPrintln("Timeout waiting for response.");
    }
}
void sendATCommandAndWaitForResponse_AT_Action(const char* command, const char* expectedResponse, uint32_t responseTimeout_ms) {
    int responseReceived = 0;
    uint32_t previousTick = HAL_GetTick();

    while (!responseReceived && (previousTick + responseTimeout_ms > HAL_GetTick())) {
        // Envoyer la commande AT
        SIMTransmit(command);
        debugPrintln(command);

        // Attendre un court délai pour la réponse
        HAL_Delay(500);

        // Vérifier si la réponse contient la réponse attendue
        if (strstr((char *)buffer_sim, expectedResponse)) {
            responseReceived = 1;
            debugPrintln(buffer_sim);
        }
    }

    if (!responseReceived) {
        debugPrintln("Timeout waiting for response.");
        sendATCommandAndWaitForResponse("AT+CRESET\r\n", "OK", 3000);
        NVIC_SystemReset();
    }
}
//**************************************** GPS *********************************************************
/*void sendATCommandAndWaitForResponse_gps(const char* command, const char* expectedResponse, uint32_t responseTimeout_ms, char* buffer_gps) {
    int responseReceived = 0;
    uint32_t previousTick = HAL_GetTick();
    //char data_gps [100];

    while (!responseReceived && (previousTick + responseTimeout_ms > HAL_GetTick())) {
        // Envoyer la commande AT
        SIMTransmit(command);
        debugPrintln(command);

        // Attendre un court délai pour la réponse
        HAL_Delay(10000);

        // Vérifier si la réponse contient la réponse attendue
        if (strstr((char *)buffer_sim, expectedResponse)) {
            responseReceived = 1;
            debugPrintln(buffer_sim);
            strcpy(buffer_gps, buffer_sim);

        }
    }

    if (!responseReceived) {
        debugPrintln("Timeout waiting for response.");
    }
}*/
//_____________________________________________________________________________________________________

// Fonction pour extraire la valeur RSSI
int extractRSSIValue(const char* response) {
    int rssi = -1;  // Initialisation à une valeur par défaut
    char* token = strstr(response, "+CSQ: ");
    if (token != NULL) {
        token += 6;  // Avance le pointeur pour ignorer "+CSQ: "
        rssi = atoi(token);  // Convertit la chaîne en entier
    }
    return rssi;
}

// Fonction pour convertir la valeur en RSSI
int getRSSI(int value) {
    if (value >= 2 && value <= 30) {
        return valueToRSSI[value - 2]; // Ajuste l'index pour accéder au bon élément
    } else {
        return -999; // Valeur d'erreur pour les valeurs en dehors de la plage
    }
}

int sendATCommandAndWaitForResponse_signalquality(char* command, char* expectedResponse, uint32_t responseTimeout_ms) {
    int responseReceived = 0;
    uint32_t previousTick = HAL_GetTick();

    while (!responseReceived && (previousTick + responseTimeout_ms > HAL_GetTick())) {
        // Envoyer la commande AT
        SIMTransmit(command);
        debugPrintln(command);

        // Attendre un court délai pour la réponse
        HAL_Delay(1000);

        // Vérifier si la réponse contient la réponse attendue
        if (strstr((char *)buffer_sim, expectedResponse)) {
            responseReceived = 1;
            debugPrintln(buffer_sim);
            int rssi = extractRSSIValue(buffer_sim);
            int RSSI = getRSSI(rssi);
                if (rssi != -999) {
                	return RSSI;
                } else {
                    return -111;
                }
            snprintf(buffer_sim, sizeof(buffer_sim), "RSSI = %d\n", rssi);
            HAL_UART_Transmit(&huart2, (uint8_t *)buffer_sim, strlen(buffer_sim), HAL_MAX_DELAY);

        }
    }

    if (!responseReceived) {
        debugPrintln("Timeout waiting for response.");
    }
}

void SIM_INIT(){

	  sendATCommandAndWaitForResponse_AT_Action("AT\r\n", "OK", 3000);
	  HAL_Delay(500);
	  //************************* GPS **********************************
	  /*sendATCommandAndWaitForResponse("AT+CGPS=1\r\n", "OK", 3000);
	  HAL_Delay(500);*/
	  //________________________________________________________________
	  sendATCommandAndWaitForResponse("AT+CREG?\r\n", "OK", 3000);
	  HAL_Delay(500);
	  sendATCommandAndWaitForResponse("AT+CNMP=2\r\n", "OK", 3000);
	  HAL_Delay(500);
	  sprintf(ATcommand,"AT+CGDCONT=1,\"IP\",\"%s\"\r\n", apn);
	  //sprintf(ATcommand,"AT+CGDCONT=1,\"IP\",\"iot.telenet.be\"\r\n");
	  sendATCommandAndWaitForResponse(ATcommand, "OK", 3000);
	  HAL_Delay(500);

}
//********************************** GPS *******************************
/*
Coordinates parseNMEA(char* nmea) {
    Coordinates coords;
    char* token = strtok(nmea, ",");

    // Extract latitude
    if (token != NULL) {
        coords.latitude = atof(token);
        token = strtok(NULL, ",");
    }

    // Extract latitude direction
    if (token != NULL) {
        coords.latDirection = token[0];
        token = strtok(NULL, ",");
    }

    // Extract longitude
    if (token != NULL) {
        coords.longitude = atof(token);
        token = strtok(NULL, ",");
    }

    // Extract longitude direction
    if (token != NULL) {
        coords.lonDirection = token[0];
        token = strtok(NULL, ",");
    }

    return coords;
}

void extractData(const char* input, char* output) {
    // Find the position of the colon
    const char* colonPos = strchr(input, ':');

    // Check if colon was found
    if (colonPos != NULL) {
        // Move the pointer to the next character after the colon
        colonPos++;

        // Copy the data after the colon to the output buffer
        strcpy(output, colonPos);
    } else {
        // Handle error case where colon is not found
        strcpy(output, "");
    }
}

float convertDMSToDecimal(float dms, char direction) {
    int degrees = (int)(dms / 100);   // Extract degrees
    float minutes = fmod(dms, 100);   // Extract minutes
    float decimal = degrees + (minutes / 60.0);
    if (direction == 'S' || direction == 'W') {
        decimal = -decimal;
    }
    return decimal;
}
*/
//_____________________________________________________________________
void HTTPConnect(char* data_Json){
	sendATCommandAndWaitForResponse("AT+HTTPINIT\r\n", "OK", 3000);
	HAL_Delay(500);
	snprintf(ATcommand, sizeof(ATcommand), "AT+HTTPPARA=\"URL\",\"%s\"\r\n", url);
	sendATCommandAndWaitForResponse(ATcommand, "OK", 3000);
	HAL_Delay(500);
	sprintf(ATcommand,"AT+HTTPPARA=\"CONTENT\",\"application/json\"\r\n");
	sendATCommandAndWaitForResponse(ATcommand, "OK", 3000);
	HAL_Delay(500);
	snprintf(ATcommand, sizeof(ATcommand), "AT+HTTPDATA=%d,10000\r\n", strlen(data_Json));
	sendATCommandAndWaitForResponse(ATcommand, "DOWNLOAD", 6000);
	HAL_Delay(500);
	sendATCommandAndWaitForResponse(data_Json, "OK", 6000);
	sendATCommandAndWaitForResponse_AT_Action("AT+HTTPACTION=1\r\n", "OK", 3000);
	//sendATCommandAndWaitForResponse("AT+HTTPREAD=0,500\r\n","OK",3000);
	HAL_Delay(3000);
	sendATCommandAndWaitForResponse("AT+HTTPTERM\r\n", "OK", 3000);
	HAL_Delay(500);
}
