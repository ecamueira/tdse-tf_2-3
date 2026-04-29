
#ifndef TASK_INC_TASK_ACTUATOR_COOLER_ATTRIBUTE_H_
#define TASK_INC_TASK_ACTUATOR_COOLER_ATTRIBUTE_H_

/********************** CPP guard ********************************************/
#ifdef __cplusplus
extern "C" {
#endif

/********************** inclusions *******************************************/
#include <stdint.h>
#include <stdbool.h>
#include "main.h"
/********************** macros ***********************************************/

/********************** typedef **********************************************/
/* Actuator Cooler Statechart - State Transition Table */
/* ------------------------+-----------------------+-----------------------+-----------------------+------------------------
 * | Current               | Event                 |                       | Next                  |                       |
 * | State                 | (Parameters)          | [Guard]               | State                 | Actions               |
 * |=======================+=======================+=======================+=======================+=======================|
 * | INICIAL               |                       |                       | ST_COOLER_OFF         | tick = DEL_COOLER_MIN |
 * |                       |                       |                       |                       | flag_activar = False  |
 * |                       |                       |                       |                       | intensity = 0         |
 * |-----------------------+-----------------------+-----------------------+-----------------------+-----------------------|
 * | ST_COOLER_OFF         | flag_activar_cooler   |                       | ST_COOLER_ON          | update_cooler_hw()    |
 * |                       | == true               |                       |                       | (Uses intensity set   |
 * |                       | (intensity is updated |                       |                       |  by external task)    |
 * |                       |  externally)          |                       |                       |                       |
 * |-----------------------+-----------------------+-----------------------+-----------------------+-----------------------|
 * | ST_COOLER_ON          | flag_activar_cooler   |                       | ST_COOLER_OFF         | update_cooler_hw(0)   |
 * |                       | == false              |                       |                       |                       |
 * |                       +-----------------------+-----------------------+-----------------------+-----------------------|
 * |                       | ELSE                  |                       | ST_COOLER_ON          | update_cooler_hw()    |
 * |                       |                       |                       |                       | (Updates PWM with     |
 * |                       |                       |                       |                       |  current intensity)   |
 * ------------------------+-----------------------+-----------------------+-----------------------+------------------------ */

/* States of Task Actuator */
typedef enum task_actuator_cooler_st {ST_COOLER_OFF,
							   ST_COOLER_ON,
							   } task_actuator_cooler_st_t;

/* Identifier of Task Actuator */
typedef enum task_actuator_cooler_id {ID_COOLER} task_actuator_cooler_id_t;

typedef struct
{
	task_actuator_cooler_id_t	identifier;
	TIM_HandleTypeDef* htim;
	uint32_t            channel;

} task_actuator_cooler_cfg_t;

typedef struct
{
	uint32_t			tick;
	task_actuator_cooler_st_t	state;
	bool				flag_activar_cooler;
	uint16_t			intensity;
} task_actuator_cooler_dta_t;

/********************** external data declaration ****************************/
extern task_actuator_cooler_dta_t task_actuator_cooler_dta;

/********************** external functions declaration ***********************/

/********************** End of CPP guard *************************************/
#ifdef __cplusplus
}
#endif

#endif /* TASK_INC_TASK_ACTUATOR_COOLER_ATTRIBUTE_H_ */

/********************** end of file ******************************************/
