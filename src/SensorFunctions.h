/*
 * SensorFunctions.h
 *
 *  Created on: Feb 6, 2026
 *      Author: VincentD'Agostino
 */

#ifndef SENSORFUNCTIONS_H_
#define SENSORFUNCTIONS_H_

#include <stdint.h>
#include "Project.h"


//--------------------------------
// Macro Definitions
#define SENSOR_HISTORY_LENGTH 8




//--------------------------------
// Structures, enums, unions, typdefs
typedef union Sensor {
    uint8_t byteAccess[(4 * SENSOR_HISTORY_LENGTH) + 4 + 2 + 1 + 1];
    struct SensorData {
        int32_t rawData[SENSOR_HISTORY_LENGTH];
        int32_t total;
        int16_t average;
        uint8_t nextValue;
        uint8_t lastValue;
    } data;
} sensor_data_t;

typedef union Sensor_int32 {
    uint8_t byteAccess[(4 * SENSOR_HISTORY_LENGTH) + 4 + 4 + 1 + 1];
    struct SensorData_int32 {
        int32_t rawData[SENSOR_HISTORY_LENGTH];
        int32_t total;
        int32_t average;
        uint8_t nextValue;
        uint8_t lastValue;
    } data;
} sensor_data_int32_t;

typedef union se_Sensor {
    uint8_t byteAccess[(4 * SENSOR_HISTORY_LENGTH) + 4 + 2 + 1 + 1];
    struct se_SensorData {
        uint32_t rawData[SENSOR_HISTORY_LENGTH];
        uint32_t total;
        uint16_t average;
        uint8_t nextValue;
        uint8_t lastValue;
    } data;
} se_sensor_data_t;

typedef struct sensor_obj {
    sensor_data_t * pSensorData;
    uint16_t sineOffset;
} sensor_obj_t;


//-------------------------------
// Public Variables




//--------------------------------
// Public Prototypes
uint8_t InitSensor(sensor_data_t * _sensor_to_init, const sensor_data_t * _init_data);
uint8_t InitSensorInt32(sensor_data_int32_t * _sensor_to_init, const sensor_data_int32_t * _init_data);
uint8_t InitSeSensor(se_sensor_data_t * _sensor_to_init, const se_sensor_data_t * _init_data);
uint8_t ParseParamsToUINT16(uint8_t * _param_string, uint16_t *_param_array, uint8_t _no_of_params);
uint8_t AddSensorUint16Value(se_sensor_data_t * _raw_sensor_data, uint16_t _new_data);
uint8_t AddSensorInt16Value(sensor_data_t * _raw_sensor_data, int16_t _new_data);
uint8_t AddSensorInt32Value(sensor_data_int32_t * _raw_sensor_data, int32_t _new_data);
void ProcessSensorDataSim(sensor_obj_t * _sensor);




#endif /* SENSORSIMULATOR_H_ */
