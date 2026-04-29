
#ifndef TASK_INC_TASK_SENSOR_ANALOG_H_
#define TASK_INC_TASK_SENSOR_ANALOG_H_

/********************** CPP guard ********************************************/
#ifdef __cplusplus
extern "C" {
#endif

/********************** inclusions *******************************************/
#include <stdint.h>
#include <stdbool.h>

/********************** macros ***********************************************/

/********************** typedef **********************************************/

/********************** external data declaration ****************************/
extern uint32_t g_task_sensor_analog_cnt;
extern volatile uint32_t g_task_sensor_analog_tick_cnt;

/********************** external functions declaration ***********************/
extern void task_sensor_analog_init(void *parameters);
extern void task_sensor_analog_update(void *parameters);

/********************** End of CPP guard *************************************/
#ifdef __cplusplus
}
#endif

#endif /* TASK_INC_TASK_SENSOR_ANALOG_H_ */

/********************** end of file ******************************************/
