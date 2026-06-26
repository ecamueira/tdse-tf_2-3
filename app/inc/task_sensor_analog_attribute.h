

#ifndef TASK_INC_TASK_SENSOR_ANALOG_ATTRIBUTE_H_
#define TASK_INC_TASK_SENSOR_ANALOG_ATTRIBUTE_H_

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
/* Analog Sensors Statechart - State Transition Table */
/* ------------------------+-----------------------+-----------------------+-----------------------+------------------------
 * | Current               | Event                 |                       | Next                  |                       |
 * | State                 | (Parameters)          | [Guard]               | State                 | Actions               |
 * |=======================+=======================+=======================+=======================+=======================|
 * | ST_SEN_ADC_SETUP      | ADC Conversion Done   | [val within limits]   | ST_SEN_ADC_SETUP      | put_value() (Update   |
 * |                       |                       |                       |                       | system interface)     |
 * |                       +-----------------------+-----------------------+-----------------------+-----------------------|
 * |                       | ADC Conversion Done   | [val out of limits]   | ST_SEN_ADC_REVISION   | tick = DEL_SEN_ADC_MAX|
 * |-----------------------+-----------------------+-----------------------+-----------------------+-----------------------|
 * | ST_SEN_ADC_REVISION   | tick > 0              |                       | ST_SEN_ADC_REVISION   | tick--                |
 * |                       +-----------------------+-----------------------+-----------------------+-----------------------|
 * |                       | tick == 0             | [val within limits]   | ST_SEN_ADC_SETUP      | -                     |
 * |                       +-----------------------+-----------------------+-----------------------+-----------------------|
 * |                       | tick == 0             | [val out of limits]   | ST_SEN_ADC_FALLO      | -                     |
 * |-----------------------+-----------------------+-----------------------+-----------------------+-----------------------|
 * | ST_SEN_ADC_FALLO      | ADC Conversion Done   | [val within limits]   | ST_SEN_ADC_REVISION   | tick = DEL_SEN_ADC_MAX|
 * |                       +-----------------------+-----------------------+-----------------------+-----------------------|
 * |                       | ELSE                  |                       | ST_SEN_ADC_FALLO      | -                     |
 * ------------------------+-----------------------+-----------------------+-----------------------+------------------------ */

/* States of Task Sensor */
typedef enum task_sensor_analog_st {ST_SEN_ADC_SETUP,
							 ST_SEN_ADC_REVISION,
							 ST_SEN_ADC_FALLO} task_sensor_analog_st_t;

/* Identifier of Task Sensor */
typedef enum task_sensor_analog_id {ID_SEN_HUM_SUELO,
							ID_SEN_NIVEL_AGUA,
							ID_SEN_NIVEL_FERTILIZANTE} task_sensor_analog_id_t;

typedef struct {
	task_sensor_analog_id_t	identifier;
    ADC_HandleTypeDef *hadc;
    uint32_t channel;
    uint16_t valor_max;
    uint16_t valor_min;
} task_sensor_analog_cfg_t;

typedef struct
{
	uint32_t			tick;
	task_sensor_analog_st_t	state;
	uint16_t				valor1;
} task_sensor_analog_dta_t;




/********************** external data declaration ****************************/
extern task_sensor_analog_dta_t task_sensor_analog_dta_list[];

/********************** external functions declaration ***********************/

/********************** End of CPP guard *************************************/
#ifdef __cplusplus
}
#endif

#endif /* TASK_INC_TASK_SENSOR_ATTRIBUTE_H_ */

/********************** end of file ******************************************/
