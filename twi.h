 /******************************************************************************
 *
 * Module: TWI(I2C)
 *
 * File Name: twi.h
 *
 * Description: Header file for the TWI(I2C) AVR driver
 *
 * Author: Mohamed Tarek
 *
 *******************************************************************************/ 

#ifndef TWI_H_
#define TWI_H_

#include "std_types.h"

/*******************************************************************************
 *                      Preprocessor Macros                                    *
 *******************************************************************************/

/* I2C Status Bits in the TWSR Register */
#define TWI_START         0x08 /* start has been sent */
#define TWI_REP_START     0x10 /* repeated start */
#define TWI_MT_SLA_W_ACK  0x18 /* Master transmit ( slave address + Write request ) to slave + ACK received from slave. */
#define TWI_MT_SLA_R_ACK  0x40 /* Master transmit ( slave address + Read request ) to slave + ACK received from slave. */
#define TWI_MT_DATA_ACK   0x28 /* Master transmit data and ACK has been received from Slave. */
#define TWI_MR_DATA_ACK   0x50 /* Master received data and send ACK to slave. */
#define TWI_MR_DATA_NACK  0x58 /* Master received data but doesn't send ACK to slave. */

/*******************************************************************************
 *                         Types Declaration                                   *
 *******************************************************************************/

typedef enum
{
	/* value to be stored in TWBR register according to the required bit rate */
	NORMAL_MODE_100=0x20,FAST_MODE_400=0x02
}Bit_Rate;

typedef struct
{
	Bit_Rate rate;
	uint8 address;
}TWI_ConfigType;

/*******************************************************************************
 *                      Functions Prototypes                                   *
 *******************************************************************************/

/*
 * Description :
 * initialize TWI according to the configuration structure
 */
void TWI_init(const TWI_ConfigType * Config_Ptr);

/*
 * Description :
 * sending start bit
 */
void TWI_start(void);

/*
 * Description :
 * sending stop bit
 */
void TWI_stop(void);

/*
 * Description :
 * sending data byte
 */
void TWI_writeByte(uint8 data);

/*
 * Description :
 * receiving data byte with acknowledgment
 */
uint8 TWI_readByteWithACK(void);

/*
 * Description :
 * receiving data byte with negative acknowledgment
 */
uint8 TWI_readByteWithNACK(void);

/*
 * Description :
 * getting TWI status register
 */
uint8 TWI_getStatus(void);


#endif /* TWI_H_ */
