
#ifndef TASK_INC_TASK_SYSTEM_LED_INTERFACE_H_
#define TASK_INC_TASK_SYSTEM_LED_INTERFACE_H_

/********************** CPP guard ********************************************/
#ifdef __cplusplus
extern "C" {
#endif

/********************** inclusions *******************************************/

/********************** macros ***********************************************/

/********************** typedef **********************************************/

/********************** external data declaration ****************************/

/********************** external functions declaration ***********************/
extern void put_color_value(uint8_t valor);
extern void put_intensidad_value(uint8_t valor);
extern void put_flag_new_data(bool flag);
/********************** End of CPP guard *************************************/
#ifdef __cplusplus
}
#endif

#endif /* TASK_INC_TASK_SYSTEM_LED_INTERFACE_H_ */

/********************** end of file ******************************************/
