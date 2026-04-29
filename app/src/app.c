
/********************** inclusions *******************************************/
/* Project includes */
#include <task_actuator_bomba_agua.h>
#include <task_sensor_analog.h>
#include <task_sensor_digital.h>
#include "main.h"

/* Demo includes */
#include "logger.h"
#include "dwt.h"

/* Application & Tasks includes */
#include "board.h"
#include "task_system_riego.h"
#include "task_system_ventilacion.h"
#include "task_system_level_tanque.h"
#include "task_system_LED.h"
#include "task_system_memory.h"
#include "task_sensor_BLE.h"
#include "task_actuator_LED.h"
#include "task_actuator_cooler.h"
#include "task_actuator_bomba_agua.h"

/********************** macros and definitions *******************************/
#define G_APP_CNT_INI		0ul
#define G_APP_TICK_CNT_INI	0ul

#define TASK_X_WCET_INI		0ul
#define TASK_X_DELAY_MIN	0ul

typedef struct {
	void (*task_init)(void *);		// Pointer to task (must be a
									// 'void (void *)' function)
	void (*task_update)(void *);	// Pointer to task (must be a
									// 'void (void *)' function)
	void *parameters;				// Pointer to parameters
} task_cfg_t;

typedef struct {
    uint32_t WCET;			// Worst-case execution time (microseconds)
} task_dta_t;

/********************** internal data declaration ****************************/
const task_cfg_t task_cfg_list[]	= {
		{task_system_memory_init, task_system_memory_update, NULL},
		{task_sensor_analog_init, task_sensor_analog_update, 	NULL},
		{task_sensor_digital_init, task_sensor_digital_update, 	NULL},
		{task_sensor_BLE_init,task_sensor_BLE_update, NULL},
		{task_system_ventilacion_init,task_system_ventilacion_update, 	NULL},
		{task_system_LED_init,task_system_LED_update, 	NULL},
		{task_system_level_tanque_init,task_system_level_tanque_update, 	NULL},
		{task_system_riego_init,task_system_riego_update, 	NULL},
		{task_actuator_bomba_agua_init,task_actuator_bomba_agua_update,NULL},
		{task_actuator_cooler_init,task_actuator_cooler_update,NULL},
		{task_actuator_LED_init,	task_actuator_LED_update, 	NULL}
};

#define TASK_QTY	(sizeof(task_cfg_list)/sizeof(task_cfg_t))

/********************** internal functions declaration ***********************/

/********************** internal data definition *****************************/
const char *p_sys	= " Bare Metal - Event-Triggered Systems (ETS)";
const char *p_app	= " App - Model Integration - C codig";

/********************** external data declaration ****************************/
uint32_t g_app_cnt;
uint32_t g_app_runtime_us;

volatile uint32_t g_app_tick_cnt;

task_dta_t task_dta_list[TASK_QTY];

/********************** external functions definition ************************/
void app_init(void)
{
	uint32_t index;

	/* Print out: Application Initialized */
	LOGGER_INFO(" ");
	LOGGER_INFO("%s is running - Tick [mS] = %lu", GET_NAME(app_init), HAL_GetTick());

	LOGGER_INFO(p_sys);
	LOGGER_INFO(p_app);

	/* Init & Print out: Application execution counter */
	g_app_cnt = G_APP_CNT_INI;
	LOGGER_INFO(" %s = %lu", GET_NAME(g_app_cnt), g_app_cnt);

	/* Init Cycle Counter */
	cycle_counter_init();

    /* Go through the task arrays */
	for (index = 0; TASK_QTY > index; index++)
	{
		/* Run task_x_init */
		(*task_cfg_list[index].task_init)(task_cfg_list[index].parameters);

		/* Init variables */
		task_dta_list[index].WCET = TASK_X_WCET_INI;
	}

	/* Protect shared resource */
	__asm("CPSID i");	/* disable interrupts */
	/* Init Tick Counter */
	g_app_tick_cnt = G_APP_TICK_CNT_INI;

	g_task_sensor_analog_tick_cnt = G_APP_TICK_CNT_INI;
	g_task_sensor_digital_tick_cnt = G_APP_TICK_CNT_INI;
	g_task_system_level_tanque_tick_cnt = G_APP_TICK_CNT_INI;
	g_task_system_riego_tick_cnt = G_APP_TICK_CNT_INI;
	g_task_system_ventilacion_tick_cnt = G_APP_TICK_CNT_INI;
	g_task_actuator_bomba_agua_tick_cnt = G_APP_TICK_CNT_INI;
	g_task_sensor_BLE_tick_cnt=G_APP_TICK_CNT_INI;
	g_task_actuator_LED_tick_cnt=G_APP_TICK_CNT_INI;
	g_task_system_LED_tick_cnt=G_APP_TICK_CNT_INI;
	g_task_system_memory_tick_cnt=G_APP_TICK_CNT_INI;
	g_task_actuator_cooler_tick_cnt=G_APP_TICK_CNT_INI;
    __asm("CPSIE i");	/* enable interrupts */
}

void app_update(void)
{
	uint32_t index;
	bool b_time_update_required = false;
	uint32_t cycle_counter_time_us;

	/* Protect shared resource */
	__asm("CPSID i");	/* disable interrupts */
    if (G_APP_TICK_CNT_INI < g_app_tick_cnt)
    {
		/* Update Tick Counter */
    	g_app_tick_cnt--;
    	b_time_update_required = true;
    }
    __asm("CPSIE i");	/* enable interrupts */

	/* Check if it's time to run tasks */
    while (b_time_update_required)
    {
    	/* Update App Counter */
    	g_app_cnt++;
    	g_app_runtime_us = 0;

		/* Go through the task arrays */
		for (index = 0; TASK_QTY > index; index++)
		{
			cycle_counter_reset();

    		/* Run task_x_update */
			(*task_cfg_list[index].task_update)(task_cfg_list[index].parameters);

			cycle_counter_time_us = cycle_counter_get_time_us();

			/* Update variables */
			g_app_runtime_us += cycle_counter_time_us;

			if (task_dta_list[index].WCET < cycle_counter_time_us)
			{
				task_dta_list[index].WCET = cycle_counter_time_us;
			}
		}

		/* Protect shared resource */
		__asm("CPSID i");	/* disable interrupts */
		if (G_APP_TICK_CNT_INI < g_app_tick_cnt)
		{
			/* Update Tick Counter */
			g_app_tick_cnt--;
			b_time_update_required = true;
		}
		else
		{
			b_time_update_required = false;
		}
		__asm("CPSIE i");	/* enable interrupts */
	}
}

void HAL_SYSTICK_Callback(void)
{
	/* Update Tick Counter */
	g_app_tick_cnt++;
	g_task_sensor_analog_tick_cnt++;
	g_task_sensor_digital_tick_cnt++;
	g_task_system_level_tanque_tick_cnt++;
	g_task_system_riego_tick_cnt++;
	g_task_system_ventilacion_tick_cnt++;
	g_task_actuator_bomba_agua_tick_cnt++;
	g_task_actuator_LED_tick_cnt++;
	g_task_sensor_BLE_tick_cnt++;
	g_task_system_LED_tick_cnt++;
	g_task_system_memory_tick_cnt++;
	g_task_actuator_cooler_tick_cnt++;
}

/********************** end of file ******************************************/
