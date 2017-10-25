/* Copyright 2016, Eric Pernia.
 * All rights reserved.
 *
 * This file is part sAPI library for microcontrollers.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its
 *    contributors may be used to endorse or promote products derived from this
 *    software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 */

/*
 * Date: 2016-04-26
 */

/*==================[inclusions]=============================================*/

#include "main.h"   	// <= own header (optional)
#include "sapi.h"       // <= sAPI header

//#include "ff.h"       		// <= Biblioteca FAT FS
#include "fatfs_stubs.h"	// En este archivo se setea si sacar datos por SD o UART

/*==================[macros and definitions]=================================*/
#define LONGITUD_DATOS_RTC   (19)
#define FILENAME "hola.txt"
/*==================[internal data declaration]==============================*/

// Buffer donde se va a guardar la informacion de la aplicacion.

static FATFS FileSystem;  	// <-- FatFs work area needed for each volume
static FIL   File;          // <-- File object needed for each open file

/*==================[internal functions declaration]=========================*/

void	formatInfoAlmacenar ( uint8_t *cadenaDestino, uint8_t *cadenaOrigen, uint8_t longitudMax);
void 	formatInfoADC 		( uint16_t valorADC, uint8_t *datosADC );
void    formatInfoRTC 		( rtc_t *rtc, uint8_t *infoToSave );

/*==================[internal data definition]===============================*/


/*==================[external data definition]===============================*/

/*==================[internal functions definition]==========================*/


/*==================[external functions definition]==========================*/

/* FUNCION PRINCIPAL, PUNTO DE ENTRADA AL PROGRAMA LUEGO DE RESET. */
int main(void){
	rtc_t rtc;
	uint8_t datosRTC [LONGITUD_DATOS_RTC];
	uint8_t datosADC [5];
	uint8_t datosAlmacenar [100];

	// Variable para almacenar el valor leido del ADC CH1
	uint16_t adcValue = 0;
	uint32_t bytesWritten;
	delay_t delay1s;


    // Inicializar la placa
    boardConfig();
    // Inicializar UART_USB a 115200 baudios
    uartConfig( UART_USB, 115200 );
    // Habilita el ADC
    adcConfig( ADC_ENABLE );
    // Configura el puerto SPI0
   	spiConfig( SPI0 );


	// Setea hora y fecha
	rtc.year  = 2017;
	rtc.month = 10;
	rtc.mday  = 24;
	rtc.wday  = 1;
	rtc.hour  = 14;
	rtc.min   = 50;
	rtc.sec   = 0;
	// Inicia el rtc con rtcConfig(&rtc);
	rtcConfig( &rtc );



    // Habilita la interrupcion del tick cada 10 ms y cuando ocurre llama a diskTickHook()
   	tickConfig( 10, NULL );
   	delayConfig ( &delay1s, 1000 );


   	// Monta el sistema de archivos
   	if( f_mount_( &FileSystem, "", 0 ) != FR_OK ){
   		// Se podria poner un mensaje de error si no lo puede montar.
   	}


   	while(1) {

	   if( delayRead( &delay1s ) ){
	   		// Actualiza el clock a la nueva hora.
	   		rtcRead( &rtc );
	   		adcValue = adcRead( CH1 );					// Lee el valor del ADC CH1

	   		// Formatea el array InformationArray con el valor leido del potenciometro.
	   		formatInfoADC (adcValue, datosADC );

	   		// Saca por consola el texto "ADC CH1 value: xxxx"
	   		//  uartWriteString( UART_USB, "ADC CH1 value: " );
	   		// uartWriteString( UART_USB, InformationArray );


	   		// Formatea la salida y la guarda en el arreglo global informationToSave.
		   	// Formato: DD/MM/YYYY_HH:MM:SS_XXX;YYY;ZZZ;
		   	formatInfoRTC( &rtc, datosRTC );
		   	// Escribe por UART el array formateado con el timestamp
		   	//	uartWriteString(UART_USB, infoToSave);

		   	formatInfoAlmacenar (datosAlmacenar, datosADC, 100);
		   	formatInfoAlmacenar (datosAlmacenar, datosRTC, 100);
		   	datosAlmacenar[24] = '\n';
		   	datosAlmacenar[25] = '\r';
		   	datosAlmacenar[26] = '\0';

		   	// FA_CREATE_ALWAYS se sobreescribe el archivo. FA_OPEN_APPEND el nuevo contenido se escribe al final.
		   	if( f_open_( &File, FILENAME, FA_WRITE | FA_OPEN_APPEND ) == FR_OK ){
		   		// Escribe en el archivo abierto el texto "Hola mundo\n\r".
		   		f_write_( &File, (char *) datosAlmacenar, 50, &bytesWritten );           //"Hola mundo\r\n"

				// Una vez que escribio cierra el archivo.
				f_close_(&File);
				// Chequea si los bytes escritos son iguales a los que se pidio escribir.
				if( bytesWritten == 50 ){
					// Si salio todo bien prende el LED verde
					gpioWrite( LEDG, ON );
					datosAlmacenar[0] = '\0';
				}
			}
		   	else {
				// Si la operacion fallo prende el LED rojo.
				gpioWrite( LEDR, ON );
			}
	   }
   }
   return 0 ;
}

