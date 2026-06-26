

#ifndef TASK_INC_TASK_SENSOR_DIG_ATTRIBUTE_H_
#define TASK_INC_TASK_SENSOR_DIG_ATTRIBUTE_H_

/********************** CPP guard ********************************************/
#ifdef __cplusplus
extern "C" {
#endif

/********************** inclusions *******************************************/
#include <stdint.h>
#include "main.h"

/********************** macros ***********************************************/

/********************** typedef **********************************************/
/* Digital Sensor (AHT10) Statechart - State Transition Table */
/* ------------------------+-----------------------+-----------------------+-----------------------+------------------------
 * | Current               | Event                 |                       | Next                  |                       |
 * | State                 | (Parameters)          | [Guard]               | State                 | Actions               |
 * |=======================+=======================+=======================+=======================+=======================|
 * | ST_DIG_INIT           | data_ready_AHT10      | [Range OK]            | ST_DIG_SETUP          | Update History        |
 * |                       |                       |                       |                       | calc delta_temp/hum   |
 * |-----------------------+-----------------------+-----------------------+-----------------------+-----------------------|
 * | ST_DIG_SETUP          | data_ready_AHT10      | [Range OK AND         | ST_DIG_SETUP          | Update History        |
 * |                       |                       |  Delta < Max]         |                       | put_system_value()    |
 * |                       +-----------------------+-----------------------+-----------------------+-----------------------|
 * |                       | data_ready_AHT10      | [Range Error OR       | ST_DIG_REVISION       | tick = DEL_SEN_DIG_MAX|
 * |                       |                       |  Delta > Max]         |                       |                       |
 * |-----------------------+-----------------------+-----------------------+-----------------------+-----------------------|
 * | ST_DIG_REVISION       | data_ready_AHT10      | [tick > 0]            | ST_DIG_REVISION       | tick--                |
 * |                       +-----------------------+-----------------------+-----------------------+-----------------------|
 * |                       | data_ready_AHT10      | [tick == 0 AND        | ST_DIG_SETUP          | -                     |
 * |                       |                       |  Range & Delta OK]    |                       |                       |
 * |                       +-----------------------+-----------------------+-----------------------+-----------------------|
 * |                       | data_ready_AHT10      | [tick == 0 AND        | ST_DIG_FALLO          | -                     |
 * |                       |                       |  Error Persists]      |                       |                       |
 * |-----------------------+-----------------------+-----------------------+-----------------------+-----------------------|
 * | ST_DIG_FALLO          | data_ready_AHT10      | [Range OK AND         | ST_DIG_REVISION       | tick = DEL_SEN_DIG_MAX|
 * |                       |                       |  Delta < Max]         |                       |                       |
 * |                       +-----------------------+-----------------------+-----------------------+-----------------------|
 * |                       | ELSE                  |                       | ST_DIG_FALLO          | -                     |
 * ------------------------+-----------------------+-----------------------+-----------------------+------------------------ */

/* States of Task Sensor */
typedef enum task_sensor_dig_st {ST_DIG_INIT,
								ST_DIG_SETUP,
							 ST_DIG_REVISION,
							 ST_DIG_FALLO} task_sensor_dig_st_t;

typedef struct
{
	uint32_t			tick;
	task_sensor_dig_st_t	state;
	float				hum_ambiente;
	float				hum_ambiente_anterior;
	float				temperatura;
	float				temperatura_anterior;
} task_sensor_dig_dta_t;




/********************** external data declaration ****************************/
extern task_sensor_dig_dta_t task_sensor_dig_dta_list[];

/********************** external functions declaration ***********************/

/********************** End of CPP guard *************************************/
#ifdef __cplusplus
}
#endif

#endif /* TASK_INC_TASK_SENSOR_DIG_ATTRIBUTE_H_ */

/********************** end of file ******************************************/
