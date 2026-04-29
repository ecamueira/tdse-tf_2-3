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
#include "task_actuator_interface_LED.h"

/********************** macros and definitions *******************************/
#define G_TASK_ACT_LED_CNT_INIT			0ul
#define G_TASK_ACT_LED_TICK_CNT_INI		0ul
#define PWM_PERIOD_VAL                  1000
#define INTENSITY_OFF                   0

/********************** internal data declaration ****************************/
extern TIM_HandleTypeDef htim1;
extern TIM_HandleTypeDef htim4;

const task_actuator_LED_cfg_t task_actuator_LED_cfg =
	{
		ID_LED_1,      // identifier
		&htim4,        // htim_r (Mismo timer)
		&htim1,        // htim_g (Mismo timer)
		&htim1,        // htim_b (Mismo timer)
		TIM_CHANNEL_1, // channel_r
		TIM_CHANNEL_3, // channel_g
		TIM_CHANNEL_2, // channel_b
	};

#define ACTUATOR_LED_CFG_QTY	(sizeof(task_actuator_LED_cfg)/sizeof(task_actuator_LED_cfg_t))

task_actuator_LED_dta_t task_actuator_LED_dta=
	{ST_LED_OFF, EV_LED_OFF, false, 0};

#define ACTUATOR_LED_DTA_QTY	(sizeof(task_actuator_LED_dta)/sizeof(task_actuator_LED_dta_t))

/********************** internal functions declaration ***********************/
void task_actuator_LED_statechart(void);
static void update_led_hw(const task_actuator_LED_cfg_t *c, uint16_t r, uint16_t g, uint16_t b);
void setPWM(TIM_HandleTypeDef timer,
            uint32_t channel,
            uint16_t period,
            uint16_t pulse);
/********************** internal data definition *****************************/
const char *p_task_actuator_LED 		= "Task Actuator (Actuator Statechart)";
const char *p_task_actuator_LED_ 		= "Non-Blocking & Update By Time Code";

/********************** external data declaration ****************************/
uint32_t g_task_actuator_LED_cnt;
volatile uint32_t g_task_actuator_LED_tick_cnt;

/********************** external functions definition ************************/
void task_actuator_LED_init(void *parameters)
{
	uint32_t index;
	const task_actuator_LED_cfg_t *p_task_actuator_LED_cfg;
	task_actuator_LED_dta_t *p_task_actuator_LED_dta;

	/* Print out: Task Initialized */
	LOGGER_INFO(" ");
	LOGGER_INFO("  %s is running - %s", GET_NAME(task_actuator_LED_init), p_task_actuator_LED);
	LOGGER_INFO("  %s is a %s", GET_NAME(task_actuator_LED), p_task_actuator_LED_);

	/* Init & Print out: Task execution counter */
	g_task_actuator_LED_cnt = G_TASK_ACT_LED_CNT_INIT;
	LOGGER_INFO("   %s = %lu", GET_NAME(g_task_actuator_LED_cnt), g_task_actuator_LED_cnt);

	for (index = 0; ACTUATOR_LED_DTA_QTY > index; index++)
	{
		/* Update Task Actuator Configuration & Data Pointer */
		p_task_actuator_LED_cfg = &task_actuator_LED_cfg;
		p_task_actuator_LED_dta = &task_actuator_LED_dta;

		HAL_TIM_PWM_Start(p_task_actuator_LED_cfg->htim_r, p_task_actuator_LED_cfg->channel_r);
		HAL_TIM_PWM_Start(p_task_actuator_LED_cfg->htim_g, p_task_actuator_LED_cfg->channel_g);
		HAL_TIM_PWM_Start(p_task_actuator_LED_cfg->htim_b, p_task_actuator_LED_cfg->channel_b);
		__HAL_TIM_SET_COMPARE(p_task_actuator_LED_cfg->htim_r, p_task_actuator_LED_cfg->channel_r, p_task_actuator_LED_dta->intensity);
		__HAL_TIM_SET_COMPARE(p_task_actuator_LED_cfg->htim_g, p_task_actuator_LED_cfg->channel_g, p_task_actuator_LED_dta->intensity);
		__HAL_TIM_SET_COMPARE(p_task_actuator_LED_cfg->htim_b, p_task_actuator_LED_cfg->channel_b, p_task_actuator_LED_dta->intensity);
	}
}

