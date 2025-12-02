// File: tune.c

#include "defines.h"

#ifdef DEBUG_UART
#endif

#define TUNE_COARSE_FAST

#define RELAY_DELAY    2  // 2 = 20ms
#define AddTolerance(x) (x + (x / 20)) // +5%

#define COARSE_CAP_STEPS   11 
const uint8_t coarse_cap_value[COARSE_CAP_STEPS] = {
  0b00000000,
  0b00000001,
  0b00000010,
  0b00000100,
  0b00001000,
  0b00010000,
  0b00100000,
  0b01000000,
  0b01100000,
  0b01110000,
  0b01111111
};

#define COARSE_IND_STEPS   11 
const uint8_t coarse_ind_value[COARSE_IND_STEPS] = {
  0b00000000,
  0b00000001,
  0b00000010,
  0b00000100,
  0b00001000,
  0b00010000,
  0b00100000,
  0b01000000,
  0b01100000,
  0b01110000,
  0b01111111
};

enum
{
  fine_low = 0, fine_mid, fine_high
};

tunestate_t TUNE_state;
static uint8_t TUNE_step;
static uint8_t TUNE_delay;
static uint8_t best_swr_valid; // Flaga czy znaleziono cokolwiek sensownego

void TUNE_Init(void)
{
  TUNE_state = TUNE_IDLE;
  TUNE_step = 0;
  TUNE_delay = 0;
  best_swr_valid = 0;

  global.cap_sw = 0;
  global.cap_relays = 0;
  global.ind_relays = 0;
  UTILI_SetRelays();
  TUNE_delay = RELAY_DELAY;
}

