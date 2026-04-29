
#ifndef TASK_INC_TASK_SYSTEM_VENTILACION_ATTRIBUTE_H_
#define TASK_INC_TASK_SYSTEM_VENTILACION_ATTRIBUTE_H_

/********************** CPP guard ********************************************/
#ifdef __cplusplus
extern "C" {
#endif

/********************** inclusions *******************************************/
#include <stdint.h>
#include <stdbool.h>

/********************** macros ***********************************************/

/********************** typedef **********************************************/
/* System Ventilacion Statechart - State Transition Table */
/* ------------------------+-----------------------+-----------------------+-----------------------+------------------------------------------
 * | Current               | Event                 |                       | Next                  |                                         |
 * | State                 | (Parameters)          | [Guard]               | State                 | Actions                                 |
 * |=======================+=======================+=======================+=======================+=========================================|
 * | ST_SYS_VEN_IDLE       | calcular_coeficiente_ | [C_ven > UMBRAL_VENT_ | ST_SYS_VEN_STARTING   | tick = TIEMPO_ARRANQUE                  |
 * |                       | ventilacion()         |  ON]                  |                       | put_actuator_cooler(true, PWM_MAXIMO)   |
 * |-----------------------+-----------------------+-----------------------+-----------------------+-----------------------------------------|
 * | ST_SYS_VEN_STARTING   | tick > 0              |                       | ST_SYS_VEN_STARTING   | tick--                                  |
 * |                       +-----------------------+-----------------------+-----------------------+-----------------------------------------|
 * |                       | tick == 0             |                       | ST_SYS_VEN_ACTIVE     | -                                       |
 * |-----------------------+-----------------------+-----------------------+-----------------------+-----------------------------------------|
 * | ST_SYS_VEN_ACTIVE     | calcular_coeficiente_ | [C_ven < UMBRAL_VENT_ | ST_SYS_VEN_IDLE       | put_actuator_cooler(false, 0)           |
 * |                       | ventilacion()         |  OFF]                 |                       |                                         |
 * |                       +-----------------------+-----------------------+-----------------------+-----------------------------------------|
 * |                       | calcular_coeficiente_ | [C_ven >= UMBRAL_     | ST_SYS_VEN_ACTIVE     | pwm_calc = (C_ven - ON) * GANANCIA      |
 * |                       | ventilacion()         |  VENT_OFF]            |                       | if (diff > TOLERANCIA) -> Update PWM    |
 * |-----------------------+-----------------------+-----------------------+-----------------------------------------------------------------|
 * | ST_SYS_VEN_FALLO      | tick == 0             |                       | ST_SYS_VEN_IDLE       | tick = TIEMPO_ARRANQUE                  |
 * |-----------------------+-----------------------+-----------------------+-----------------------+-----------------------------------------|
 * | DEFAULT               | Any                   |                       | ST_SYS_VEN_IDLE       | tick = DEL_SYS_VEN_MIN                  |
 * ------------------------+-----------------------+-----------------------+-----------------------+----------------------------------------- */

/* State of Task System */
typedef enum task_system_ventilacion_st {ST_SYS_VEN_IDLE,
										 ST_SYS_VEN_STARTING,
										 ST_SYS_VEN_ACTIVE
} task_system_ventilacion_st_t;

typedef struct
{
	uint32_t			tick;
	task_system_ventilacion_st_t	state;
	float				humedad_ambiente;
	float				temperatura;
	uint8_t				humedad_ambiente_min;
	uint8_t				temperatura_ambiente_max;
	uint16_t 			pwm_actual;
} task_system_ventilacion_dta_t;

/********************** external data declaration ****************************/
extern task_system_ventilacion_dta_t task_system_ventilacion_dta;

/********************** external functions declaration ***********************/

/********************** End of CPP guard *************************************/
#ifdef __cplusplus
}
#endif

#endif /* TASK_INC_TASK_SYSTEM_ATTRIBUTE_H_ */

/********************** end of file ******************************************/
