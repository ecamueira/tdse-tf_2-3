
#ifndef TASK_INC_TASK_SYSTEM_MEMORY_INTERFACE_H_
#define TASK_INC_TASK_SYSTEM_MEMORY_INTERFACE_H_

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

/********************** external functions declaration ***********************/
extern void put_system_memory_user_config(uint8_t color, uint8_t intensidad, uint8_t temp_max,  uint8_t hum_amb_min, uint8_t hum_suelo_min);
extern void put_system_memory_user_config_flag(bool Hay_datos);

/********************** End of CPP guard *************************************/
#ifdef __cplusplus
}
#endif

#endif /* TASK_INC_TASK_SYSTEM_MEMORY_INTERFACE_H_ */

/********************** end of file ******************************************/
