/*
 ================================================================================================
 Name        : MC1.c
 Author      : Youssef Ahmed
 Description : MC1 Code in Final Project
 Date        : 3/11/2021
 ================================================================================================
 */

#define F_CPU 8000000UL
#define ENTER_BUTTON 13
#define PASSWORD_LENGTH 5
#define ISRs_FOR_15SEC 458
#define ISRs_FOR_3SEC 92
#define ISRs_FOR_1MIN 1831
#define MC_READY 0x01
#define ACK 0x02

#include "lcd.h"
#include "keypad.h"
#include "uart.h"
#include "timer0.h"
#include "std_types.h"
#include <util/delay.h> /* For the delay functions */
#include "avr/io.h"

/* defining general counter */
static uint8 counter=-1;

static uint8 wrong_length;

volatile static uint16 ISR_counter=0;

/* call back function in application */
static void call_back(void)
{
	/* increasing variable indicating how many times the ISR is executed */
	ISR_counter++;
}

/* function to get password from the user */
void get_pass(uint8 password[PASSWORD_LENGTH])
{
	wrong_length=0;
	for(counter=0;counter<PASSWORD_LENGTH;counter++)
	{
		password[counter]= KEYPAD_getPressedKey();
		_delay_ms(500);
		/* displaying '*' for each character of the password */
		if(password[counter]!=ENTER_BUTTON) LCD_displayCharacter('*');
		else {wrong_length=1;break;}
	}

	if (wrong_length==0)
	{
		uint8 test=KEYPAD_getPressedKey();
		_delay_ms(500);
		if(test!=ENTER_BUTTON){
			LCD_displayCharacter('*');
			wrong_length=1;
			/* get excess characters from the user */
			while((KEYPAD_getPressedKey())!=ENTER_BUTTON){_delay_ms(500);LCD_displayCharacter('*');}
			_delay_ms(500);
		}
	}
}

/* function to get password from user twice for the first time */
void enter_reenter_pass(uint8 password[PASSWORD_LENGTH],uint8 check[PASSWORD_LENGTH])
{
	do
	{
		LCD_clearScreen();
		LCD_displayString("enter 5character");
		LCD_moveCursor(1,0);
		LCD_displayString("password: ");
		LCD_moveCursor(1,9);
		get_pass(password);
	}
	while(wrong_length);
	LCD_clearScreen();
	LCD_moveCursor(0,0);
	LCD_displayString("re-enter pass:");
	LCD_moveCursor(1,0);
	get_pass(check);
}

/* function to confirm that the 2 entered passwords at the first time are the same */
void confirm_pass(uint8 password[PASSWORD_LENGTH],uint8 check[PASSWORD_LENGTH])
{
	/* defining flag that is set if the passwords are not matched */
	uint8 flag=0;
	do
	{
		if(wrong_length==1)
		{
			/* take the password again if not the same length */
			enter_reenter_pass(password,check);
		}
		counter=0;
		while(counter<PASSWORD_LENGTH)
		{
			if(password[counter]==check[counter])
			{
				counter++;
				flag=0;
			}
			else
			{
				flag=1;
				break;
			}
		}
		if (flag==1)
		{
			/* take the password again if not matched */
			enter_reenter_pass(password,check);
		}
	}
	/* repeat until the 2 passwords are matched */
	while((flag==1)||(wrong_length==1));
}

/* function to send password to MC2 using UART byte by byte */
void send_pass(uint8 password[PASSWORD_LENGTH])
{
	for(counter=0;counter<PASSWORD_LENGTH;counter++)
		UART_sendByte(password[counter]);
}


