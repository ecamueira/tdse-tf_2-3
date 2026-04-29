
#ifndef TASK_INC_TASK_ACTUATOR_BOMBA_AGUA_INTERFACE_H_
#define TASK_INC_TASK_ACTUATOR_BOMBA_AGUA_INTERFACE_H_

/********************** CPP guard ********************************************/
#ifdef __cplusplus
extern "C" {
#endif

/********************** inclusions *******************************************/
#include "main.h"
#include <stdint.h>
#include <stdbool.h>
/********************** macros ***********************************************/

/********************** typedef **********************************************/

/********************** external data declaration ****************************/

/********************** external functions declaration ***********************/
extern void put_event_task_actuator_bomba_agua(bool flag, uint16_t tick, int identifier);
extern void put_flag_task_actuator_bomba_fetilizante(bool flag, int identifier);

/********************** End of CPP guard *************************************/
#ifdef __cplusplus
}
#endif

#endif /* TASK_INC_TASK_ACTUATOR_INTERFACE_H_ */

/********************** end of file ******************************************/
