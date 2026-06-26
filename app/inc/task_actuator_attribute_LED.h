/*
 * Copyright (c) 2023 Juan Manuel Cruz <jcruz@fi.uba.ar> <jcruz@frba.utn.edu.ar>.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its
 *    contributors may be used to endorse or promote products derived from
 *    this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 * COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
 * IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 * @file   : task_actuator_attribute.h
 * @date   : Set 26, 2023
 * @author : Juan Manuel Cruz <jcruz@fi.uba.ar> <jcruz@frba.utn.edu.ar>
 * @version	v1.0.0
 */

#ifndef TASK_INC_TASK_ACTUATOR_ATTRIBUTE_H_
#define TASK_INC_TASK_ACTUATOR_ATTRIBUTE_H_

/********************** CPP guard ********************************************/
#ifdef __cplusplus
extern "C" {
#endif

/********************** inclusions *******************************************/

/********************** macros ***********************************************/

/********************** typedef **********************************************/
/* Actuator LED RGB Statechart - State Transition Table */
/* ------------------------+-----------------------+-----------------------+-----------------------+------------------------
 * | Current               | Event                 |                       | Next                  |                       |
 * | State                 | (Parameters)          | [Guard]               | State                 | Actions               |
 * |=======================+=======================+=======================+=======================+=======================|
 * | INICIAL (Static)      |                       |                       | ST_LED_OFF            | intensity = 0         |
 * |                       |                       |                       |                       | flag = false          |
 * |-----------------------+-----------------------+-----------------------+-----------------------+-----------------------|
 * | ST_LED_OFF            | EV_LED_RED_ON         | [flag == true]        | ST_LED_RED_ON         | flag = false          |
 * |                       | EV_LED_BLUE_ON        | [flag == true]        | ST_LED_BLUE_ON        | flag = false          |
 * |                       | EV_LED_PURPLE_ON      | [flag == true]        | ST_LED_PURPLE_ON      | flag = false          |
 * |                       | EV_LED_WHITE_ON       | [flag == true]        | ST_LED_WHITE_ON       | flag = false          |
 * |                       +-----------------------+-----------------------+-----------------------+-----------------------|
 * |                       | ANY OTHER / ELSE      |                       | ST_LED_OFF            | update_led_hw(0,0,0)  |
 * |-----------------------+-----------------------+-----------------------+-----------------------+-----------------------|
 * | ST_LED_RED_ON         | EV_LED_OFF            | [flag == true]        | ST_LED_OFF            | flag = false          |
 * |                       | EV_LED_BLUE_ON        | [flag == true]        | ST_LED_BLUE_ON        | flag = false          |
 * |                       | EV_LED_PURPLE_ON      | [flag == true]        | ST_LED_PURPLE_ON      | flag = false          |
 * |                       | EV_LED_WHITE_ON       | [flag == true]        | ST_LED_WHITE_ON       | flag = false          |
 * |                       +-----------------------+-----------------------+-----------------------+-----------------------|
 * |                       | ELSE                  |                       | ST_LED_RED_ON         | update_led_hw(I,0,0)  |
 * |-----------------------+-----------------------+-----------------------+-----------------------+-----------------------|
 * | ST_LED_BLUE_ON        | EV_LED_OFF            | [flag == true]        | ST_LED_OFF            | flag = false          |
 * |                       | EV_LED_RED_ON         | [flag == true]        | ST_LED_RED_ON         | flag = false          |
 * |                       | EV_LED_PURPLE_ON      | [flag == true]        | ST_LED_PURPLE_ON      | flag = false          |
 * |                       | EV_LED_WHITE_ON       | [flag == true]        | ST_LED_WHITE_ON       | flag = false          |
 * |                       +-----------------------+-----------------------+-----------------------+-----------------------|
 * |                       | ELSE                  |                       | ST_LED_BLUE_ON        | update_led_hw(0,0,I)  |
 * |-----------------------+-----------------------+-----------------------+-----------------------+-----------------------|
 * | ST_LED_PURPLE_ON      | EV_LED_OFF            | [flag == true]        | ST_LED_OFF            | flag = false          |
 * |                       | EV_LED_RED_ON         | [flag == true]        | ST_LED_RED_ON         | flag = false          |
 * |                       | EV_LED_BLUE_ON        | [flag == true]        | ST_LED_BLUE_ON        | flag = false          |
 * |                       | EV_LED_WHITE_ON       | [flag == true]        | ST_LED_WHITE_ON       | flag = false          |
 * |                       +-----------------------+-----------------------+-----------------------+-----------------------|
 * |                       | ELSE                  |                       | ST_LED_PURPLE_ON      | update_led_hw(I,0,I)  |
 * |-----------------------+-----------------------+-----------------------+-----------------------+-----------------------|
 * | ST_LED_WHITE_ON       | EV_LED_OFF            | [flag == true]        | ST_LED_OFF            | flag = false          |
 * |                       | EV_LED_RED_ON         | [flag == true]        | ST_LED_RED_ON         | flag = false          |
 * |                       | EV_LED_BLUE_ON        | [flag == true]        | ST_LED_BLUE_ON        | flag = false          |
 * |                       | EV_LED_PURPLE_ON      | [flag == true]        | ST_LED_PURPLE_ON      | flag = false          |
 * |                       +-----------------------+-----------------------+-----------------------+-----------------------|
 * |                       | ELSE                  |                       | ST_LED_WHITE_ON       | update_led_hw(I,I,I)  |
 * ------------------------+-----------------------+-----------------------+-----------------------+------------------------ */

/* Events to excite Task Actuator */
typedef enum task_actuator_LED_ev {
							   EV_LED_RED_ON,
							   EV_LED_BLUE_ON,
							   EV_LED_PURPLE_ON,
							   EV_LED_WHITE_ON,
							   EV_LED_OFF
							   } task_actuator_LED_ev_t;

/* States of Task Actuator */
typedef enum task_actuator_LED_st {ST_LED_OFF,
							   ST_LED_RED_ON,
							   ST_LED_BLUE_ON,
							   ST_LED_PURPLE_ON,
							   ST_LED_WHITE_ON
							   } task_actuator_LED_st_t;

/* Identifier of Task Actuator */
typedef enum task_actuator_LED_id {ID_LED_1} task_actuator_LED_id_t;

typedef struct
{
	task_actuator_LED_id_t	identifier;
	TIM_HandleTypeDef* htim_r;
	TIM_HandleTypeDef* htim_g;
	TIM_HandleTypeDef* htim_b;
	uint32_t            channel_r;
	uint32_t            channel_g;
	uint32_t            channel_b;

} task_actuator_LED_cfg_t;

typedef struct
{
	task_actuator_LED_st_t	state;
	task_actuator_LED_ev_t	event;
	bool				flag;
	uint8_t			intensity;
} task_actuator_LED_dta_t;

/********************** external data declaration ****************************/
extern task_actuator_LED_dta_t task_actuator_LED_dta;

/********************** external functions declaration ***********************/

/********************** End of CPP guard *************************************/
#ifdef __cplusplus
}
#endif

#endif /* TASK_INC_TASK_ACTUATOR_ATTRIBUTE_H_ */

/********************** end of file ******************************************/
