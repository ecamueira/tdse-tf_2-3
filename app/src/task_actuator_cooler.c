/********************** inclusions *******************************************/
/* Project includes */
#include "main.h"

/* Demo includes */
#include "logger.h"
#include "dwt.h"

/* Application & Tasks includes */
#include "board.h"
#include "app.h"
#include "task_actuator_cooler_attribute.h"
#include "task_actuator_cooler_interface.h"

/********************** macros and definitions *******************************/
#define G_TASK_ACT_COOLER_CNT_INIT			0ul
#define G_TASK_ACT_COOLER_TICK_CNT_INI		0ul

#define PWM_PERIOD_VAL                  1000
#define INTENSITY_COOLER_OFF               0
#define DEL_COOLER_MIN				0ul

/********************** internal data declaration ****************************/
extern TIM_HandleTypeDef htim2;


const task_actuator_cooler_cfg_t task_actuator_cooler_cfg =
	{
		ID_COOLER,
		&htim2,
		TIM_CHANNEL_1
	};

#define ACTUATOR_COOLER_CFG_QTY	(sizeof(task_actuator_cooler_cfg)/sizeof(task_actuator_cooler_cfg_t))

task_actuator_cooler_dta_t task_actuator_cooler_dta=
	{DEL_COOLER_MIN, ST_COOLER_OFF, false, 0};

#define ACTUATOR_COOLER_DTA_QTY	(sizeof(task_actuator_cooler_dta)/sizeof(task_actuator_cooler_dta_t))

/********************** internal functions declaration ***********************/
void task_actuator_cooler_statechart(void);

static void update_cooler_hw(const task_actuator_cooler_cfg_t *c, uint16_t r);

void set_cooler_PWM(TIM_HandleTypeDef timer,
            uint32_t channel,
            uint16_t period,
            uint16_t pulse);
/********************** internal data definition *****************************/
const char *p_task_actuator_cooler 		= "Task Actuator (Actuator Statechart)";
const char *p_task_actuator_cooler_ 		= "Non-Blocking & Update By Time Code";

/********************** external data declaration ****************************/
uint32_t g_task_actuator_cooler_cnt;
volatile uint32_t g_task_actuator_cooler_tick_cnt;

/********************** external functions definition ************************/
void task_actuator_cooler_init(void *parameters)
{
	/* Print out: Task Initialized */
	LOGGER_INFO(" ");
	LOGGER_INFO("  %s is running - %s", GET_NAME(task_actuator_cooler_init), p_task_actuator_cooler);
	LOGGER_INFO("  %s is a %s", GET_NAME(task_actuator_cooler), p_task_actuator_cooler_);

	/* Init & Print out: Task execution counter */
	g_task_actuator_cooler_cnt = G_TASK_ACT_COOLER_CNT_INIT;
	LOGGER_INFO("   %s = %lu", GET_NAME(g_task_actuator_cooler_cnt), g_task_actuator_cooler_cnt);
}

void task_actuator_cooler_update(void *parameters)
{
	bool b_time_update_required = false;

	/* Protect shared resource */
	__asm("CPSID i");	/* disable interrupts*/
    if (G_TASK_ACT_COOLER_TICK_CNT_INI < g_task_actuator_cooler_tick_cnt)
    {
		/* Update Tick Counter */
    	g_task_actuator_cooler_tick_cnt--;
    	b_time_update_required = true;
    }
    __asm("CPSIE i");	/* enable interrupts */

    while (b_time_update_required)
    {
		/* Update Task Counter */
		g_task_actuator_cooler_cnt++;

		/* Run Task Statechart */
    	task_actuator_cooler_statechart();

    	/* Protect shared resource */
		__asm("CPSID i");	/* disable interrupts */
		if (G_TASK_ACT_COOLER_TICK_CNT_INI < g_task_actuator_cooler_tick_cnt)
		{
			/* Update Tick Counter */
			g_task_actuator_cooler_tick_cnt--;
			b_time_update_required = true;
		}
		else
		{
			b_time_update_required = false;
		}
		__asm("CPSIE i");	/* enable interrupts */
    }
}

void task_actuator_cooler_statechart()
{
	const task_actuator_cooler_cfg_t *p_task_actuator_cooler_cfg;
	task_actuator_cooler_dta_t *p_task_actuator_cooler_dta;

		/* Update Task Actuator Configuration & Data Pointer */
		p_task_actuator_cooler_cfg = &task_actuator_cooler_cfg;
		p_task_actuator_cooler_dta = &task_actuator_cooler_dta;

		switch (p_task_actuator_cooler_dta->state)
		{
			case ST_COOLER_OFF:

				update_cooler_hw(p_task_actuator_cooler_cfg, INTENSITY_COOLER_OFF);

				if (true == p_task_actuator_cooler_dta->flag_activar_cooler)
				{
					p_task_actuator_cooler_dta->state = ST_COOLER_ON;
				}

			break;

			case ST_COOLER_ON:

				update_cooler_hw(p_task_actuator_cooler_cfg, p_task_actuator_cooler_dta->intensity);

				if (false == p_task_actuator_cooler_dta->flag_activar_cooler)
				{
					p_task_actuator_cooler_dta->state = ST_COOLER_OFF;
				}

				break;



			default:

				p_task_actuator_cooler_dta->tick  = DEL_COOLER_MIN;
				p_task_actuator_cooler_dta->state = ST_COOLER_OFF;
				p_task_actuator_cooler_dta->flag_activar_cooler= false;

				break;
		}

}


static void update_cooler_hw(const task_actuator_cooler_cfg_t *c, uint16_t intensity)
{
    set_cooler_PWM(*(c->htim), c->channel, PWM_PERIOD_VAL, intensity);

}


void set_cooler_PWM(TIM_HandleTypeDef timer, uint32_t channel, uint16_t period, uint16_t pulse) {
  HAL_TIM_PWM_Stop(&timer, channel);
  TIM_OC_InitTypeDef sConfigOC;
  timer.Init.Period = period;
  HAL_TIM_PWM_Init(&timer);
  sConfigOC.OCMode = TIM_OCMODE_PWM1;
  sConfigOC.Pulse = pulse;
  sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
  sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
  HAL_TIM_PWM_ConfigChannel(&timer, &sConfigOC, channel);
  HAL_TIM_PWM_Start(&timer, channel);
}

/********************** end of file ******************************************/