/*==================[end of file]============================================*/


// Formatea el arreglo a guardar con DD/MM/YYYY_HH:MM:SS_XXX;YYY;ZZZ;
void formatInfoRTC ( rtc_t *rtc,  uint8_t *infoToSave ) {

	infoToSave[0]  = ( rtc->mday / 10) + '0';
    infoToSave[1]  = ( rtc->mday % 10) + '0';
    infoToSave[2]  = '/';
    infoToSave[3]  = ( rtc->month / 10) + '0';
    infoToSave[4]  = ( rtc->month % 10) + '0';
    infoToSave[5]  = '/';
    infoToSave[6]  = ( rtc->year / 1000) + '0';
    infoToSave[7]  = ( rtc->year % 1000 ) / 100 + '0';
    infoToSave[8]  = (( rtc->year % 100) / 10) + '0';
    infoToSave[9]  = ( rtc->year % 10) + '0';
    infoToSave[10] = '_';
    infoToSave[11] = ( rtc->hour / 10) + '0';
    infoToSave[12] = ( rtc->hour % 10) + '0';
    infoToSave[13] = ':';
    infoToSave[14] = ( rtc->min / 10) + '0';
    infoToSave[15] = ( rtc->min % 10) + '0';
    infoToSave[16] = ':';
    infoToSave[17] = ( rtc->sec / 10) + '0';
    infoToSave[18] = ( rtc->sec % 10) + '0';
    infoToSave[19] = '\0';
}


/**
 * Funcion que formatea el array InformationArray con el valor recibido como parametro.
 * Notar que a cada numero le agrega un + '0'. Esto es para que se conviera en un valor ASCII.
 * @param valorAdc valor leido por el ADC.
 */
void formatInfoADC ( uint16_t valorADC, uint8_t *datosADC) {
	datosADC[0] =  valorADC/1000 		+ '0';
	datosADC[1] = (valorADC%1000)/100 	+ '0';
	datosADC[2] = (valorADC%100)/10 	+ '0';
	datosADC[3] = (valorADC%10) 		+ '0';
	datosADC[4] = '\0';
}


void formatInfoAlmacenar ( uint8_t *cadenaDestino, uint8_t *cadenaOrigen, uint8_t longitudMax) {
	uint16_t i = 0, j=0;

	if ( cadenaDestino [i] != '\0') {
		while ( cadenaDestino[i] != '\0' )
			i++;
		cadenaDestino[i] = ';';
		//cadenaDestino[i+1] = ' ';
		i++;
	}

	while ( cadenaOrigen[j] != '\0' && i < longitudMax) {
		cadenaDestino[i] = cadenaOrigen[j];
		i++;
		j++;
	}
	cadenaDestino[i] = '\0';

}