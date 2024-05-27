#include <stdio.h>
#include "Batt_Level.h"
#include "stm32f1xx_hal.h"
#include "adc.h"
#include "usart.h"

#define BATTERIE_PIN GPIO_PIN_0 // Supposons que le pin analogique est GPIOA Pin 0
#define BATTERIE_PORT GPIOB // Supposons que le pin est sur le port GPIOB
#define TENSION_STABLE_PIN GPIO_PIN_7 // Supposons que le pin analogique de la tension stable est GPIOA Pin 1
#define TENSION_STABLE_PORT GPIOA // Supposons que le pin de la tension stable est sur le port GPIOB
char buffer1[400];
uint16_t raw_batterie;
uint16_t raw_tension_stable;
int pourcentageBatterie;
// Fonction pour lire la valeur de la batterie
int lireTensionBatterie() {
    GPIO_InitTypeDef GPIO_InitStruct;

    // Activer l'horloge pour le port GPIO de la batterie
    __HAL_RCC_GPIOB_CLK_ENABLE();

    // Configurer le pin GPIO de la batterie pour une entrée analogique
    GPIO_InitStruct.Pin = BATTERIE_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(BATTERIE_PORT, &GPIO_InitStruct);

    // Attendre un court instant pour que le GPIO se stabilise
    HAL_Delay(10);

    // Lecture de la valeur analogique
    HAL_ADC_Start(&hadc1);
    if (HAL_ADC_PollForConversion(&hadc1, HAL_MAX_DELAY) == HAL_OK) {
        HAL_ADC_PollForConversion(&hadc1, 300);
        raw_batterie = HAL_ADC_GetValue(&hadc1);
        float tensionBatterie = raw_batterie * (3.3 / 4096) * 1.054;
        tensionBatterie = (tensionBatterie * 37500) / 7500;

        // Calcul du pourcentage de charge de la batterie
        int pourcentageBatterie;
        if (tensionBatterie > 13) {
            pourcentageBatterie = 90;
        } else if (tensionBatterie > 10 && tensionBatterie <= 13) {
            pourcentageBatterie = ((tensionBatterie - 10) / (13 - 10)) * 100;
        } else {
            pourcentageBatterie = 5;
        }

        // Affichage de la tension de la batterie via UART
        snprintf(buffer1, sizeof(buffer1), "\nLe pourcentage de la batterie en pourcentage : %d \nLa tension du batterie %.2f", pourcentageBatterie, tensionBatterie);
        HAL_UART_Transmit(&huart2, (uint8_t *)buffer1, strlen(buffer1), HAL_MAX_DELAY);

        return pourcentageBatterie;
    } else {
        // Gestion de l'échec de la conversion ADC
        snprintf(buffer1, sizeof(buffer1), "\nÉchec de la conversion ADC pour la batterie");
        HAL_UART_Transmit(&huart2, (uint8_t *)buffer1, strlen(buffer1), HAL_MAX_DELAY);

        return -1.0f; // Valeur d'erreur
    }
}


// Fonction pour lire la tension stable
int lireTensionStable() {
    GPIO_InitTypeDef GPIO_InitStruct;
    int VoltageState;
    // Activer l'horloge pour le port GPIO de la tension stable
    __HAL_RCC_GPIOA_CLK_ENABLE();

    // Configurer le pin GPIO de la tension stable pour une entrée analogique
    GPIO_InitStruct.Pin = TENSION_STABLE_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(TENSION_STABLE_PORT, &GPIO_InitStruct);

    // Attendre un court instant pour que le GPIO se stabilise
    HAL_Delay(10);

    // Lecture de la valeur analogique de la tension stable
    HAL_ADC_Start(&hadc2); // Assurez-vous que hadc2 est correctement configuré pour votre tension stable
    if (HAL_ADC_PollForConversion(&hadc2, HAL_MAX_DELAY) == HAL_OK) {
        HAL_ADC_PollForConversion(&hadc2, 300);
        raw_tension_stable = HAL_ADC_GetValue(&hadc2);
        float tensionStable = raw_tension_stable * (3.3 / 4096);
        tensionStable = (tensionStable * 37500) / 7500;
        // Affichage de la tension stable via UART
        if (tensionStable > 1)
        {
        	VoltageState = 1;
        }
        else if (tensionStable < 1)
        {
        	VoltageState = 0;
        }
        snprintf(buffer1, sizeof(buffer1), "\nLa tension stable est : %d", VoltageState);
        HAL_UART_Transmit(&huart2, (uint8_t *)buffer1, strlen(buffer1), HAL_MAX_DELAY);


        return VoltageState;
    } else {
        // Gestion de l'échec de la conversion ADC
        snprintf(buffer1, sizeof(buffer1), "\nÉchec de la conversion ADC pour la tension stable");
        HAL_UART_Transmit(&huart2, (uint8_t *)buffer1, strlen(buffer1), HAL_MAX_DELAY);

        return -1.0f; // Valeur d'erreur
    }
}
