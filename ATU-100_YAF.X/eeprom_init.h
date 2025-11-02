// eeprom_init.h
// Inicjalizacja EEPROM domy?ln? zawarto?ci? i magicznym bajtem

#ifndef EEPROM_INIT_H
#define EEPROM_INIT_H
#include <stdint.h>
#define EEPROM_INIT_DATA_SIZE 256

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Domy?lna zawarto?? EEPROM (256 bajtów).
 * Definicja znajduje si? w eeprom_init.c.
 */
extern const uint8_t eeprom_init_data[EEPROM_INIT_DATA_SIZE];

/**
 * Inicjalizuje EEPROM domy?ln? zawarto?ci?, je?li nie ustawiono magicznego bajtu.
 * Wywo?aj na pocz?tku w main.c przed normalnym u?yciem EEPROM.
 */
void EEPROM_InitDefault(void);

/**
 * Zapisuje jeden bajt do EEPROM pod wskazany adres.
 * Implementacja dla XC8 znajduje si? w eeprom_init.c.
 */
void eeprom_write(uint8_t addr, uint8_t value);

/**
 * Odczytuje jeden bajt z EEPROM spod wskazanego adresu.
 * Implementacja dla XC8 znajduje sie w eeprom_init.c.
 */
uint8_t eeprom_read(uint8_t addr);

#ifdef __cplusplus
}
#endif

#endif // EEPROM_INIT_H