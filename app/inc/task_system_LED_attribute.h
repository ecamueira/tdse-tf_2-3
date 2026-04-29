
#ifndef TASK_INC_TASK_SYSTEM_LED_ATTRIBUTE_H_
#define TASK_INC_TASK_SYSTEM_LED_ATTRIBUTE_H_

/********************** CPP guard ********************************************/
#ifdef __cplusplus
extern "C" {
#endif

/********************** inclusions *******************************************/
#include <stdint.h>
#include <stdbool.h>
/********************** macros ***********************************************/

/********************** typedef **********************************************/
/* System LED Statechart - State Transition Table */
/* ------------------------+-----------------------+-----------------------+-----------------------+------------------------
 * | Current               | Event                 |                       | Next                  |                       |
 * | State                 | (Parameters)          | [Guard]               | State                 | Actions               |
 * |=======================+=======================+=======================+=======================+=======================|
 * | ST_SYS_LED_IDLE       | flag == true          |                       | ST_SYS_LED_NEW_DATA   | flag = false          |
 * |                       |                       |                       |                       |                       |
 * |-----------------------+-----------------------+-----------------------+-----------------------+-----------------------|
 * | ST_SYS_LED_NEW_DATA   | (Immediate)           | [color == OFF]        | ST_SYS_LED_IDLE       | put_act_LED(EV_OFF)   |
 * |                       |                       | [color == BLANCO]     | ST_SYS_LED_IDLE       | put_act_LED(EV_WHITE) |
 * |                       |                       | [color == ROJO]       | ST_SYS_LED_IDLE       | put_act_LED(EV_RED)   |
 * |                       |                       | [color == AZUL]       | ST_SYS_LED_IDLE       | put_act_LED(EV_BLUE)  |
 * |                       |                       | [color == VIOLETA]    | ST_SYS_LED_IDLE       | put_act_LED(EV_PURPLE)|
 * |-----------------------+-----------------------+-----------------------+-----------------------+-----------------------|
 * | DEFAULT               |                       |                       | ST_SYS_LED_IDLE       | flag = false          |
 * ------------------------+-----------------------+-----------------------+-----------------------+------------------------ */

/* State of Task System */
typedef enum task_system_LED_st {ST_SYS_LED_IDLE,
								ST_SYS_LED_NEW_DATA} task_system_LED_st_t;

typedef struct
{
	task_system_LED_st_t	state;
	uint8_t							color;
	uint8_t							intensidad;
	bool				flag;
} task_system_LED_dta_t;

/********************** external data declaration ****************************/
extern task_system_LED_dta_t task_system_LED_dta;

/********************** external functions declaration ***********************/

/********************** End of CPP guard *************************************/
#ifdef __cplusplus
}
#endif

#endif /* TASK_INC_TASK_SYSTEM_LEVEL_TANQUE_ATTRIBUTE_H_ */

/********************** end of file ******************************************/
