/*
 * api.h
 *
 *  Created on: Oct 25, 2017
 *      Author: seb
 */

#ifndef API_H_
#define API_H_

#include "sapi.h"
#include "apiSD.h"
#define NUMBER_OF_SENSOR    (3)

typedef struct {
	uint8_t  name [20];
	uint8_t  id;
	bool_t   enable;
} sensorSetup;

typedef struct sensorData {
	uint8_t  name [20];
	uint8_t  id;
	uint16_t value;
} sensorData;


uint16_t samplingTime;

uint8_t apiReadTemperatureHumdity	(uint16_t * dataTemp, uint16_t * dataHum);
uint8_t apiReadWind					(uint16_t * dataWind);
uint8_t apiReadSensor				(uint16_t * dataTemp, uint16_t * dataHum, uint16_t * dataWind);
uint8_t apiProcessInformation		(uint16_t dataTemp, uint16_t dataHum, uint16_t dataWind, uint8_t * destiny);
uint8_t apiWriteSD					(uint8_t * filename, uint8_t * stringData);

void BorrarConsola					( void );
void RutinaBienvenida				( void );
uint8_t apiSensorNameSetup			( void );
uint8_t apiSensorStateInitilize		( void );
uint8_t apiSensorSetup 				( void );
uint8_t apiSamplingTimeSetup		( void );
uint8_t uartReadString				( uartMap_t uart, uint8_t* receivedString, uint8_t lengthString );
uint8_t uartReadInt					( uartMap_t uart, uint8_t* receivedNumber );
uint8_t apiSensorEnable				( void );

#endif /* API_H_ */