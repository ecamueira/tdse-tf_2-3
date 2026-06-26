/********************** inclusions *******************************************/
/* Project includes */
#include "main.h"

/* Demo includes */
#include "logger.h"
#include "dwt.h"

/* Application & Tasks includes */
#include "board.h"
#include "app.h"
#include "task_actuator_bomba_agua_attribute.h"
#include "task_actuator_bomba_agua_interface.h"
#include "task_actuator_bomba_agua.h"


/********************** macros and definitions *******************************/
#define G_TASK_ACT_BOM_CNT_INIT			0ul
#define G_TASK_ACT_BOM_TICK_CNT_INI		0ul

#define DEL_BOM_MIN				  			0ul
#define TIEMPO_ESPERA_FERTILIZACION			86400000ul
#define TIEMPO_DOSIFICACION_FERTILIZACION	5000ul

/********************** internal data declaration ****************************/
const task_actuator_bomba_agua_cfg_t task_actuator_bomba_agua_cfg_list[]={
	{ID_BOMBA_AGUA,  GPIOB,  GPIO_PIN_0, GPIO_PIN_RESET,GPIO_PIN_SET},
	{ID_BOMBA_FERTILIZANTE,  GPIOC,  GPIO_PIN_1, GPIO_PIN_RESET,GPIO_PIN_SET}
};

#define ACTUATOR_BOM_CFG_QTY	(sizeof(task_actuator_bomba_agua_cfg_list)/sizeof(task_actuator_bomba_agua_cfg_t))

task_actuator_bomba_agua_dta_t task_actuator_bomba_agua_dta_list[]={
	{DEL_BOM_MIN, ST_BOMBA_OFF, false},
	{DEL_BOM_MIN, ST_BOMBA_OFF, false}
};

#define ACTUATOR_BOMBA_AGUA_DTA_QTY	(sizeof(task_actuator_bomba_agua_dta_list)/sizeof(task_actuator_bomba_agua_dta_t))

/********************** internal functions declaration ***********************/
void task_actuator_bomba_agua_statechart(void);

/********************** internal data definition *****************************/
const char *p_task_actuator_bomba_agua 		= "Task Actuator (Actuator Statechart)";
const char *p_task_actuator_bomba_agua_ 		= "Non-Blocking & Update By Time Code";

/********************** external data declaration ****************************/
uint32_t g_task_actuator_bomba_agua_cnt;
volatile uint32_t g_task_actuator_bomba_agua_tick_cnt;

/********************** external functions definition ************************/
void task_actuator_bomba_agua_init(void *parameters)
{
		uint32_t index;
		const task_actuator_bomba_agua_cfg_t *p_task_actuator_bomba_agua_cfg;

		/* Print out: Task Initialized */
		LOGGER_INFO(" ");
		LOGGER_INFO("  %s is running - %s", GET_NAME(task_actuator_bomba_agua_init), p_task_actuator_bomba_agua_);
		LOGGER_INFO("  %s is a %s", GET_NAME(task_actuator_bomba_agua), p_task_actuator_bomba_agua_);

		/* Init & Print out: Task execution counter */
		g_task_actuator_bomba_agua_cnt = G_TASK_ACT_BOM_CNT_INIT;
		LOGGER_INFO("   %s = %lu", GET_NAME(g_task_actuator_bomba_agua_cnt), g_task_actuator_bomba_agua_cnt);

		for (index = 0; ACTUATOR_BOMBA_AGUA_DTA_QTY > index; index++)
		{
			/* Update Task Actuator Configuration & Data Pointer */
			p_task_actuator_bomba_agua_cfg = &task_actuator_bomba_agua_cfg_list[index];
			HAL_GPIO_WritePin(p_task_actuator_bomba_agua_cfg->gpio_port, p_task_actuator_bomba_agua_cfg->pin, p_task_actuator_bomba_agua_cfg->bomba_off);
		}
}

