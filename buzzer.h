 /******************************************************************************
 *
 * Module: BUZZER
 *
 * File Name: buzzer.h
 *
 * Description: header file for the Buzzer driver
 *
 * Author: Youssef Ahmed
 *
 *******************************************************************************/

#ifndef BUZZER_H_
#define BUZZER_H_

/*******************************************************************************
 *                                Definitions                                  *
 *******************************************************************************/
/* The Ports and Pins IDs for buzzer two pins */
#define BUZZER_PORT_ID                   PORTC_ID
#define BUZZER_PIN_ID         			  PIN7_ID


/*******************************************************************************
 *                              Functions Prototypes                           *
 *******************************************************************************/

/*
 * Description :
 * Initialize buzzer by setup the direction of buzzer pins as output by send the request to GPIO driver.
 */
void BUZZER_init(void);

/*
 * Description :
 * Turn buzzer ON
 */
void BUZZER_on(void);

/*
 * Description :
 * Turn buzzer OFF
 */
void BUZZER_off(void);

#endif /* BUZZER_H_ */
