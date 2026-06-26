
#ifndef TASK_INC_TASK_SYSTEM_RIEGO_ATTRIBUTE_H_
#define TASK_INC_TASK_SYSTEM_RIEGO_ATTRIBUTE_H_

/********************** CPP guard ********************************************/
#ifdef __cplusplus
extern "C" {
#endif

/********************** inclusions *******************************************/
#include <stdint.h>
#include <stdbool.h>
/********************** macros ***********************************************/

/********************** typedef **********************************************/
/* System Riego Statechart - State Transition Table */
/* ------------------------+-----------------------+-----------------------+-----------------------+------------------------------------------
 * | Current               | Event                 |                       | Next                  |                                         |
 * | State                 | (Parameters)          | [Guard]               | State                 | Actions                                 |
 * |=======================+=======================+=======================+=======================+=========================================|
 * | ST_SYS_RIEGO_NO_REGAR | calcular_coeficiente_ | [C_riego > 3.0f AND   | ST_SYS_RIEGO_ESPERA_  | ticks = C_riego * MULTIPLICADOR_TICKS   |
 * |                       | riego()               |  flag_tanque == true] | ABSORCION             | Limitador(MINIMO_TICKS, MAXIMO_TICKS)   |
 * |                       |                       |                       |                       | p_task_dta->tick = ticks + ESPERA_ABSOR |
 * |                       |                       |                       |                       | put_event_actuator_bomba(true, ticks)   |
 * |                       +-----------------------+-----------------------+-----------------------+-----------------------------------------|
 * |                       | calcular_coeficiente_ | [C_riego <= 3.0f OR   | ST_SYS_RIEGO_NO_REGAR | -                                       |
 * |                       | riego()               |  flag_tanque == false]|                       |                                         |
 * |-----------------------+-----------------------+-----------------------+-----------------------+-----------------------------------------|
 * | ST_SYS_RIEGO_ESPERA_  | p_task_dta->tick > 0  |                       | ST_SYS_RIEGO_ESPERA_  | p_task_dta->tick--                      |
 * | ABSORCION             |                       |                       | ABSORCION             |                                         |
 * |                       +-----------------------+-----------------------+-----------------------+-----------------------------------------|
 * |                       | p_task_dta->tick == 0 |                       | ST_SYS_RIEGO_NO_REGAR | -                                       |
 * |-----------------------+-----------------------+-----------------------+-----------------------+-----------------------------------------|
 * | DEFAULT               | Any                   |                       | ST_SYS_RIEGO_NO_REGAR | -                                       |
 * ------------------------+-----------------------+-----------------------+-----------------------+----------------------------------------- */

/* State of Task System */
typedef enum task_system_riego_st {ST_SYS_RIEGO_REGAR,
								   ST_SYS_RIEGO_ESPERA_ABSORCION,
								   ST_SYS_RIEGO_NO_REGAR} task_system_riego_st_t;

typedef struct
{
	uint32_t			tick;
	task_system_riego_st_t	state;
	bool				flag_tanque;
	uint16_t 			humedad_suelo;
	float				humedad_ambiente;
	float				temperatura;
	uint8_t				humedad_suelo_min;
	uint8_t				humedad_ambiente_min;
	uint8_t				temperatura_ambiente_max;
} task_system_riego_dta_t;

/********************** external data declaration ****************************/
extern task_system_riego_dta_t task_system_riego_dta;

/********************** external functions declaration ***********************/

/********************** End of CPP guard *************************************/
#ifdef __cplusplus
}
#endif

#endif /* TASK_INC_TASK_SYSTEM_ATTRIBUTE_H_ */

/********************** end of file ******************************************/
