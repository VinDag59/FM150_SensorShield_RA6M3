/*
* Copyright (c) 2020 - 2025 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
*/

#include "hal_data.h"
#include "Project.h"
#include "Scheduler.h"
#include "SensorFunctions.h"
#include <string.h>
#include <stdio.h>



//--------------------------------
// Macro Definitions
//#define SENSOR_HISTORY_LENGTH 8
//#define SENSOR_1_SS R_PORT4->PODR_b.PODR15
//#define SENSOR_2_SS R_PORT4->PODR_b.PODR14
//#define SENSOR_3_SS R_PORT6->PODR_b.PODR11
//#define SENSOR_BAROMETER_SS R_PORT5->PODR_b.PODR8
//
//#define SS_ASSERTED 0
//#define SS_DEASSERTED 1

#define SPI_CLK_IDLE_HIGH 0
#define SPI_CLK_IDLE_LOW 1
#define SPI_DATA_SAMPLE_ODD_EDGE 0
#define SPI_DATA_SAMPLE_EVEN_EDGE 1



//--------------------------------
// Structures, enums, unions, typdefs


//--------------------------------
// Variables
fsp_err_t status = FSP_SUCCESS;
fsp_err_t err     = FSP_SUCCESS;


// === I2C
#define SENSOR_I2C_BUS_ADDRESS 0x44
static volatile i2c_master_event_t i2c_event = I2C_MASTER_EVENT_ABORTED;
volatile uint8_t i2cBusy = false;

// *** Temp/hum
uint8_t sensorRegisters[6];
const uint8_t cmdRead[2] = {0x24,0x16}; // read temp/hum in Low repeatibility and no clock stretching
uint8_t getTempHumState = 0;
int16_t currentTempF = 40;
int16_t currentHum = 10;

sensor_data_t temperatureSensor;
sensor_data_t humiditySensor;

// === SPI
// *** Pressure Sensors
volatile uint8_t transferNotComplete;

// Data Structures
sensor_data_int32_t pressureSensor1;
sensor_data_int32_t pressureSensor2;
sensor_data_int32_t pressureSensor3;
se_sensor_data_t pressureSensorBarometer;

// byte swapper scratchpad for endian-ness
union {
    uint8_t byteAccess[2];
    uint16_t wordAccess;
} bar_convert;

