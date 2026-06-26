
/********************** inclusions *******************************************/
/* Project includes */
#include "main.h"

/* Demo includes */
#include "logger.h"
#include "dwt.h"

/* Application & Tasks includes */
#include "board.h"
#include "app.h"
#include "task_system_LED_attribute.h"

/********************** macros and definitions *******************************/

/********************** internal data declaration ****************************/

/********************** internal functions declaration ***********************/

/********************** internal data definition *****************************/

/********************** external data declaration ****************************/

/********************** external functions definition ************************/
void put_color_value(uint8_t valor) {
	task_system_LED_dta.color = valor;
}
void put_intensidad_value(uint8_t valor) {
	task_system_LED_dta.intensidad = valor;
}
void put_flag_new_data(bool flag){
	task_system_LED_dta.flag = flag;
}

/********************** end of file ******************************************/
