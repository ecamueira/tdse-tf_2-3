

#ifndef TASK_INC_TASK_SENSOR_DIG_H_
#define TASK_INC_TASK_SENSOR_DIG_H_

/********************** CPP guard ********************************************/
#ifdef __cplusplus
extern "C" {
#endif

/********************** inclusions *******************************************/

/********************** macros ***********************************************/

/********************** typedef **********************************************/

/********************** external data declaration ****************************/
extern uint32_t g_task_sensor_digital_cnt;
extern volatile uint32_t g_task_sensor_digital_tick_cnt;

/********************** external functions declaration ***********************/
extern void task_sensor_digital_init(void *parameters);
extern void task_sensor_digital_update(void *parameters);

/********************** End of CPP guard *************************************/
#ifdef __cplusplus
}
#endif

#endif /* TASK_INC_TASK_SENSOR_DIG_H_ */

/********************** end of file ******************************************/