void task_actuator_LED_update(void *parameters)
{
	bool b_time_update_required = false;

	/* Protect shared resource */
	__asm("CPSID i");	/* disable interrupts*/
    if (G_TASK_ACT_LED_TICK_CNT_INI < g_task_actuator_LED_tick_cnt)
    {
		/* Update Tick Counter */
    	g_task_actuator_LED_tick_cnt--;
    	b_time_update_required = true;
    }
    __asm("CPSIE i");	/* enable interrupts */

    while (b_time_update_required)
    {
		/* Update Task Counter */
		g_task_actuator_LED_cnt++;

		/* Run Task Statechart */
    	task_actuator_LED_statechart();

    	/* Protect shared resource */
		__asm("CPSID i");	/* disable interrupts */
		if (G_TASK_ACT_LED_TICK_CNT_INI < g_task_actuator_LED_tick_cnt)
		{
			/* Update Tick Counter */
			g_task_actuator_LED_tick_cnt--;
			b_time_update_required = true;
		}
		else
		{
			b_time_update_required = false;
		}
		__asm("CPSIE i");	/* enable interrupts */
    }
}

void task_actuator_LED_statechart()
{
	const task_actuator_LED_cfg_t *p_task_actuator_LED_cfg;
	task_actuator_LED_dta_t *p_task_actuator_LED_dta;
		/* Update Task Actuator Configuration & Data Pointer */
		p_task_actuator_LED_cfg = &task_actuator_LED_cfg;
		p_task_actuator_LED_dta = &task_actuator_LED_dta;

		switch (p_task_actuator_LED_dta->state)
		{
			case ST_LED_OFF:

				//PWM configuration
				update_led_hw(p_task_actuator_LED_cfg, INTENSITY_OFF, INTENSITY_OFF, INTENSITY_OFF);
				if ((true == p_task_actuator_LED_dta->flag) && (EV_LED_RED_ON == p_task_actuator_LED_dta->event))
				{
					p_task_actuator_LED_dta->flag = false;
					p_task_actuator_LED_dta->state = ST_LED_RED_ON;
				}
				else if((true == p_task_actuator_LED_dta->flag) && (EV_LED_BLUE_ON == p_task_actuator_LED_dta->event))
				{
					p_task_actuator_LED_dta->flag = false;
					p_task_actuator_LED_dta->state = ST_LED_BLUE_ON;
				}

				else if((true == p_task_actuator_LED_dta->flag) && (EV_LED_PURPLE_ON == p_task_actuator_LED_dta->event))
				{
					p_task_actuator_LED_dta->flag = false;
					p_task_actuator_LED_dta->state = ST_LED_PURPLE_ON;
				}
				else if((true == p_task_actuator_LED_dta->flag) && (EV_LED_WHITE_ON == p_task_actuator_LED_dta->event))
				{
					p_task_actuator_LED_dta->flag = false;
					p_task_actuator_LED_dta->state = ST_LED_WHITE_ON;
				}
				else
									p_task_actuator_LED_dta->flag = false;
								break;

			case ST_LED_RED_ON:
				update_led_hw(p_task_actuator_LED_cfg, p_task_actuator_LED_dta->intensity, INTENSITY_OFF, INTENSITY_OFF);
				if ((true == p_task_actuator_LED_dta->flag) && (EV_LED_OFF == p_task_actuator_LED_dta->event))
				{
					p_task_actuator_LED_dta->flag = false;
					p_task_actuator_LED_dta->state = ST_LED_OFF;
				}
				else if((true == p_task_actuator_LED_dta->flag) && (EV_LED_BLUE_ON == p_task_actuator_LED_dta->event))
				{
					p_task_actuator_LED_dta->flag = false;
					p_task_actuator_LED_dta->state = ST_LED_BLUE_ON;
				}

				else if((true == p_task_actuator_LED_dta->flag) && (EV_LED_PURPLE_ON == p_task_actuator_LED_dta->event))
				{
					p_task_actuator_LED_dta->flag = false;
					p_task_actuator_LED_dta->state = ST_LED_PURPLE_ON;
				}
				else if((true == p_task_actuator_LED_dta->flag) && (EV_LED_WHITE_ON == p_task_actuator_LED_dta->event))
				{
					p_task_actuator_LED_dta->flag = false;
					p_task_actuator_LED_dta->state = ST_LED_WHITE_ON;
				}
				else
									p_task_actuator_LED_dta->flag = false;
				break;

			case ST_LED_BLUE_ON:
				update_led_hw(p_task_actuator_LED_cfg, INTENSITY_OFF, INTENSITY_OFF, p_task_actuator_LED_dta->intensity);
				if ((true == p_task_actuator_LED_dta->flag) && (EV_LED_OFF == p_task_actuator_LED_dta->event))
				{
					p_task_actuator_LED_dta->flag = false;
					p_task_actuator_LED_dta->state = ST_LED_OFF;
				}
				else if ((true == p_task_actuator_LED_dta->flag) && (EV_LED_RED_ON == p_task_actuator_LED_dta->event))
				{
					p_task_actuator_LED_dta->flag = false;
					p_task_actuator_LED_dta->state = ST_LED_RED_ON;
				}
				else if((true == p_task_actuator_LED_dta->flag) && (EV_LED_PURPLE_ON == p_task_actuator_LED_dta->event))
				{
					p_task_actuator_LED_dta->flag = false;
					p_task_actuator_LED_dta->state = ST_LED_PURPLE_ON;
				}
				else if((true == p_task_actuator_LED_dta->flag) && (EV_LED_WHITE_ON == p_task_actuator_LED_dta->event))
				{
					p_task_actuator_LED_dta->flag = false;
					p_task_actuator_LED_dta->state = ST_LED_WHITE_ON;
				}
				else
					p_task_actuator_LED_dta->flag = false;
				break;


			case ST_LED_PURPLE_ON:
				update_led_hw(p_task_actuator_LED_cfg, p_task_actuator_LED_dta->intensity, INTENSITY_OFF, p_task_actuator_LED_dta->intensity);
				if ((true == p_task_actuator_LED_dta->flag) && (EV_LED_OFF == p_task_actuator_LED_dta->event))
				{
					p_task_actuator_LED_dta->flag = false;
					p_task_actuator_LED_dta->state = ST_LED_OFF;
				}
				else if ((true == p_task_actuator_LED_dta->flag) && (EV_LED_RED_ON == p_task_actuator_LED_dta->event))
				{
					p_task_actuator_LED_dta->flag = false;
					p_task_actuator_LED_dta->state = ST_LED_RED_ON;
				}
				else if((true == p_task_actuator_LED_dta->flag) && (EV_LED_BLUE_ON == p_task_actuator_LED_dta->event))
				{
					p_task_actuator_LED_dta->flag = false;
					p_task_actuator_LED_dta->state = ST_LED_BLUE_ON;
				}
				else if((true == p_task_actuator_LED_dta->flag) && (EV_LED_WHITE_ON == p_task_actuator_LED_dta->event))
				{
					p_task_actuator_LED_dta->flag = false;
					p_task_actuator_LED_dta->state = ST_LED_WHITE_ON;
				}
				else
					p_task_actuator_LED_dta->flag = false;
				break;

			case ST_LED_WHITE_ON:
				update_led_hw(p_task_actuator_LED_cfg, p_task_actuator_LED_dta->intensity, p_task_actuator_LED_dta->intensity, p_task_actuator_LED_dta->intensity);
				if ((true == p_task_actuator_LED_dta->flag) && (EV_LED_OFF == p_task_actuator_LED_dta->event))
				{
					p_task_actuator_LED_dta->flag = false;
					p_task_actuator_LED_dta->state = ST_LED_OFF;
				}
				else if ((true == p_task_actuator_LED_dta->flag) && (EV_LED_RED_ON == p_task_actuator_LED_dta->event))
				{
					p_task_actuator_LED_dta->flag = false;
					p_task_actuator_LED_dta->state = ST_LED_RED_ON;
				}
				else if((true == p_task_actuator_LED_dta->flag) && (EV_LED_BLUE_ON == p_task_actuator_LED_dta->event))
				{
					p_task_actuator_LED_dta->flag = false;
					p_task_actuator_LED_dta->state = ST_LED_BLUE_ON;
				}
				else if((true == p_task_actuator_LED_dta->flag) && (EV_LED_PURPLE_ON == p_task_actuator_LED_dta->event))
				{
					p_task_actuator_LED_dta->flag = false;
					p_task_actuator_LED_dta->state = ST_LED_PURPLE_ON;

				}
				else
					p_task_actuator_LED_dta->flag = false;
				break;


			default:

				p_task_actuator_LED_dta->state = ST_LED_OFF;
				p_task_actuator_LED_dta->event = EV_LED_OFF;
				p_task_actuator_LED_dta->flag = false;

				break;
		}

}
static void update_led_hw(const task_actuator_LED_cfg_t *c, uint16_t r, uint16_t g, uint16_t b)
{
    setPWM(*(c->htim_r), c->channel_r, PWM_PERIOD_VAL, r);
    setPWM(*(c->htim_g), c->channel_g, PWM_PERIOD_VAL, g);
    setPWM(*(c->htim_b), c->channel_b, PWM_PERIOD_VAL, b);
}

void setPWM(TIM_HandleTypeDef timer, uint32_t channel, uint16_t period, uint16_t pulse) {
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
