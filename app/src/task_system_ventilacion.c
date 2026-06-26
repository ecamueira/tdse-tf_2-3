
/********************** inclusions *******************************************/

/* Project includes */
#include <task_actuator_cooler_attribute.h>
#include <task_actuator_cooler_interface.h>
#include "main.h"

/* Demo includes */
#include "logger.h"

/* Application & Tasks includes */
#include "board.h"
#include "app.h"
#include "task_system_ventilacion_attribute.h"
#include "task_system_ventilacion_interface.h"

/********************** macros and definitions *******************************/
#define G_TASK_SYS_VEN_CNT_INI			0ul
#define G_TASK_SYS_VEN_TICK_CNT_INI		0ul

#define DEL_SYS_VEN_MIN					0ul
#define DEL_SYS_VEN_HA_MAX					60
#define DEL_SYS_VEN_TA_MAX					35
#define UMBRAL_VENT_ON     3.0f
#define UMBRAL_VENT_OFF    0.5f
#define RPM_MINIMAS_COOLER 500
#define TIEMPO_ARRANQUE    2000


#define PWM_MAXIMO           1000
#define PWM_MINIMO_SEGURO    300
#define GANANCIA_VENT        50
#define TOLERANCIA_PWM 15

/********************** internal data declaration ****************************/
task_system_ventilacion_dta_t task_system_ventilacion_dta =
	{DEL_SYS_VEN_MIN, ST_SYS_VEN_IDLE,0,0,DEL_SYS_VEN_TA_MAX,DEL_SYS_VEN_HA_MAX,0};

#define SYSTEM_VEN_DTA_QTY	(sizeof(task_system_ventilacion_dta)/sizeof(task_system_ventilacion_dta_t))

/********************** internal functions declaration ***********************/
void task_system_ventilacion_statechart(void);
void calcular_coeficiente_ventilacion(
		 float T_act, uint8_t T_set,
		 float H_act,uint8_t H_set,
    float* out_C_ven);

/********************** internal data definition *****************************/
const char *p_task_system_ventilacion 		= "Task System (System Statechart)";
const char *p_task_system_ventilacion_ 		= "Non-Blocking & Update By Time Code";

/********************** external data declaration ****************************/
uint32_t g_task_system_ventilacion_cnt;
volatile uint32_t g_task_system_ventilacion_tick_cnt;

/********************** external functions definition ************************/
void task_system_ventilacion_init(void *parameters)
{
	/* Print out: Task Initialized */
	LOGGER_INFO(" ");
	LOGGER_INFO("  %s is running - %s", GET_NAME(task_system_ventilacion_init), p_task_system_ventilacion);
	LOGGER_INFO("  %s is a %s", GET_NAME(task_system_ventilacion), p_task_system_ventilacion_);

	/* Init & Print out: Task execution counter */
	g_task_system_ventilacion_cnt = G_TASK_SYS_VEN_CNT_INI;
	LOGGER_INFO("   %s = %lu", GET_NAME(g_task_system_ventilacion_cnt), g_task_system_ventilacion_cnt);

}

void task_system_ventilacion_update(void *parameters)
{
	bool b_time_update_required = false;

	    /* Protect shared resource */
	    __asm("CPSID i");    /* disable interrupts */
	    if (G_TASK_SYS_VEN_TICK_CNT_INI < g_task_system_ventilacion_tick_cnt)
	    {
	        /* Update Tick Counter */
	        g_task_system_ventilacion_tick_cnt--;
	        b_time_update_required = true;
	    }
	    __asm("CPSIE i");    /* enable interrupts */

	    while (b_time_update_required)
	    {
	        /* Update Task Counter */
	        g_task_system_ventilacion_cnt++;

	        /* Run Task Statechart */
	        task_system_ventilacion_statechart();

	        /* Protect shared resource */
	        __asm("CPSID i");    /* disable interrupts */
	        if (G_TASK_SYS_VEN_TICK_CNT_INI < g_task_system_ventilacion_tick_cnt)
	        {
	            /* Update Tick Counter */
	            g_task_system_ventilacion_tick_cnt--;
	            b_time_update_required = true;
	        }
	        else
	        {
	            b_time_update_required = false;
	        }
	        __asm("CPSIE i");    /* enable interrupts */
	    }
}

void task_system_ventilacion_statechart(void)
{
	task_system_ventilacion_dta_t *p_task_system_ventilacion_dta;
	float C_ven = 0.0f;
	uint16_t pwm_calculado = 0;
	int diferencia;

	/* Update Task System Data Pointer */
	p_task_system_ventilacion_dta = &task_system_ventilacion_dta;
	calcular_coeficiente_ventilacion(
       p_task_system_ventilacion_dta->temperatura, p_task_system_ventilacion_dta->temperatura_ambiente_max,p_task_system_ventilacion_dta->humedad_ambiente,
	   p_task_system_ventilacion_dta->humedad_ambiente_min, &C_ven
    );

	switch (p_task_system_ventilacion_dta->state)
	{
		case ST_SYS_VEN_IDLE:

	        if (C_ven > UMBRAL_VENT_ON)
	        {
	            p_task_system_ventilacion_dta->tick = TIEMPO_ARRANQUE;
	            p_task_system_ventilacion_dta->state = ST_SYS_VEN_STARTING;
	            p_task_system_ventilacion_dta->pwm_actual=PWM_MAXIMO;
	        	put_task_actuator_cooler(true,p_task_system_ventilacion_dta->pwm_actual);
	        }
	        break;

	    case ST_SYS_VEN_STARTING:

	    	if (p_task_system_ventilacion_dta->tick == 0)
	        {
	    		p_task_system_ventilacion_dta->state = ST_SYS_VEN_ACTIVE;
	        }else
	        {
	        	p_task_system_ventilacion_dta->tick--;
	        }
	        break;

	    case ST_SYS_VEN_ACTIVE:

	        if (C_ven < UMBRAL_VENT_OFF)
	        {
	        	put_task_actuator_cooler(false,0);
	            p_task_system_ventilacion_dta->state = ST_SYS_VEN_IDLE;
	        }else
	        {
	        	 // REGULACIÓN PROPORCIONAL CONTINUA
	             pwm_calculado = ((uint16_t)(C_ven - UMBRAL_VENT_ON) * GANANCIA_VENT);

	             if (pwm_calculado > PWM_MAXIMO) pwm_calculado = PWM_MAXIMO;
	             if (pwm_calculado < PWM_MINIMO_SEGURO) pwm_calculado = PWM_MINIMO_SEGURO;

	             diferencia = (int)pwm_calculado - (int)p_task_system_ventilacion_dta->pwm_actual;
	             if (diferencia >= TOLERANCIA_PWM || diferencia <= -TOLERANCIA_PWM)
	             {
	            	 p_task_system_ventilacion_dta->pwm_actual = pwm_calculado;
	                 put_task_actuator_cooler(true,p_task_system_ventilacion_dta->pwm_actual);
	             }

	        }
	        break;

	    break;
		default:

			p_task_system_ventilacion_dta->tick  = DEL_SYS_VEN_MIN;
			p_task_system_ventilacion_dta->state = ST_SYS_VEN_IDLE;
			break;
	}
}
void calcular_coeficiente_ventilacion(float T_act, uint8_t T_set, float H_act, uint8_t H_set, float *out_C_ven)
{
    // 1. Error térmico
    float E_t = T_act - (float)T_set;

    // 2. Error hídrico.
    float E_h = H_act - (float)H_set;

    // Le damos un peso extra a la humedad porque en un indoor cerrado,
    *out_C_ven = E_t + (E_h * 1.5f);
}

/********************** end of file ******************************************/
