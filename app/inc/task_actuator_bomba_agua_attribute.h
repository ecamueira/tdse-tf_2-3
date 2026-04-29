
#ifndef TASK_INC_TASK_ACTUATOR_BOMBA_AGUA_ATTRIBUTE_H_
#define TASK_INC_TASK_ACTUATOR_BOMBA_AGUA_ATTRIBUTE_H_

/********************** CPP guard ********************************************/
#ifdef __cplusplus
extern "C" {
#endif

/********************** inclusions *******************************************/
#include "stdbool.h"
#include "main.h"
/********************** macros ***********************************************/

/********************** typedef **********************************************/
/* Actuator Bomba (Agua & Ferti) - State Transition Table */
/* ------------------------+-----------------------+-----------------------+-----------------------+------------------------------------------
 * | Current               | Event                 |                       | Next                  |                                         |
 * |State                 | (Parameters)          | [Guard]               | State                 | Actions                                 |
 * |=======================+=======================+=======================+=======================+=========================================|
 * | ST_BOMBA_OFF          | flag_activar_bomba    | [id == ID_BOMBA_      | ST_BOMBA_ON           | GPIO = ON                               |
 * |                       | == true               |  FERILIZANTE]         |                       | tick = TIEMPO_DOSIFICACION_FERTILIZACION|
 * |                       +-----------------------+-----------------------+-----------------------+-----------------------------------------|
 * |                       | flag_activar_bomba    | [id != ID_BOMBA_      | ST_BOMBA_ON           | GPIO = ON                               |
 * |                       | == true               |  FERILIZANTE]         |                       | (tick mantenido del evento)             |
 * |-----------------------+-----------------------+-----------------------+-----------------------+-----------------------------------------|
 * | ST_BOMBA_ON           | tick > 0              |                       | ST_BOMBA_ON           | tick--                                  |
 * |                       +-----------------------+-----------------------+-----------------------+-----------------------------------------|
 * |                       | tick == 0             | [id == ID_BOMBA_      | ST_BOMBA_ESPERA_DIARIA| GPIO = OFF                              |
 * |                       |                       |  FERILIZANTE]         |                       | tick = TIEMPO_ESPERA_FERTILIZACION      |
 * |                       +-----------------------+-----------------------+-----------------------+-----------------------------------------|
 * |                       | tick == 0             | [id != ID_BOMBA_      | ST_BOMBA_OFF          | GPIO = OFF                              |
 * |                       |                       |  FERILIZANTE]         |                       | tick = DEL_BOM_MIN                      |
 * |                       |                       |                       |                       | flag_activar_bomba = false              |
 * |-----------------------+-----------------------+-----------------------+-----------------------+-----------------------------------------|
 * | ST_BOMBA_ESPERA_DIARIA| tick > 0              |                       | ST_BOMBA_ESPERA_DIARIA| tick--                                  |
 * |                       +-----------------------+-----------------------+-----------------------+-----------------------------------------|
 * |                       | tick == 0             |                       | ST_BOMBA_OFF          | -                                       |
 * ------------------------+-----------------------+-----------------------+-----------------------+----------------------------------------- */
/* States of Task Actuator */
typedef enum task_actuator_bomba_agua_st {ST_BOMBA_OFF,
							   	   	   	  ST_BOMBA_ON,
										  ST_BOMBA_ESPERA_DIARIA} task_actuator_bomba_agua_st_t;

/* Identifier of Task Actuator */
typedef enum task_actuator_bomba_agua_id {ID_BOMBA_AGUA,
										  ID_BOMBA_FERTILIZANTE} task_actuator_bomba_agua_id_t;

typedef struct
{
	task_actuator_bomba_agua_id_t	identifier;
	GPIO_TypeDef *		gpio_port;
	uint16_t			pin;
	GPIO_PinState		bomba_on;
	GPIO_PinState		bomba_off;
} task_actuator_bomba_agua_cfg_t;

typedef struct
{
	uint32_t			tick;
	task_actuator_bomba_agua_st_t	state;
	bool				flag_activar_bomba;
} task_actuator_bomba_agua_dta_t;

/********************** external data declaration ****************************/
extern task_actuator_bomba_agua_dta_t task_actuator_bomba_agua_dta_list[];

/********************** external functions declaration ***********************/

/********************** End of CPP guard *************************************/
#ifdef __cplusplus
}
#endif

#endif /* TASK_INC_TASK_ACTUATOR_ATTRIBUTE_H_ */

/********************** end of file ******************************************/