// Commands and specific config structures
uint8_t spiCmdStartSingle[3] = {0xAA, 0x00, 0x00};
uint8_t spiCmdReadValue[7] = {0xF0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
spi_cfg_t dlhr_spi0_cfg;

uint8_t spiCmdGetBarometricPressure[2] = {0x20, 0x00};
uint8_t spiCmdGetTemp[2] = {0x40, 0x00};
spi_cfg_t bar_spi0_cfg;

uint8_t spiReadSensorData[7] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

// Sensor data structure init values
const sensor_data_int32_t initValues0Int32 = {.byteAccess ={0,0, 0,0, 0,0, 0,0, 0,0, 0,0, 0,0, 0,0, 0,0,0,0, 0,0,0,0, 0}};
const sensor_data_t initValues0 = {.byteAccess ={0,0, 0,0, 0,0, 0,0, 0,0, 0,0, 0,0, 0,0, 0,0,0,0, 0,0, 0}};
const se_sensor_data_t initSeValues0 = {.byteAccess ={0,0, 0,0, 0,0, 0,0, 0,0, 0,0, 0,0, 0,0, 0,0,0,0, 0,0, 0}};



//----------------------------------
// Prototypes
void spi_callback(spi_callback_args_t *p_args);
void sci_i2c_master_callback(i2c_master_callback_args_t *p_args);
void R_BSP_WarmStart(bsp_warm_start_event_t event);


//----------------------------------
// Code Start

/*******************************************************************************************************************//**
 * @brief  Blinky example application
 *
 * Blinks all leds at a rate of 1 second using the software delay function provided by the BSP.
 *
 **********************************************************************************************************************/
void hal_entry (void)
{
    /* Define the units to be used with the software delay function */
    const bsp_delay_units_t bsp_delay_units = BSP_DELAY_UNITS_MILLISECONDS;

    /* Set the blink frequency (must be <= bsp_delay_units / 2) */
    const uint32_t freq_in_hz = 1;

    /* Calculate the delay in terms of bsp_delay_units */
    const uint32_t delay = bsp_delay_units / (freq_in_hz * 2);


    // -----------------------------------------
    // Set up peripherals
    // Give me a SysTick every 1mS
    SysTick_Config (SystemCoreClock / 1000);

    memcpy(&bar_spi0_cfg, &g_spi0_cfg, sizeof(g_spi0_cfg));
    bar_spi0_cfg.clk_phase = SPI_CLK_PHASE_EDGE_ODD;
    bar_spi0_cfg.clk_polarity = SPI_CLK_POLARITY_LOW;
//    memcpy(&bar_spi0_ext_cfg, &g_spi0_cfg_extend, sizeof(g_spi0_cfg_extend));
//    bar_spi0_cfg.p_extend = &bar_spi0_ext_cfg;
//    bar_spi0_ext_cfg.clock_phase = SPI_CLK_IDLE_LOW;
//    bar_spi0_ext_cfg.data_phase = SPI_DATA_SAMPLE_ODD_EDGE;

    memcpy(&dlhr_spi0_cfg, &g_spi0_cfg, sizeof(g_spi0_cfg));
    dlhr_spi0_cfg.clk_phase = SPI_CLK_PHASE_EDGE_ODD;
    dlhr_spi0_cfg.clk_polarity = SPI_CLK_POLARITY_LOW;
//    memcpy(&dlhr_spi0_ext_cfg, &g_spi0_cfg_extend, sizeof(g_spi0_cfg_extend));
//    dlhr_spi0_cfg.p_extend = &dlhr_spi0_ext_cfg;
//    dlhr_spi0_ext_cfg.clock_phase = SPI_CLK_IDLE_LOW; //1;
//    dlhr_spi0_ext_cfg.data_phase = SPI_DATA_SAMPLE_ODD_EDGE; //0;


    /* Holds level to set for pins */
    bsp_io_level_t pin_level = BSP_IO_LEVEL_LOW;

    err = R_SCI_I2C_Open(&g_i2c0_ctrl, &g_i2c0_cfg);

    while (1)
    {
        //---------------------------------
        // 10mS Tasks
        if (ten_mS_Flag) {
          ten_mS_Flag = false;

          switch (getTempHumState++) {
          case 0:
              // Temp/Hum
              err = R_SCI_I2C_Write(&g_i2c0_ctrl, (uint8_t *)&cmdRead[0], 2, false);

              // Pressure Setup
              status = R_SPI_Close(&g_spi0_ctrl);
              status = R_SPI_Open(&g_spi0_ctrl,&dlhr_spi0_cfg);

              // Pressure 1
              R_BSP_SoftwareDelay(1, BSP_DELAY_UNITS_MICROSECONDS);
              SENSOR_1_SS = SS_ASSERTED;
              R_BSP_SoftwareDelay(1, BSP_DELAY_UNITS_MICROSECONDS);
              status = R_SPI_Write(&g_spi0_ctrl, spiCmdStartSingle, 3, SPI_BIT_WIDTH_8_BITS);
              R_BSP_SoftwareDelay(50, BSP_DELAY_UNITS_MICROSECONDS);
              //R_BSP_SoftwareDelay(1, BSP_DELAY_UNITS_MILLISECONDS);
              SENSOR_1_SS = SS_DEASSERTED;

              // Pressure 2
              R_BSP_SoftwareDelay(1, BSP_DELAY_UNITS_MICROSECONDS);
              SENSOR_2_SS = SS_ASSERTED;
              R_BSP_SoftwareDelay(1, BSP_DELAY_UNITS_MICROSECONDS);
              status = R_SPI_Write(&g_spi0_ctrl, spiCmdStartSingle, 3, SPI_BIT_WIDTH_8_BITS);
              R_BSP_SoftwareDelay(50, BSP_DELAY_UNITS_MICROSECONDS);
              //R_BSP_SoftwareDelay(1, BSP_DELAY_UNITS_MILLISECONDS);
              SENSOR_2_SS = SS_DEASSERTED;

              // Pressure 3
              R_BSP_SoftwareDelay(1, BSP_DELAY_UNITS_MICROSECONDS);
              SENSOR_3_SS = SS_ASSERTED;
              R_BSP_SoftwareDelay(1, BSP_DELAY_UNITS_MICROSECONDS);
              status = R_SPI_Write(&g_spi0_ctrl, spiCmdStartSingle, 3, SPI_BIT_WIDTH_8_BITS);
              R_BSP_SoftwareDelay(50, BSP_DELAY_UNITS_MICROSECONDS);
              //R_BSP_SoftwareDelay(1, BSP_DELAY_UNITS_MILLISECONDS);
              SENSOR_3_SS = SS_DEASSERTED;
              break;
          case 1:
              // Temp/Hum
              err = R_SCI_I2C_Read(&g_i2c0_ctrl, &sensorRegisters[0], 6, false);

              // Pressure 1
              SENSOR_1_SS = SS_ASSERTED;
              R_BSP_SoftwareDelay(5, BSP_DELAY_UNITS_MICROSECONDS);
              status = R_SPI_WriteRead(&g_spi0_ctrl, spiCmdReadValue, spiReadSensorData, 7, SPI_BIT_WIDTH_8_BITS);
              R_BSP_SoftwareDelay(50, BSP_DELAY_UNITS_MICROSECONDS);
              //R_BSP_SoftwareDelay(1, BSP_DELAY_UNITS_MILLISECONDS);
              SENSOR_1_SS = SS_DEASSERTED;
              AddSensorInt32Value(&pressureSensor1, (spiReadSensorData[1] << 16) + (spiReadSensorData[2] << 8) + spiReadSensorData[3]);

              // Pressure 2
              SENSOR_2_SS = SS_ASSERTED;
              R_BSP_SoftwareDelay(5, BSP_DELAY_UNITS_MICROSECONDS);
              status = R_SPI_WriteRead(&g_spi0_ctrl, spiCmdReadValue, spiReadSensorData, 7, SPI_BIT_WIDTH_8_BITS);
              R_BSP_SoftwareDelay(50, BSP_DELAY_UNITS_MICROSECONDS);
              //R_BSP_SoftwareDelay(1, BSP_DELAY_UNITS_MILLISECONDS);
              SENSOR_2_SS = SS_DEASSERTED;
              AddSensorInt32Value(&pressureSensor2, (spiReadSensorData[1] << 16) + (spiReadSensorData[2] << 8) + spiReadSensorData[3]);

              // Pressure 3
              SENSOR_3_SS = SS_ASSERTED;
              R_BSP_SoftwareDelay(5, BSP_DELAY_UNITS_MICROSECONDS);
              status = R_SPI_WriteRead(&g_spi0_ctrl, spiCmdReadValue, spiReadSensorData, 7, SPI_BIT_WIDTH_8_BITS);
              R_BSP_SoftwareDelay(50, BSP_DELAY_UNITS_MICROSECONDS);
              //R_BSP_SoftwareDelay(1, BSP_DELAY_UNITS_MILLISECONDS);
              SENSOR_3_SS = SS_DEASSERTED;
              AddSensorInt32Value(&pressureSensor3, (spiReadSensorData[1] << 16) + (spiReadSensorData[2] << 8) + spiReadSensorData[3]);
              break;
          case 2:
              // Temp/Hum
              currentTempF = (int16_t)(
                      (int32_t)(
                      (int32_t)(
                      (int32_t)(((int32_t)sensorRegisters[0] << 8) + sensorRegisters[1])
                      * 3150)
                      / 0xFFFF)
                      - 490);

              currentHum = (int16_t)(
                      (uint32_t)(
                      (uint32_t)(
                      (uint32_t)(((uint32_t)sensorRegisters[3] << 8) + sensorRegisters[4])
                      *1000)
                      /0xFFFF));

              // Barometer
              // Pressure Setup
              status = R_SPI_Close(&g_spi0_ctrl);
              status = R_SPI_Open(&g_spi0_ctrl,&bar_spi0_cfg);
              R_BSP_SoftwareDelay(1, BSP_DELAY_UNITS_MICROSECONDS);
              SENSOR_BAROMETER_SS = SS_ASSERTED;
              R_BSP_SoftwareDelay(1, BSP_DELAY_UNITS_MICROSECONDS);
              status = R_SPI_WriteRead(&g_spi0_ctrl,spiCmdGetBarometricPressure, spiReadSensorData, 2, SPI_BIT_WIDTH_8_BITS);
              R_BSP_SoftwareDelay(50, BSP_DELAY_UNITS_MICROSECONDS);
              SENSOR_BAROMETER_SS = SS_DEASSERTED;
              bar_convert.byteAccess[0] = spiReadSensorData[1];
              bar_convert.byteAccess[1] = spiReadSensorData[0];
              AddSensorUint16Value(&pressureSensorBarometer, (uint16_t)((uint16_t)((int16_t)(bar_convert.wordAccess >> 1) & 0x0FFF) - (int16_t)(-2340)) / (uint16_t)39);
              break;
          case 3:
              // Temp/Hum
              AddSensorInt16Value(&temperatureSensor, currentTempF);
              AddSensorInt16Value(&humiditySensor, currentHum);
              getTempHumState = 0;
              break;
          }
        }  // end of 10mS Tasks
        //---------------------------------


        //---------------------------------
        // 25mS Tasks
        if (twentyfive_mS_Flag) {
          twentyfive_mS_Flag = false;

        }  // end of 25mS Tasks
        //---------------------------------


        //---------------------------------
        // 100mS Tasks
        if (hundred_mS_Flag) {
          hundred_mS_Flag = false;


        }  // end of 100mS Tasks
        //---------------------------------


        //---------------------------------
        // 1 Sec Tasks
        if (one_S_Flag) {
          one_S_Flag = false;

          BOARD_MOUNTED_RED_LED = pin_level;
          BOARD_MOUNTED_GREEN_LED  = pin_level;
          BOARD_MOUNTED_BLUE_LED  = pin_level;

          /* Toggle level for next write */
          if (BSP_IO_LEVEL_LOW == pin_level)
          {
              pin_level = BSP_IO_LEVEL_HIGH;
          }
          else
          {
              pin_level = BSP_IO_LEVEL_LOW;
          }
        } // end of 1Sec Tasks
        //---------------------------------


        //---------------------------------
        // Every time through the loop

        // end Every time through the loop
        //---------------------------------


    }
}

/*******************************************************************************************************************//**
 * This function is called at various points during the startup process.  This implementation uses the event that is
 * called right before main() to set up the pins.
 *
 * @param[in]  event    Where at in the start up process the code is currently at
 **********************************************************************************************************************/
void R_BSP_WarmStart (bsp_warm_start_event_t event)
{
    if (BSP_WARM_START_RESET == event)
    {
#if BSP_FEATURE_FLASH_LP_VERSION != 0

        /* Enable reading from data flash. */
        R_FACI_LP->DFLCTL = 1U;

        /* Would normally have to wait tDSTOP(6us) for data flash recovery. Placing the enable here, before clock and
         * C runtime initialization, should negate the need for a delay since the initialization will typically take more than 6us. */
#endif
    }

    if (BSP_WARM_START_POST_C == event)
    {
        /* C runtime environment and system clocks are setup. */

        /* Configure pins. */
        R_IOPORT_Open(&IOPORT_CFG_CTRL, &IOPORT_CFG_NAME);
    }
}


void sci_i2c_master_callback(i2c_master_callback_args_t *p_args)
{
    if (NULL != p_args) {
        /* capture callback event for validating the i2c transfer event*/
        i2c_event = p_args->event;
        if (i2c_event == I2C_MASTER_EVENT_TX_COMPLETE) {
            i2cBusy = false;
        }
    }
}


void spi_callback(spi_callback_args_t *p_args)
{
    if (NULL != p_args) {
        transferNotComplete = 0;
    }
}


