 /******************************************************************************
 *
 * Module: Buzzer
 *
 * File Name: buzzer.c
 *
 * Description: source file for the buzzer driver
 *
 * Author: Youssef Ahmed
 *
 *******************************************************************************/

#include "buzzer.h"
#include "gpio.h"

/*
 * Description :
 * Initialize buzzer by setup the direction of buzzer pins as output by send the request to GPIO driver.
 */
void BUZZER_init(void)
{
	GPIO_setupPinDirection(BUZZER_PORT_ID, BUZZER_PIN_ID, PIN_OUTPUT);
}

/*
 * Description :
 * Turn buzzer ON
 */
void BUZZER_on(void)
{
	GPIO_writePin(BUZZER_PORT_ID, BUZZER_PIN_ID, LOGIC_HIGH);
}

/*
 * Description :
 * Turn buzzer OFF
 */
void BUZZER_off(void)
{
	GPIO_writePin(BUZZER_PORT_ID, BUZZER_PIN_ID, LOGIC_LOW);
}
