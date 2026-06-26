

/********************** inclusions *******************************************/
/* Project includes */
#include "main.h"

/* Demo includes */
#include "logger.h"
#include "dwt.h"

/* Application & Tasks includes */
#include "board.h"
#include "app.h"
#include "task_actuator_attribute_LED.h"

/********************** macros and definitions *******************************/

/********************** internal data declaration ****************************/

/********************** internal functions declaration ***********************/

/********************** internal data definition *****************************/

/********************** external data declaration ****************************/

/********************** external functions definition ************************/
void put_task_actuator_LED(task_actuator_LED_ev_t event, uint8_t intensidad)
{
	task_actuator_LED_dta_t *p_task_actuator_LED_dta;
	p_task_actuator_LED_dta = &task_actuator_LED_dta;
	p_task_actuator_LED_dta->intensity = intensidad;
	p_task_actuator_LED_dta->event = event;
	p_task_actuator_LED_dta->flag = true;
}

/********************** end of file ******************************************/