int main(void)
{
	/* defining 2 arrays to get password twice from the user at the first time */
	uint8 password[PASSWORD_LENGTH]={0};
	uint8 check[PASSWORD_LENGTH]={0};
	/* Initialize the LCD Driver */
	LCD_init();
	/* getting and confirming the password */
	enter_reenter_pass(password,check);
	confirm_pass(password,check);

	/* enable global interrupts */
	SREG  |= (1<<7);

	/* Create configuration structure for timer0 driver */
	Timer0_ConfigType Timer_Config = {NORMAL_MODE,F_CPU_1024,0,0};
	/* Set the Call back function pointer in timer0 driver */
	Timer0_setCallBack(call_back);

	/* Create configuration structure for UART driver */
	UART_ConfigType UART_Config = {DISABLED,EIGHT_BIT,ONE_BIT,9600};
	/* Initialize the UART driver */
	UART_init(&UART_Config);

	UART_sendByte(MC_READY);
	while(UART_recieveByte() != ACK){}

	/* sending confirmed password to be stored in EEPROM */
	send_pass(password);

	while(1)
	{
		/* show main menu */
		LCD_clearScreen();
		LCD_displayString("+ : open door");
		LCD_moveCursor(1,0);
		LCD_displayString("- : change pass");
		uint8 key;
		key= KEYPAD_getPressedKey();
		_delay_ms(500);
		volatile uint8 wrong_pass=0,flag=0;
		if ((key=='+')||(key=='-'))
		{
			UART_sendByte(MC_READY);
			while(UART_recieveByte() != ACK){}
			/* send the required option to MC2 */
			UART_sendByte(key);
			do
			{
				LCD_clearScreen();
				LCD_displayString("enter pass:");
				LCD_moveCursor(1,0);
				/* get password to either open the door or change password */
				get_pass(password);

				UART_sendByte(MC_READY);
				while(UART_recieveByte() != ACK){}
				UART_sendByte(wrong_length);

				if(wrong_length==1)
				{
					wrong_pass++;
					continue;
				}
				/* send password to MC2 to compare it with the stored password */
				send_pass(password);

				while(UART_recieveByte() != MC_READY){}
				UART_sendByte(ACK);
				/* get value of the flag (which is set when the entered password is not correct) from MC2 */
				flag=UART_recieveByte();
				wrong_pass+=flag;
				UART_sendByte(wrong_pass);
			}
			/* loop until entering the right password or entering wrong password for 3 times */
			while(((flag==1)||(wrong_length==1))&&(wrong_pass<3));
			if(wrong_pass>=3)
			{
				/* show error message when entering wrong password for 3 times */
				ISR_counter=0;
				LCD_clearScreen();
				/* initializing timer0 driver */
				Timer0_Init(&Timer_Config);
				LCD_displayString("wrong pass");
				/* show the error message for 1 minute */
				while(ISR_counter<=ISRs_FOR_1MIN){}
				/* disable timer0 */
				Timer0_Deinit();
			}
		}
		if ((key=='+') && (wrong_pass<3))
		{
			ISR_counter=0;
			LCD_clearScreen();
			/* initializing timer0 driver */
			Timer0_Init(&Timer_Config);

			/* show door state during opening and closing */
			while(ISR_counter<=ISRs_FOR_15SEC)
			{
				LCD_moveCursor(0,0);
				LCD_displayString("opening!!");
			}
			LCD_clearScreen();
			ISR_counter=0;
			TCNT0 = 0;
			while(ISR_counter<=ISRs_FOR_3SEC)
			{
				LCD_moveCursor(0,0);
				LCD_displayString("ENTER!!");
			}
			LCD_clearScreen();
			ISR_counter=0;
			TCNT0 = 0;
			while(ISR_counter<=ISRs_FOR_15SEC)
			{
				LCD_moveCursor(0,0);
				LCD_displayString("closing!!");
			}
			/* disable timer0 */
			Timer0_Deinit();
		}

		if ((key=='-') && (wrong_pass<3))
		{
			/* get new password and send it to MC2 to store it in EEPROM */
			enter_reenter_pass(password,check);
			confirm_pass(password,check);

			UART_sendByte(MC_READY);
			while(UART_recieveByte() != ACK){}
			send_pass(password);
		}
	}
}
