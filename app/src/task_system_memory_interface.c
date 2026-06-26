
/********************** inclusions *******************************************/
/* Project includes */
#include <task_system_memory_attribute.h>
#include "main.h"

/* Demo includes */
#include "logger.h"
#include "dwt.h"

/* Application & Tasks includes */
#include "board.h"
#include "app.h"

/********************** macros and definitions *******************************/

/********************** internal data declaration ****************************/

/********************** internal functions declaration ***********************/

/********************** internal data definition *****************************/

/********************** external data declaration ****************************/

/********************** external functions definition ************************/
void put_system_memory_user_config(uint8_t color, uint8_t intensidad, uint8_t temp_max,  uint8_t hum_amb_min, uint8_t hum_suelo_min)
{
	task_system_memory_dta.color_led=color;
	task_system_memory_dta.intensidad_led=intensidad;
	task_system_memory_dta.temperatura=temp_max;
	task_system_memory_dta.humedad_aire=hum_amb_min;
	task_system_memory_dta.humedad_suelo=hum_suelo_min;
}
void put_system_memory_user_config_flag(bool Hay_datos)
{
	task_system_memory_dta.flag=Hay_datos;
}

/********************** end of file ******************************************/
