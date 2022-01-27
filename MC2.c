/*
 ================================================================================================
 Name        : MC2.c
 Author      : Youssef Ahmed
 Description : MC2 Code in Final Project
 Date        : 3/11/2021
 ================================================================================================
 */

#define F_CPU 8000000UL
#define ENTER_BUTTON 13
#define PASSWORD_LENGTH 5
#define MY_ADDRESS 0x01
#define EEPROM_LOCATION 0x0311
#define ISRs_FOR_15SEC 458
#define ISRs_FOR_3SEC 92
#define ISRs_FOR_1MIN 1831
#define MC_READY 0x01
#define ACK 0x02

#include "uart.h"
#include "twi.h"
#include "external_eeprom.h"
#include "dc_motor.h"
#include "timer0.h"
#include "buzzer.h"
#include "std_types.h"
#include <util/delay.h>
#include "avr/io.h"

/* defining general counter */
static uint8 counter=0;
volatile static uint16 ISR_counter=0;

/* call back function in application */
static void call_back(void)
{
	/* increasing variable indicating how many times the ISR is executed */
	ISR_counter++;
}

/* function to receive password from MC1 using UART byte by byte */
void receive_pass(uint8 password[PASSWORD_LENGTH])
{
	for(counter=0;counter<PASSWORD_LENGTH;counter++)
		password[counter]= UART_recieveByte();
}

/* function to write password in EEPROM byte by byte */
void store_password(uint8 password[PASSWORD_LENGTH])
{
	for(counter=0;counter<PASSWORD_LENGTH;counter++)
	{
		EEPROM_writeByte(EEPROM_LOCATION + counter, password[counter] );
		/* EEPROM time response */
		_delay_ms(10);
	}
}


int main(void)
{
	uint8 password[PASSWORD_LENGTH]={0};
	uint8 byte_from_EEPROM=0;

	/* Create configuration structure for UART driver */
	UART_ConfigType UART_Config = {DISABLED,EIGHT_BIT,ONE_BIT,9600};
	/* Initialize the UART driver with Baud-rate = 9600 bits/sec */
	UART_init(&UART_Config);

	/* Create configuration structure for TWI driver */
	TWI_ConfigType TWI_Config = {FAST_MODE_400,MY_ADDRESS};
	/* initializing TWI driver */
	TWI_init(&TWI_Config);
	/* enable global interrupts */
	SREG  |= (1<<7);
	/* Create configuration structure for timer0 driver */
	Timer0_ConfigType Timer_Config = {NORMAL_MODE,F_CPU_1024,0,0};
	/* Set the Call back function pointer in timer0 driver */
	Timer0_setCallBack(call_back);

	while(UART_recieveByte() != MC_READY){}
	UART_sendByte(ACK);
	/* receive password from MC1 and store it in EEPROM */
	receive_pass(password);
	store_password(password);

	while(1)
	{
		/* flag to be set when passwords are not matched */
		uint8 flag=0;
		/* counter to be increased when passwords are not matched */
		uint8 wrong_pass=0;

		uint8 wrong_length=0,key;

		while(UART_recieveByte() != MC_READY){}
		UART_sendByte(ACK);
		/* receive the required option from MC1 */
		key=UART_recieveByte();
		do{
			while(UART_recieveByte() != MC_READY){}
			UART_sendByte(ACK);
			wrong_length=UART_recieveByte();
			if(wrong_length==1)
			{
				wrong_pass++;
				continue;
			}
			receive_pass(password);
			counter=0;
			/* compare received password with the stored one in EEPROM */
			while(counter<PASSWORD_LENGTH)
			{
				EEPROM_readByte(EEPROM_LOCATION + counter, &(byte_from_EEPROM));
				if( byte_from_EEPROM==password[counter])
				{
					counter++;
					flag=0;
				}
				else
				{
					/* passwords are not matched (flag=1)  */
					flag=1;
					break;
				}
			}
			UART_sendByte(MC_READY);
			while(UART_recieveByte() != ACK){}
			UART_sendByte(flag);
			wrong_pass=UART_recieveByte();
		}
		/* loop until entering the right password or entering wrong password for 3 times */
		while(((flag==1)||(wrong_length==1))&&(wrong_pass<3));

		/* turn buzzer on when entering wrong password for 3 times */
		if(wrong_pass>=3)
		{
			/* initializing buzzer */
			BUZZER_init();
			ISR_counter=0;
			Timer0_Init(&Timer_Config);
			BUZZER_on();
			/* turn buzzer on for 1 minute */
			while(ISR_counter<=ISRs_FOR_1MIN){}
			/* turn buzzer off */
			BUZZER_off();
			/* disable timer0 */
			Timer0_Deinit();
		}

		if ((key=='+') && (wrong_pass<3))
		{
			/* initializing dc motor */
			DcMotor_Init();
			ISR_counter=0;
			/* initializing timer0 */
			Timer0_Init(&Timer_Config);
			/* rotate dc motor clockwise for 15 seconds */
			DcMotor_Rotate(DC_MOTOR_CW);
			while(ISR_counter<=ISRs_FOR_15SEC){}
			ISR_counter=0;
			TCNT0 = 0;
			/* stop dc motor for 3 seconds */
			DcMotor_Rotate(DC_MOTOR_STOP);
			while(ISR_counter<=ISRs_FOR_3SEC){}
			ISR_counter=0;
			TCNT0 = 0;
			/* rotate dc motor anti-clockwise for 15 seconds */
			DcMotor_Rotate(DC_MOTOR_ACW);
			while(ISR_counter<=ISRs_FOR_15SEC){}
			DcMotor_Rotate(DC_MOTOR_STOP);
			/* disable timer0 */
			Timer0_Deinit();

		}
		if ((key=='-') && (wrong_pass<3))
		{
			while(UART_recieveByte() != MC_READY){}
			UART_sendByte(ACK);
			/* receive new password from MC1 and store it in EEPROM */
			receive_pass(password);
			store_password(password);
		}
	}
}
