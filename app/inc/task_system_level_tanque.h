
#ifndef TASK_INC_TASK_SYSTEM_LEVEL_TANQUE_H_
#define TASK_INC_TASK_SYSTEM_LEVEL_TANQUE_H_

/********************** CPP guard ********************************************/
#ifdef __cplusplus
extern "C" {
#endif

/********************** inclusions *******************************************/

/********************** macros ***********************************************/

/********************** typedef **********************************************/

/********************** external data declaration ****************************/
extern uint32_t g_task_system_level_tanque_cnt;
extern volatile uint32_t g_task_system_level_tanque_tick_cnt;

/********************** external functions declaration ***********************/
extern void task_system_level_tanque_init(void *parameters);
extern void task_system_level_tanque_update(void *parameters);

/********************** End of CPP guard *************************************/
#ifdef __cplusplus
}
#endif

#endif /* TASK_INC_TASK_SYSTEM_LEVEL_TANQUE_H_ */

/********************** end of file ******************************************/
