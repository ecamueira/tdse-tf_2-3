
/********************** inclusions *******************************************/
/* Project includes */
#include <task_system_level_tanque_attribute.h>
#include <task_system_level_tanque_interface.h>
#include "main.h"
#include <math.h>


/* Demo includes */
#include "logger.h"

/* Application & Tasks includes */
#include "board.h"
#include "app.h"
#include "task_system_riego_interface.h"
#include "task_actuator_bomba_agua_interface.h"
/********************** macros and definitions *******************************/
#define G_TASK_SYS_LEVEL_TANQUE_CNT_INI			0ul
#define G_TASK_SYS_LEVEL_TANQUE_TICK_CNT_INI		0ul
#define DELTA_NIVEL_MAX  10

/********************** internal data declaration ****************************/
task_system_level_tanque_dta_t task_system_level_tanque_dta_list[] = {
	{ID_SYS_TANQUE_AGUA, ST_SYS_LEV_TAN_VACIO,0,0, false},
	{ID_SYS_TANQUE_FERTILIZANTE, ST_SYS_LEV_TAN_VACIO,0,0, false}
};

#define SYSTEM_LEVEL_TANQUE_DTA_QTY	(sizeof(task_system_level_tanque_dta_list)/sizeof(task_system_level_tanque_dta_t))

/********************** internal functions declaration ***********************/
void task_system_level_tanque_statechart(void);

/********************** internal data definition *****************************/
const char *p_task_system_level_tanque 		= "Task System (System LVL tanque Statechart)";
const char *p_task_system_level_tanque_ 		= "Non-Blocking & Update By Time Code";
uint16_t delta_medicion_nivel=0;
/********************** external data declaration ****************************/
uint32_t g_task_system_level_tanque_cnt;
volatile uint32_t g_task_system_level_tanque_tick_cnt;

/********************** external functions definition ************************/
void task_system_level_tanque_init(void *parameters)
{

	/* Print out: Task Initialized */
	LOGGER_INFO(" ");
	LOGGER_INFO("  %s is running - %s", GET_NAME(task_system_level_tanque_init), p_task_system_level_tanque);
	LOGGER_INFO("  %s is a %s", GET_NAME(task_system_level_tanque), p_task_system_level_tanque_);

	/* Init & Print out: Task execution counter */
	g_task_system_level_tanque_cnt = G_TASK_SYS_LEVEL_TANQUE_CNT_INI;
	LOGGER_INFO("   %s = %lu", GET_NAME(g_task_system_level_tanque_cnt), g_task_system_level_tanque_cnt);
}

void task_system_level_tanque_update(void *parameters)
{
	bool b_time_update_required = false;

	/* Protect shared resource */
	__asm("CPSID i");	/* disable interrupts */
    if (G_TASK_SYS_LEVEL_TANQUE_TICK_CNT_INI < g_task_system_level_tanque_tick_cnt)
    {
		/* Update Tick Counter */
    	g_task_system_level_tanque_tick_cnt--;
    	b_time_update_required = true;
    }
    __asm("CPSIE i");	/* enable interrupts */

    while (b_time_update_required)
    {
		/* Update Task Counter */
		g_task_system_level_tanque_cnt++;

		/* Run Task Statechart */
    	task_system_level_tanque_statechart();

    	/* Protect shared resource */
		__asm("CPSID i");	/* disable interrupts */
		if (G_TASK_SYS_LEVEL_TANQUE_TICK_CNT_INI < g_task_system_level_tanque_tick_cnt)
		{
			/* Update Tick Counter */
			g_task_system_level_tanque_tick_cnt--;
			b_time_update_required = true;
		}
		else
		{
			b_time_update_required = false;
		}
		__asm("CPSIE i");	/* enable interrupts */
    }
}

void task_system_level_tanque_statechart(void)
{
	uint32_t index;
	task_system_level_tanque_dta_t *p_task_system_level_tanque_dta;

	/* Update Task System Data Pointer */
	for(index = 0; index < SYSTEM_LEVEL_TANQUE_DTA_QTY; index++){
	p_task_system_level_tanque_dta = &task_system_level_tanque_dta_list[index];
	delta_medicion_nivel=fabs(p_task_system_level_tanque_dta->nivel_anterior-p_task_system_level_tanque_dta->nivel_actual);
	p_task_system_level_tanque_dta->nivel_anterior=p_task_system_level_tanque_dta->nivel_actual;
	switch (p_task_system_level_tanque_dta->state)
	{
		case ST_SYS_LEV_TAN_IDLE:

			if(delta_medicion_nivel<=DELTA_NIVEL_MAX && p_task_system_level_tanque_dta->nivel_actual<=100)
			{
				p_task_system_level_tanque_dta->flag=false;
				p_task_system_level_tanque_dta->state=ST_SYS_LEV_TAN_VACIO;
				if(p_task_system_level_tanque_dta->identifier==ID_SYS_TANQUE_AGUA)
				{
					put_level_tanque_agua_value(p_task_system_level_tanque_dta->flag);
				}
				if(p_task_system_level_tanque_dta->identifier==ID_SYS_TANQUE_FERTILIZANTE)
				{
					put_flag_task_actuator_bomba_fetilizante(p_task_system_level_tanque_dta->flag, 1);
				}
			}

			break;

		case ST_SYS_LEV_TAN_VACIO:

			if (delta_medicion_nivel<=DELTA_NIVEL_MAX && p_task_system_level_tanque_dta->nivel_actual>=200)
			{
				p_task_system_level_tanque_dta->flag=true;
				if(p_task_system_level_tanque_dta->identifier==ID_SYS_TANQUE_AGUA)
					{

					put_level_tanque_agua_value(p_task_system_level_tanque_dta->flag);
					}
				if(p_task_system_level_tanque_dta->identifier==ID_SYS_TANQUE_FERTILIZANTE)
				{
					put_flag_task_actuator_bomba_fetilizante(p_task_system_level_tanque_dta->flag, 1);
				}
				p_task_system_level_tanque_dta->state=ST_SYS_LEV_TAN_IDLE;
			}

			break;

		default:

			p_task_system_level_tanque_dta->state = ST_SYS_LEV_TAN_VACIO;
			p_task_system_level_tanque_dta->flag = false;

			break;
	}
	}
}

/********************** end of file ******************************************/
