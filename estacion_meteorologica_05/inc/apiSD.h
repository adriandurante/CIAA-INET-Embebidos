/*
 * apiSD.h
 *
 *  Created on: Oct 25, 2017
 *      Author: seb
 */

#ifndef APISD_H_
#define APISD_H_

#include "sapi.h"
#include "fatfs_stubs.h"
#include "ff.h"

/* estos son los estados de salida */
#define _API_STATE_OK			(0)
#define _API_STATE_ERROR		(1)

/************* FAT **************/
bool_t 		diskTickHook	( void *ptr );
/***************************/

/************* SD **************/
uint8_t 	apiSD_Init		( void );
uint8_t 	apiSD_Write		( uint8_t * strDatalogFilename, uint8_t * stringData, uint32_t * size );

/***************************/


#endif /* APISD_H_ */
