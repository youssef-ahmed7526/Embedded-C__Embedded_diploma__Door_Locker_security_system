 /******************************************************************************
 *
 * Module: Timer0
 *
 * File Name: timer0.c
 *
 * Description: Source file for the AVR timer0 driver
 *
 * Author: Youssef Ahmed
 *
 *******************************************************************************/

#include "common_macros.h"
#include <avr/io.h> /* To use timer0 Registers */
#include <avr/interrupt.h> /* For timer0 ISR */
#include "std_types.h"
#include "timer0.h"

/*******************************************************************************
 *                           Global Variables                                  *
 *******************************************************************************/

/* Global variables to hold the address of the call back function in the application */
static volatile void (*g_callBackPtr)(void) = NULL_PTR;

/*******************************************************************************
 *                       Interrupt Service Routines                            *
 *******************************************************************************/
ISR(TIMER0_OVF_vect)
{
	if(g_callBackPtr != NULL_PTR)
	{
		/* Call the Call Back function in the application */
		(*g_callBackPtr)();
	}
}

ISR(TIMER0_COMP_vect)
{
	if(g_callBackPtr != NULL_PTR)
	{
		/* Call the Call Back function in the application */
		(*g_callBackPtr)();
	}
}

/*******************************************************************************
 *                      Functions Definitions                                  *
 *******************************************************************************/
/*
 * Description : Function to initialize timer0 driver
 * 1- Set timer mode: Normal (over flow) or compare mode
 * 2- select required clock
 * 3- set timer0 initial value
 * 4- set compare value in case of compare mode
 */
void Timer0_Init(const Timer0_ConfigType * Config_Ptr)
{
	/* Set Timer initial value */
	TCNT0 = Config_Ptr->initial_value;
	/*
	 * Non PWM mode FOC0=1
	 * setting clock according to chosen pre-scaler in the configuration structure
	 */
	TCCR0 = (1<<FOC0) | (Config_Ptr->clock);

	if((Config_Ptr->mode)==NORMAL_MODE)
	{
		/*
		 * 1. Normal Mode WGM01=0 & WGM00=0
		 * 2. Normal Mode COM00=0 & COM01=0
		 */
		TCCR0 &= (~(1<<WGM01)) ;

		/* Enable Timer0 Overflow Interrupt */
		TIMSK |= (1<<TOIE0);
	}
	else if ((Config_Ptr->mode)==CTC_MODE)
	{
		/* Set Timer compare value */
		OCR0  = Config_Ptr->compare_value;
		/* Enable Timer0 Compare Interrupt */
		TIMSK |= (1<<OCIE0);
		/* Configure timer0 control register
		 * 1. CTC Mode WGM01=1 & WGM00=0
		 * 2. No need for OC0 so COM00=0 & COM01=0
		 */
		TCCR0 |= (1<<WGM01);
	}
}

void Timer0_setCallBack(void(*a_ptr)(void))
{
	/* Save the address of the Call back function in a global variable */
	g_callBackPtr = a_ptr;
}

void Timer0_Deinit()
{
	/* set clock bits CS12 CS11 CS10 to zero */
	TCCR0 &= 0xF8;
	TIMSK &= 0xFC;
}

