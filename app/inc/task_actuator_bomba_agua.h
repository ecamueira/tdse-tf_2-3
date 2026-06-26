
#ifndef TASK_INC_TASK_ACTUATOR_BOMBA_AGUA_H_
#define TASK_INC_TASK_ACTUATOR_BOMBA_AGUA_H_

/********************** CPP guard ********************************************/
#ifdef __cplusplus
extern "C" {
#endif

/********************** inclusions *******************************************/
#include "main.h"
/********************** macros ***********************************************/

/********************** typedef **********************************************/

/********************** external data declaration ****************************/
extern uint32_t g_task_actuator_bomba_agua_cnt;
extern volatile uint32_t g_task_actuator_bomba_agua_tick_cnt;

/********************** external functions declaration ***********************/
extern void task_actuator_bomba_agua_init(void *parameters);
extern void task_actuator_bomba_agua_update(void *parameters);

/********************** End of CPP guard *************************************/
#ifdef __cplusplus
}
#endif

#endif /* TASK_INC_TASK_ACTUATOR_H_ */

/********************** end of file ******************************************/