void task_actuator_bomba_agua_update(void *parameters)
{
	bool b_time_update_required = false;

	/* Protect shared resource */
	__asm("CPSID i");	/* disable interrupts*/
    if (G_TASK_ACT_BOM_TICK_CNT_INI < g_task_actuator_bomba_agua_tick_cnt)
    {
		/* Update Tick Counter */
    	g_task_actuator_bomba_agua_tick_cnt--;
    	b_time_update_required = true;
    }
    __asm("CPSIE i");	/* enable interrupts */

    while (b_time_update_required)
    {
		/* Update Task Counter */
		g_task_actuator_bomba_agua_cnt++;

		/* Run Task Statechart */
    	task_actuator_bomba_agua_statechart();

    	/* Protect shared resource */
		__asm("CPSID i");	/* disable interrupts */
		if (G_TASK_ACT_BOM_TICK_CNT_INI < g_task_actuator_bomba_agua_tick_cnt)
		{
			/* Update Tick Counter */
			g_task_actuator_bomba_agua_tick_cnt--;
			b_time_update_required = true;
		}
		else
		{
			b_time_update_required = false;
		}
		__asm("CPSIE i");	/* enable interrupts */
    }
}

void task_actuator_bomba_agua_statechart(void)
{
	const task_actuator_bomba_agua_cfg_t *p_task_actuator_bomba_agua_cfg;
	task_actuator_bomba_agua_dta_t *p_task_actuator_bomba_agua_dta;
	uint8_t index;
		/* Update Task Actuator Configuration & Data Pointer */
	for(index = 0; ACTUATOR_BOMBA_AGUA_DTA_QTY > index; index++)
{
	p_task_actuator_bomba_agua_cfg = &task_actuator_bomba_agua_cfg_list[index];
	p_task_actuator_bomba_agua_dta = &task_actuator_bomba_agua_dta_list[index];

	switch (p_task_actuator_bomba_agua_dta->state)
	{
		case ST_BOMBA_OFF:

			if (p_task_actuator_bomba_agua_dta->flag_activar_bomba==true)
			{
				HAL_GPIO_WritePin(p_task_actuator_bomba_agua_cfg->gpio_port, p_task_actuator_bomba_agua_cfg->pin, p_task_actuator_bomba_agua_cfg->bomba_on);
				p_task_actuator_bomba_agua_dta->state = ST_BOMBA_ON;
				if(p_task_actuator_bomba_agua_cfg->identifier==ID_BOMBA_FERTILIZANTE)
				{
					p_task_actuator_bomba_agua_dta->tick=TIEMPO_DOSIFICACION_FERTILIZACION;
				}
			}
			break;

		case ST_BOMBA_ON:

			if (p_task_actuator_bomba_agua_dta->tick > 0)
					{
						p_task_actuator_bomba_agua_dta->tick--;
					}
			else if(p_task_actuator_bomba_agua_dta->tick == 0)
					{
						HAL_GPIO_WritePin(p_task_actuator_bomba_agua_cfg->gpio_port, p_task_actuator_bomba_agua_cfg->pin, p_task_actuator_bomba_agua_cfg->bomba_off);

						if(p_task_actuator_bomba_agua_cfg->identifier==ID_BOMBA_FERTILIZANTE)
						{
							p_task_actuator_bomba_agua_dta->state = ST_BOMBA_ESPERA_DIARIA;
							p_task_actuator_bomba_agua_dta->tick = TIEMPO_ESPERA_FERTILIZACION;
						}else
						{
							p_task_actuator_bomba_agua_dta->state = ST_BOMBA_OFF;
							p_task_actuator_bomba_agua_dta->tick = DEL_BOM_MIN;
							p_task_actuator_bomba_agua_dta->flag_activar_bomba=false;
						}
					}

				break;
		case ST_BOMBA_ESPERA_DIARIA:

			if (p_task_actuator_bomba_agua_dta->tick > 0)
			{
				p_task_actuator_bomba_agua_dta->tick--;
			}
			else if(p_task_actuator_bomba_agua_dta->tick == 0)
			{
				p_task_actuator_bomba_agua_dta->state = ST_BOMBA_OFF;
			}

				break;


			default:

				p_task_actuator_bomba_agua_dta->tick  = DEL_BOM_MIN;
				p_task_actuator_bomba_agua_dta->state = ST_BOMBA_OFF;
				p_task_actuator_bomba_agua_dta->flag_activar_bomba= false;

				break;
		}
	}
}

/********************** end of file ******************************************/
