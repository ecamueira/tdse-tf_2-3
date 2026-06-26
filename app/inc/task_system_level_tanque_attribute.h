
#ifndef TASK_INC_TASK_SYSTEM_LEVEL_TANQUE_ATTRIBUTE_H_
#define TASK_INC_TASK_SYSTEM_LEVEL_TANQUE_ATTRIBUTE_H_

/********************** CPP guard ********************************************/
#ifdef __cplusplus
extern "C" {
#endif

/********************** inclusions *******************************************/
#include <stdint.h>
#include <stdbool.h>
/********************** macros ***********************************************/

/********************** typedef **********************************************/
/* System Level Tanque Statechart - State Transition Table */
/* ------------------------+-----------------------+-----------------------+-----------------------+------------------------
 * | Current               | Event                 |                       | Next                  |                       |
 * | State                 | (Parameters)          | [Guard]               | State                 | Actions               |
 * |=======================+=======================+=======================+=======================+=======================|
 * | ST_SYS_LEV_TAN_VACIO  | Nivel de Tanque       | [nivel > 200]         | ST_SYS_LEV_TAN_IDLE   | flag = true           |
 * | 			           |                       |                       |                       | put_tanque_agua(true) |
 * |                       +-----------------------+-----------------------+-----------------------+-----------------------|
 * |                       | Nivel de Tanque       | [nivel <= 200]        | ST_SYS_LEV_TAN_VACIO  | -                     |
 * |-----------------------+-----------------------+-----------------------+-----------------------+-----------------------|
 * | ST_SYS_LEV_TAN_IDLE   | Nivel de Tanque       | [nivel <= 100]        | ST_SYS_LEV_TAN_VACIO  | flag = false          |
 * |                       |                       |                       |                       | put_tanque_agua(false)|
 * |                       +-----------------------+-----------------------+-----------------------+-----------------------|
 * |                       | Nivel de Tanque       | [nivel > 100]         | ST_SYS_LEV_TAN_IDLE   | -                     |
 * ------------------------+-----------------------+-----------------------+-----------------------+------------------------ */
/* State of Task System */
typedef enum task_system_level_tanque_st {ST_SYS_LEV_TAN_IDLE,
										 ST_SYS_LEV_TAN_VACIO} task_system_level_tanque_st_t;

/* Identifier of Task System Level Tanque */
typedef enum task_system_level_tanque_id {ID_SYS_TANQUE_AGUA,
									      ID_SYS_TANQUE_FERTILIZANTE} task_system_level_tanque_id_t;
typedef struct
{
	task_system_level_tanque_id_t identifier;
	task_system_level_tanque_st_t	state;
	uint16_t							nivel_actual;
	uint16_t							nivel_anterior;
	bool				flag;
} task_system_level_tanque_dta_t;

/********************** external data declaration ****************************/
extern task_system_level_tanque_dta_t task_system_level_tanque_dta_list[];

/********************** external functions declaration ***********************/

/********************** End of CPP guard *************************************/
#ifdef __cplusplus
}
#endif

#endif /* TASK_INC_TASK_SYSTEM_LEVEL_TANQUE_ATTRIBUTE_H_ */

/********************** end of file ******************************************/
