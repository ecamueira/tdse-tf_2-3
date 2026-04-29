
/********************** inclusions *******************************************/
/* Project includes */
#include <task_system_LED_attribute.h>
#include <task_system_LED_interface.h>
#include "main.h"

/* Demo includes */
#include "logger.h"

/* Application & Tasks includes */
#include "board.h"
#include "app.h"
#include "task_actuator_attribute_LED.h"
#include "task_actuator_interface_LED.h"

/********************** macros and definitions *******************************/
#define G_TASK_SYS_LED_CNT_INI			0ul
#define G_TASK_SYS_LED_TICK_CNT_INI		0ul

#define COLOR_LED_BLANCO	1
#define COLOR_LED_ROJO	2
#define COLOR_LED_AZUL	3
#define COLOR_LED_VIOLETA	4
#define COLOR_LED_OFF	0

/********************** internal data declaration ****************************/
task_system_LED_dta_t task_system_LED_dta = {
	  ST_SYS_LED_IDLE, 0,0, false
};

/********************** internal functions declaration ***********************/
void task_system_LED_statechart(void);

/********************** internal data definition *****************************/
const char *p_task_system_LED 		= "Task System (System LED Statechart)";
const char *p_task_system_LED_ 		= "Non-Blocking & Update By Time Code";

/********************** external data declaration ****************************/
uint32_t g_task_system_LED_cnt;
volatile uint32_t g_task_system_LED_tick_cnt;

/********************** external functions definition ************************/
void task_system_LED_init(void *parameters)
{
	/* Print out: Task Initialized */
	LOGGER_INFO(" ");
	LOGGER_INFO("  %s is running - %s", GET_NAME(task_system_LED_init), p_task_system_LED);
	LOGGER_INFO("  %s is a %s", GET_NAME(task_system_LED), p_task_system_LED_);

	/* Init & Print out: Task execution counter */
	g_task_system_LED_cnt = G_TASK_SYS_LED_CNT_INI;
	LOGGER_INFO("   %s = %lu", GET_NAME(g_task_system_LED_cnt), g_task_system_LED_cnt);
}

void task_system_LED_update(void *parameters)
{
	bool b_time_update_required = false;

	/* Protect shared resource */
	__asm("CPSID i");	/* disable interrupts */
    if (G_TASK_SYS_LED_TICK_CNT_INI < g_task_system_LED_tick_cnt)
    {
		/* Update Tick Counter */
    	g_task_system_LED_tick_cnt--;
    	b_time_update_required = true;
    }
    __asm("CPSIE i");	/* enable interrupts */

    while (b_time_update_required)
    {
		/* Update Task Counter */
		g_task_system_LED_cnt++;

		/* Run Task Statechart */
    	task_system_LED_statechart();

    	/* Protect shared resource */
		__asm("CPSID i");	/* disable interrupts */
		if (G_TASK_SYS_LED_TICK_CNT_INI < g_task_system_LED_tick_cnt)
		{
			/* Update Tick Counter */
			g_task_system_LED_tick_cnt--;
			b_time_update_required = true;
		}
		else
		{
			b_time_update_required = false;
		}
		__asm("CPSIE i");	/* enable interrupts */
    }
}

void task_system_LED_statechart(void)
{
	task_system_LED_dta_t *p_task_system_LED_dta;

	/* Update Task System Data Pointer */
	p_task_system_LED_dta = &task_system_LED_dta;

	switch (p_task_system_LED_dta->state)
	{
		case ST_SYS_LED_IDLE:

			if ((true == p_task_system_LED_dta->flag))
			{
				p_task_system_LED_dta->flag = false;
				p_task_system_LED_dta->state = ST_SYS_LED_NEW_DATA;
			}
			break;

		case ST_SYS_LED_NEW_DATA:

			if ((p_task_system_LED_dta->color==COLOR_LED_OFF))
			{
				put_task_actuator_LED(EV_LED_OFF, p_task_system_LED_dta->intensidad);
				p_task_system_LED_dta->state = ST_SYS_LED_IDLE;
			}
			else if ((p_task_system_LED_dta->color==COLOR_LED_BLANCO))
			{
				put_task_actuator_LED(EV_LED_WHITE_ON,  p_task_system_LED_dta->intensidad);
				p_task_system_LED_dta->state = ST_SYS_LED_IDLE;
			}

			else if (((p_task_system_LED_dta->color==COLOR_LED_ROJO)))
			{
				put_task_actuator_LED(EV_LED_RED_ON, p_task_system_LED_dta->intensidad);
				p_task_system_LED_dta->state = ST_SYS_LED_IDLE;
			}
			else if (((p_task_system_LED_dta->color==COLOR_LED_AZUL)))
			{
				put_task_actuator_LED(EV_LED_BLUE_ON, p_task_system_LED_dta->intensidad);
				p_task_system_LED_dta->state = ST_SYS_LED_IDLE;
			}
			else if (((p_task_system_LED_dta->color==COLOR_LED_VIOLETA)))
			{
				put_task_actuator_LED(EV_LED_PURPLE_ON, p_task_system_LED_dta->intensidad);
				p_task_system_LED_dta->state = ST_SYS_LED_IDLE;
			}
			break;

		default:

			p_task_system_LED_dta->state = ST_SYS_LED_IDLE;
			p_task_system_LED_dta->flag = false;

			break;
	}
}

/********************** end of file ******************************************/
