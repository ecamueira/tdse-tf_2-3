
#ifndef TASK_INC_TASK_SYSTEM_RIEGO_INTERFACE_H_
#define TASK_INC_TASK_SYSTEM_RIEGO_INTERFACE_H_

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
extern void put_humedad_suelo_value(uint16_t valor);
extern void put_humedad_suelo_value_min(uint8_t valor);
extern void put_riego_humedad_ambiente_value_min(uint8_t valor);
extern void put_riego_temperatura_ambiente_value_max(uint8_t valor);
extern void put_riego_humedad_temperatura_ambiente_value(float hum_amb, float temp_amb);
extern void put_level_tanque_agua_value(bool flag_tanque);
/********************** End of CPP guard *************************************/
#ifdef __cplusplus
}
#endif

#endif /* TASK_INC_TASK_SYSTEM_INTERFACE_H_ */

/********************** end of file ******************************************/