void TUNE_Run(void)
{
  static uint8_t ind_step;
  static uint8_t cap_step;
  static int16_t best_swr;

  static uint8_t best_cap_step;
  static uint8_t best_ind_step;
  static uint8_t best_cap_sw;

  static int16_t best_row_swr;
  static int16_t best_last_row_swr;

  static uint8_t fine_cap_diff_low;
  static uint8_t fine_cap_diff_high;
  static uint8_t fine_ind_diff_low;
  static uint8_t fine_ind_diff_high;

  static uint8_t fine_cap[3];
  static uint8_t fine_ind[3];

#ifdef TUNE_DEBUG
  char str[7];
#endif

  if ((TUNE_state == TUNE_BREAK) || (TUNE_state == TUNE_READY)) return;

  if (TUNE_delay > 0)
  {
    TUNE_delay--;
    return;
  }

#ifdef TUNE_DEBUG
  if (UART_Busy()) return;
#endif

  UTILI_CalPWR();
  UTILI_CalSWR();

  if ((TUNE_step != 0) && (global.PWR < TUNE_MIN_PWR))
  {
    TUNE_state = TUNE_PWRLOW;
    return;
  }

  switch (TUNE_step)
  {
    case 0:
      if (global.PWR >= TUNE_MIN_PWR)
      {
        TUNE_step = 10;
        TUNE_state = TUNE_RUN;
        best_swr = MAX_SWR;
        best_row_swr = MAX_SWR;
        best_last_row_swr = MAX_SWR;
        ind_step = 0;
        cap_step = 0;
        best_swr_valid = 0;
        TUNE_delay = RELAY_DELAY;

#ifdef TUNE_DEBUG
        UART_WriteLn();
        UART_WriteStrLn("Tune Start");
        UART_WriteStrLn("CAP_SW=0");
#endif
      }
      break;

    case 10: // coarse tuning
      if (global.SWR <= global.tune_stop_swr)
      {
        TUNE_state = TUNE_READY;
        break;
      }

      if (global.SWR < best_row_swr)
        best_row_swr = global.SWR;

      if (global.SWR < best_swr)
      {
        best_swr = global.SWR;
        best_cap_step = cap_step;
        best_ind_step = ind_step;
        best_cap_sw = global.cap_sw;
        best_swr_valid = 1;
      }

      cap_step++;

#ifdef TUNE_COARSE_FAST
      if ((cap_step >= COARSE_CAP_STEPS) || (global.SWR > AddTolerance(best_row_swr)))
#else
      if (cap_step >= COARSE_CAP_STEPS)
#endif
      {
        cap_step = 0;
        ind_step++;

#ifdef TUNE_COARSE_FAST
        if (AddTolerance(best_last_row_swr) < best_row_swr)
        {
          if (global.cap_sw == 1)
          {
            TUNE_step = 20;
            break;
          }
          else
          {
            global.cap_sw = 1;
            cap_step = 0;
            ind_step = 0;
            best_row_swr = MAX_SWR;
            best_last_row_swr = MAX_SWR;
          }
        }
        else
        {
          best_last_row_swr = best_row_swr;
        }
        best_row_swr = MAX_SWR;
#endif
        if (ind_step >= COARSE_IND_STEPS)
        {
          ind_step = 0;
          best_row_swr = MAX_SWR;
          if (global.cap_sw == 1)
          {
            TUNE_step = 20;
            break;
          }
          else
          {
            global.cap_sw = 1;
          }
        }
      }

      global.cap_relays = coarse_cap_value[cap_step];
      global.ind_relays = coarse_ind_value[ind_step];
      UTILI_SetRelays();
      TUNE_delay = RELAY_DELAY;
      break;

    case 20: // fine tuning - init
      if (!best_swr_valid)
      {
        // No valid config found ? fallback to clean state but finish gracefully
        global.cap_sw = 0;
        global.cap_relays = 0;
        global.ind_relays = 0;
        UTILI_SetRelays();
        TUNE_state = TUNE_READY;
        return;
      }

      global.cap_sw     = best_cap_sw;
      global.cap_relays = coarse_cap_value[best_cap_step];
      global.ind_relays = coarse_ind_value[best_ind_step];

      fine_cap[fine_mid] = coarse_cap_value[best_cap_step];
      fine_ind[fine_mid] = coarse_ind_value[best_ind_step];

      fine_cap_diff_low  = (best_cap_step > 0) ? (coarse_cap_value[best_cap_step] - coarse_cap_value[best_cap_step - 1]) / 2 : 0;
      fine_cap_diff_high = (best_cap_step < (COARSE_CAP_STEPS - 1)) ? (coarse_cap_value[best_cap_step + 1] - coarse_cap_value[best_cap_step]) / 2 : 0;
      fine_ind_diff_low  = (best_ind_step > 0) ? (coarse_ind_value[best_ind_step] - coarse_ind_value[best_ind_step - 1]) / 2 : 0;
      fine_ind_diff_high = (best_ind_step < (COARSE_IND_STEPS - 1)) ? (coarse_ind_value[best_ind_step + 1] - coarse_ind_value[best_ind_step]) / 2 : 0;

      fine_cap[fine_low]  = fine_cap[fine_mid] - fine_cap_diff_low;
      fine_cap[fine_high] = fine_cap[fine_mid] + fine_cap_diff_high;
      fine_ind[fine_low]  = fine_ind[fine_mid] - fine_ind_diff_low;
      fine_ind[fine_high] = fine_ind[fine_mid] + fine_ind_diff_high;

      best_swr = MAX_SWR;

      cap_step = fine_mid;
      ind_step = fine_mid;

      global.cap_relays = fine_cap[cap_step];
      global.ind_relays = fine_ind[ind_step];
      UTILI_SetRelays();
      TUNE_delay = RELAY_DELAY;
      TUNE_step = 30;
      break;

    case 30: // fine tuning - run
      if (global.SWR <= global.tune_stop_swr)
      {
        TUNE_state = TUNE_READY;
        break;
      }

      if (global.SWR < best_swr)
      {
        best_swr = global.SWR;
        best_cap_step = cap_step;
        best_ind_step = ind_step;
      }

      do
      {
        if (cap_step == fine_mid)
        {
          if (fine_cap[fine_low] != fine_cap[fine_mid])
          {
            cap_step = fine_low;
            break;
          }
          else if (fine_cap[fine_high] != fine_cap[fine_mid])
          {
            cap_step = fine_high;
            break;
          }
        }

        if (cap_step == fine_low && fine_cap[fine_high] != fine_cap[fine_mid])
        {
          cap_step = fine_high;
          break;
        }

        cap_step = fine_mid;

        if (ind_step == fine_mid)
        {
          if (fine_ind[fine_low] != fine_ind[fine_mid])
          {
            ind_step = fine_low;
            break;
          }
          else if (fine_ind[fine_high] != fine_ind[fine_mid])
          {
            ind_step = fine_high;
            break;
          }
        }

        if (ind_step == fine_low && fine_ind[fine_high] != fine_ind[fine_mid])
        {
          ind_step = fine_high;
          break;
        }

        ind_step = fine_mid;

      } while ((cap_step != fine_mid) || (ind_step != fine_mid));

      if ((cap_step == fine_mid) && (ind_step == fine_mid))
      {
        global.cap_relays = fine_cap[best_cap_step];
        global.ind_relays = fine_ind[best_ind_step];
        UTILI_SetRelays();
        TUNE_state = TUNE_READY;
        break;
      }

      global.cap_relays = fine_cap[cap_step];
      global.ind_relays = fine_ind[ind_step];
      UTILI_SetRelays();
      TUNE_delay = RELAY_DELAY;
      break;

    default:
      TUNE_state = TUNE_BREAK;
      break;
  }
}