/*
 * Scheduler.c

 *
 *  Created on: Jun 26, 2023
 *      Author: VincentD'Agostino
 */

#include "Project.h"
#include <stdint.h>
#include "Scheduler.h"



uint16_t ten_mS_Counter = TEN_mS_SEED;
volatile uint8_t ten_mS_Flag = false;

uint16_t twentyfive_mS_Counter = TWENTYFIVE_mS_SEED;
volatile uint8_t twentyfive_mS_Flag = false;

uint16_t hundred_mS_Counter = ONEHUNDRED_mS_SEED;
volatile uint8_t hundred_mS_Flag = false;

uint16_t one_S_Counter = ONE_S_SEED;
volatile uint8_t one_S_Flag = false;

uint16_t i2c_timeout_Counter = I2C_TIMEOUT_INIT;
volatile uint8_t i2c_timeout_Flag;


uint16_t reportStreamInterval_Counter = 1000;
volatile uint8_t sendReport_Flag = false;
extern uint16_t streamIntervalDelay;


void SysTick_Handler(void)
{
    ten_mS_Counter--;
    if (ten_mS_Counter == 0) {
      ten_mS_Flag = true;
      ten_mS_Counter = TEN_mS_SEED;
    }

    twentyfive_mS_Counter--;
    if (twentyfive_mS_Counter == 0) {
      twentyfive_mS_Flag = true;
      twentyfive_mS_Counter = TWENTYFIVE_mS_SEED;
    }

    hundred_mS_Counter--;
    if (hundred_mS_Counter == 0) {
      hundred_mS_Flag = true;
      hundred_mS_Counter = ONEHUNDRED_mS_SEED;
    }

    one_S_Counter--;
    if (one_S_Counter == 0) {
      one_S_Flag = true;
      one_S_Counter = ONE_S_SEED;
    }

    if (i2c_timeout_Counter != 0) {
        i2c_timeout_Counter--;
        if (i2c_timeout_Counter == 0) {
            i2c_timeout_Flag = true;
        }
    }

#if STREAMING == true
    reportStreamInterval_Counter--;
    if (reportStreamInterval_Counter == 0) {
        sendReport_Flag = true;
        reportStreamInterval_Counter = streamIntervalDelay;
    }
#endif
  }

void SetI2cTimeout(uint16_t _timeout)
{
    i2c_timeout_Counter = _timeout;
    i2c_timeout_Flag = false;
}

