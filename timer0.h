 /******************************************************************************
 *
 * Module: Timer0
 *
 * File Name: timer0.h
 *
 * Description: header file for timer0 driver
 *
 * Author: Youssef Ahmed
 *
 *******************************************************************************/

#ifndef TIMER0_H_
#define TIMER0_H_

#include "std_types.h"

/*******************************************************************************
 *                         Types Declaration                                   *
 *******************************************************************************/
typedef enum
{
	NORMAL_MODE,CTC_MODE
}Timer_Mode;

typedef enum
{
	NO_CLOCK,F_CPU_CLOCK,F_CPU_8,F_CPU_64,F_CPU_256,F_CPU_1024,EXTERNAL_T0_FALLING,EXTERNAL_T0_RISING
}Clock_Select;

typedef struct
{
	Timer_Mode mode;
	Clock_Select clock;
	uint8 initial_value;
	uint8 compare_value;
}Timer0_ConfigType;

/*******************************************************************************
 *                              Functions Prototypes                           *
 *******************************************************************************/

/*
 * Description :
 * Initialize timer0 by getting pointer to configuration structure defining:
 * 1- Timer mode: Normal (over flow) or compare mode
 * 2- clock selection
 * 3- timer0 initial value
 * 4- compare value in case of compare mode
 */
void Timer0_Init(const Timer0_ConfigType * Config_Ptr);

/*
 * Description: Function to set the Call Back function address.
 */
void Timer0_setCallBack(void(*a_ptr)(void));

/*
 * Description: Function to disable the Timer0
 */
void Timer0_Deinit(void);

#endif /* TIMER0_H_ */
