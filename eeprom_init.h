// File: eeprom_init.h

#ifndef EEPROM_INIT_H
#define EEPROM_INIT_H
#include <stdint.h>
#define EEPROM_INIT_DATA_SIZE 256

#ifdef __cplusplus
extern "C" {
#endif

extern const uint8_t eeprom_init_data[EEPROM_INIT_DATA_SIZE]; // Wielkosc EEPROM (256 bajtów)

void EEPROM_InitDefault(void); // Inicjalizuje EEPROM domyslna zawartoscia

void eeprom_write(uint8_t addr, uint8_t value); // Zapisuje jeden bajt do EEPROM pod wskazany adres. Implementacja dla XC8 w eeprom_init.c

uint8_t eeprom_read(uint8_t addr); // Odczytuje jeden bajt z EEPROM spod wskazanego adresu. Implementacja dla XC8 w eeprom_init.c

#ifdef __cplusplus
}
#endif

#endif // EEPROM_INIT_H