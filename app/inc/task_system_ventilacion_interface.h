
#ifndef TASK_INC_TASK_SYSTEM_VENTILACION_INTERFACE_H_
#define TASK_INC_TASK_SYSTEM_VENTILACION_INTERFACE_H_

/********************** CPP guard ********************************************/
#ifdef __cplusplus
extern "C" {
#endif

/********************** inclusions *******************************************/

/********************** macros ***********************************************/

/********************** typedef **********************************************/

/********************** external data declaration ****************************/

/********************** external functions declaration ***********************/
extern void put_ventilacion_humedad_ambiente_value_min(uint8_t valor);
extern void put_ventilacion_temperatura_ambiente_value_max(uint8_t valor);
extern void put_ventilacion_humedad_temperatura_ambiente_value(float hum_amb, float temp_amb);
/********************** End of CPP guard *************************************/
#ifdef __cplusplus
}
#endif

#endif /* TASK_INC_TASK_SYSTEM_INTERFACE_H_ */

/********************** end of file ******************************************/
