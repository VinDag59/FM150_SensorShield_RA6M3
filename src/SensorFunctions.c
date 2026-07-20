/*
 * SensorSimulator.c
 *
 *  Created on: Feb 6, 2026
 *      Author: VincentD'Agostino
 */


#include "Project.h"
#include "SensorFunctions.h"
#include "ASCII_numbers.h"



uint8_t InitSensor(sensor_data_t * _sensor_to_init, const sensor_data_t * _init_data)
{
    uint8_t i;
    uint8_t returnStatus = 0; // all good

    for (i=0; i < sizeof(sensor_data_t); i++) {
        _sensor_to_init->byteAccess[i] = _init_data->byteAccess[i];
    }
    return returnStatus;
}


uint8_t InitSensorInt32(sensor_data_int32_t * _sensor_to_init, const sensor_data_int32_t * _init_data)
{
    uint8_t i;
    uint8_t returnStatus = 0; // all good

    for (i=0; i < sizeof(sensor_data_t); i++) {
        _sensor_to_init->byteAccess[i] = _init_data->byteAccess[i];
    }
    return returnStatus;
}


uint8_t InitSeSensor(se_sensor_data_t * _sensor_to_init, const se_sensor_data_t * _init_data)
{
    uint8_t i;
    uint8_t returnStatus = 0; // all good

    for (i=0; i < sizeof(sensor_data_t); i++) {
        _sensor_to_init->byteAccess[i] = _init_data->byteAccess[i];
    }
    return returnStatus;
}



uint8_t ParseParamsToUINT16(uint8_t * _param_string, uint16_t *_param_array, uint8_t _no_of_params)
{
    uint8_t i = 0;
    uint8_t returnStatus = 0;
    uint8_t paramStart = 0;
    uint8_t paramEnd = 0;

    while (_param_string[paramEnd] != '\n') {
        while ( (_param_string[paramEnd] != ':') && (_param_string[paramEnd] != '\n') ) {
            paramEnd++;
        }

        ConvertASCII2UINT16((const char *)&_param_string[paramStart], (paramEnd - paramStart), _param_string[paramEnd], &_param_array[i]);

        if (_param_string[paramEnd] != '\n')
            paramEnd++;
        paramStart = paramEnd;
        i++;
    }

    if (i != _no_of_params)
        returnStatus = 1;

    return returnStatus;
}


// note that the max value that can be entered is 32767 b/c it is converted to int16 for the data array
uint8_t AddSensorUint16Value(se_sensor_data_t * _raw_sensor_data, uint16_t _new_data)
{
    uint8_t returnStatus = 0;

    _raw_sensor_data->data.total -= _raw_sensor_data->data.rawData[_raw_sensor_data->data.nextValue];
    _raw_sensor_data->data.rawData[_raw_sensor_data->data.nextValue] = _new_data;
    _raw_sensor_data->data.total += _raw_sensor_data->data.rawData[_raw_sensor_data->data.nextValue];

    _raw_sensor_data->data.average = (uint16_t)_raw_sensor_data->data.total / SENSOR_HISTORY_LENGTH;

    _raw_sensor_data->data.lastValue = _raw_sensor_data->data.nextValue;
    _raw_sensor_data->data.nextValue++;
    if(_raw_sensor_data->data.nextValue >= SENSOR_HISTORY_LENGTH) _raw_sensor_data->data.nextValue = 0;

    return returnStatus;
}


uint8_t AddSensorInt16Value(sensor_data_t * _raw_sensor_data, int16_t _new_data)
{
    uint8_t returnStatus = 0;

    _raw_sensor_data->data.total -= _raw_sensor_data->data.rawData[_raw_sensor_data->data.nextValue];
    _raw_sensor_data->data.rawData[_raw_sensor_data->data.nextValue] = _new_data;
    _raw_sensor_data->data.total += _raw_sensor_data->data.rawData[_raw_sensor_data->data.nextValue];

    _raw_sensor_data->data.average = (int16_t)_raw_sensor_data->data.total / SENSOR_HISTORY_LENGTH;

    _raw_sensor_data->data.lastValue = _raw_sensor_data->data.nextValue;
    _raw_sensor_data->data.nextValue++;
    if(_raw_sensor_data->data.nextValue >= SENSOR_HISTORY_LENGTH) _raw_sensor_data->data.nextValue = 0;

    return returnStatus;
}

uint8_t AddSensorInt32Value(sensor_data_int32_t * _raw_sensor_data, int32_t _new_data)
{
    uint8_t returnStatus = 0;

    _raw_sensor_data->data.total -= _raw_sensor_data->data.rawData[_raw_sensor_data->data.nextValue];
    _raw_sensor_data->data.rawData[_raw_sensor_data->data.nextValue] = _new_data;
    _raw_sensor_data->data.total += _raw_sensor_data->data.rawData[_raw_sensor_data->data.nextValue];

    _raw_sensor_data->data.average = _raw_sensor_data->data.total / SENSOR_HISTORY_LENGTH;

    _raw_sensor_data->data.lastValue = _raw_sensor_data->data.nextValue;
    _raw_sensor_data->data.nextValue++;
    if(_raw_sensor_data->data.nextValue >= SENSOR_HISTORY_LENGTH) _raw_sensor_data->data.nextValue = 0;

    return returnStatus;

}
//void ProcessSensorDataSim(sensor_obj_t * _sensor)
//{
//    AddSensorInt16Value(_sensor->pSensorData, (sine256[_sensor->sineOffset] >> 4));
//
//    _sensor->sineOffset++;
//    if (_sensor->sineOffset >= SINE_LENGTH) _sensor->sineOffset = 0;
//
//}
