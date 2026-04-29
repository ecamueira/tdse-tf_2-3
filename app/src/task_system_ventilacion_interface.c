
/********************** inclusions *******************************************/
/* Project includes */
#include <task_system_ventilacion_attribute.h>
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
void put_ventilacion_humedad_ambiente_value_min(uint8_t valor) {
    task_system_ventilacion_dta.humedad_ambiente_min=valor;
}
void put_ventilacion_temperatura_ambiente_value_max(uint8_t valor) {
    task_system_ventilacion_dta.temperatura_ambiente_max=valor;
}
void put_ventilacion_humedad_temperatura_ambiente_value(float hum_amb, float temp_amb) {
    task_system_ventilacion_dta.humedad_ambiente= hum_amb;
    task_system_ventilacion_dta.temperatura = temp_amb;
}
/********************** end of file ******************************************/
