/* 
 * File:   eeprom.h
 * Author: DG4SN
 *
 * Created on 22. März 2022, 16:09
 */

#ifndef EEPROM_H
#define	EEPROM_H

#ifdef	__cplusplus
extern "C"
{
#endif

  
__eeprom  int16_t ee_tune_stop_swr = 115;
__eeprom  int16_t ee_tune_auto_swr = 190;
__eeprom  uint8_t ee_tune_auto_enable = 0;
__eeprom  uint8_t ee_sleep_enable =0;
__eeprom  int16_t ee_sleep_delay_sec = 30;
__eeprom  int16_t ee_cal_point_0 = 50; //5W
__eeprom  int16_t ee_cal_point_1 = 750; //75W
__eeprom  int16_t ee_cal_offset =150;
__eeprom  int16_t ee_cal_gain = 12000;
__eeprom  tunemem_t ee_tunemem[TUNEMEM_ITEMS] ={
  {0,"Startup "}, //0
  {0,"        "}, //1
  {0,"        "}, //2
  {0,"        "}, //3
  {0,"        "}, //4
  {0,"        "}, //5
  {0,"        "}, //6
  {0,"        "}, //7
  {0,"        "}, //8
  {0,"        "} //9
  
};
          


void EEPROM_Init(void);    
void EEPROM_Write(uint8_t addr, void *ptrValue, uint8_t size);
void EEPROM_Read(uint8_t addr, void *ptrValue, uint8_t size);    
  
  

#ifdef	__cplusplus
}
#endif

#endif	/* EEPROM_H */

