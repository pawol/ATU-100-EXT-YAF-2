// File: button.c

#include "defines.h"

//button_state_t BUTTON_state;

uint8_t BUTTON_count;
uint8_t BUTTON_was_long_pressed;
uint8_t BUTTON_Auto_count;
uint8_t BUTTON_Bypass_count;


void BUTTON_Init(void)
{
  BUTTON_TUNE_DIR = DIR_INPUT;
  BUTTON_TUNE_PULLUP = 1;
  
  BUTTON_AUTO_DIR = DIR_INPUT;
  BUTTON_AUTO_PULLUP = 1; 
 
  BUTTON_BYPASS_DIR = DIR_INPUT;
  BUTTON_BYPASS_PULLUP = 1;
  
  //BUTTON_state.byte = BUTTON_STATE_RESET;
}


void BUTTON_Reset(void)
{
  BUTTON_count=BUTTON_RESET;
  BUTTON_was_long_pressed =0;
}

void BUTTON_Run(void)
{
 
if (BUTTON_TUNE_R == 0) //Button pressed
{  
  if(BUTTON_count < BUTTON_MAX_COUNT)
  {
    BUTTON_count++;
  }
  
  if(BUTTON_count == BUTTON_LONG_PRESSED)
  {
    BUTTON_was_long_pressed =1;
  }
  
}
else
{
  if((!BUTTON_was_long_pressed) && (BUTTON_count > 0) && (BUTTON_count <= BUTTON_MAX_COUNT))
  {
    BUTTON_count = BUTTON_RELEASED;
  }
  else
  {
    BUTTON_count =BUTTON_IDLE;
  }
  
  BUTTON_was_long_pressed =0;
}  
  
//-----------------------------------------------------------------------  

  // Button Auto -------------------------------------------------
  if (BUTTON_AUTO_R == 0) //Button pressed
  {
    BUTTON_Auto_count++;
    if(BUTTON_Auto_count >= BUTTON_AUTO_RE_TRIGGER)
    {
      BUTTON_Auto_count=1;
    }
  }
  else
  {
    BUTTON_Auto_count =0;
  }
  
  // Button Bypass -------------------------------------------------
  if (BUTTON_BYPASS_R == 0) //Button pressed
  {
    BUTTON_Bypass_count++;
    if(BUTTON_Bypass_count >= BUTTON_AUTO_RE_TRIGGER)
    {
      BUTTON_Bypass_count=1;
    }
  }
  else
  {
    BUTTON_Bypass_count =0;
  }
  
}
