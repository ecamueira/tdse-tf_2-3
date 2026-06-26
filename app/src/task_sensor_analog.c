
/********************** inclusions *******************************************/
/* Project includes */
#include <task_sensor_analog_attribute.h>
#include <task_system_riego_attribute.h>
#include <task_system_level_tanque_attribute.h>
#include "main.h"

/* Demo includes */
#include "logger.h"

/* Application & Tasks includes */
#include "board.h"
#include "app.h"
#include "task_system_riego_interface.h"
#include "task_system_level_tanque_interface.h"

/********************** macros and definitions *******************************/
#define G_TASK_SEN_ANALOG_CNT_INIT			0ul
#define G_TASK_SEN_ANALOG_TICK_CNT_INI		0ul
#define DEL_SEN_ADC_MIN				0ul
#define DEL_SEN_ADC_MAX				5ul
#define LEVEL_TANQUE_MAX			2300
#define LEVEL_TANQUE_MIN			0
#define HUM_SUELO_MAX				3500
#define HUM_SUELO_MIN				1000
/********************** internal data declaration ****************************/
const task_sensor_analog_cfg_t task_sensor_analog_cfg_list[] = {
	{ID_SEN_HUM_SUELO,  &hadc1, ADC_CHANNEL_0,HUM_SUELO_MAX,HUM_SUELO_MIN},
	{ID_SEN_NIVEL_AGUA,  &hadc1, ADC_CHANNEL_1,LEVEL_TANQUE_MAX,LEVEL_TANQUE_MIN},
	{ID_SEN_NIVEL_FERTILIZANTE,  &hadc1, ADC_CHANNEL_4,LEVEL_TANQUE_MAX,LEVEL_TANQUE_MIN}
};

#define SENSOR_ANALOG_CFG_QTY	(sizeof(task_sensor_analog_cfg_list)/sizeof(task_sensor_analog_cfg_t))

task_sensor_analog_dta_t task_sensor_analog_dta_list[] = {
	{DEL_SEN_ADC_MIN, ST_SEN_ADC_SETUP, 1500},
	{DEL_SEN_ADC_MIN, ST_SEN_ADC_SETUP, 0},
	{DEL_SEN_ADC_MIN,ST_SEN_ADC_SETUP, 0}
};

#define SENSOR_DTA_QTY	(sizeof(task_sensor_analog_dta_list)/sizeof(task_sensor_analog_dta_t))

/********************** internal functions declaration ***********************/
void task_sensor_analog_statechart(void);
void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc);
/********************** internal data definition *****************************/
const char *p_task_sensor_analog 		= "Task Sensor (Sensor Statechart)";
const char *p_task_sensor_analog_ 		= "Non-Blocking & Update By Time Code";

/********************** external data declaration ****************************/
uint32_t g_task_sensor_analog_cnt;
volatile uint32_t g_task_sensor_analog_tick_cnt;
/********************** external functions definition ************************/
void task_sensor_analog_init(void *parameters)
{
	/* Print out: Task Initialized */
	LOGGER_INFO(" ");
	LOGGER_INFO("  %s is running - %s", GET_NAME(task_sensor_analog_init), p_task_sensor_analog);
	LOGGER_INFO("  %s is a %s", GET_NAME(task_sensor_analog), p_task_sensor_analog_);

	/* Init & Print out: Task execution counter */
	g_task_sensor_analog_cnt = G_TASK_SEN_ANALOG_CNT_INIT;
	LOGGER_INFO("   %s = %lu", GET_NAME(g_task_sensor_analog_cnt), g_task_sensor_analog_cnt);
	HAL_ADC_Start(&hadc1);
}

void task_sensor_analog_update(void *parameters)
{
	bool b_time_update_required = false;

	/* Protect shared resource */
	__asm("CPSID i");	/* disable interrupts */
    if (G_TASK_SEN_ANALOG_TICK_CNT_INI < g_task_sensor_analog_tick_cnt)
    {
		/* Update Tick Counter */
    	g_task_sensor_analog_tick_cnt--;
    	b_time_update_required = true;
    }
    __asm("CPSIE i");	/* enable interrupts */

    while (b_time_update_required)
    {
		/* Update Task Counter */

    	task_sensor_analog_statechart();
    	g_task_sensor_analog_cnt++;

    	/* Protect shared resource */
		__asm("CPSID i");	/* disable interrupts */
		if (G_TASK_SEN_ANALOG_TICK_CNT_INI < g_task_sensor_analog_tick_cnt)
		{
			/* Update Tick Counter */
			g_task_sensor_analog_tick_cnt--;
			b_time_update_required = true;
		}
		else
		{
			b_time_update_required = false;
		}
		__asm("CPSIE i");	/* enable interrupts */
    }
}

void task_sensor_analog_statechart(void)
{
    uint32_t index;
    const task_sensor_analog_cfg_t *p_task_sensor_analog_cfg;

    task_sensor_analog_dta_t *p_task_sensor_analog_dta;


    ADC_TypeDef* adc = hadc1.Instance;

    for (index = 0; index < SENSOR_DTA_QTY; index++)
    {

    	/* 1. Cambio de canal */
        adc->SQR3 = task_sensor_analog_cfg_list[index].channel;

        /* 2. ESTABILIZACIÓN: Doble conversión (la primera se descarta) */
        // Primera conversión para "limpiar" el condensador de Sample & Hold
        adc->CR2 |= ADC_CR2_SWSTART;
        while (!(adc->SR & ADC_SR_EOC));
        (void)adc->DR; // Leemos y descartamos

        // Segunda conversión
        adc->CR2 |= ADC_CR2_SWSTART;
        while (!(adc->SR & ADC_SR_EOC));

        /* 3. Guardar resultado real */
        task_sensor_analog_dta_list[index].valor1 = (uint16_t)adc->DR;

        p_task_sensor_analog_cfg = &task_sensor_analog_cfg_list[index];
    	p_task_sensor_analog_dta = &task_sensor_analog_dta_list[index];

        switch (p_task_sensor_analog_dta->state)
        	{
        		case ST_SEN_ADC_SETUP:

        			if (p_task_sensor_analog_dta->valor1 >=p_task_sensor_analog_cfg->valor_min && p_task_sensor_analog_dta->valor1 <=p_task_sensor_analog_cfg->valor_max)
        			{
        				if(ID_SEN_HUM_SUELO == p_task_sensor_analog_cfg->identifier)
        				{
        					put_humedad_suelo_value((HUM_SUELO_MAX - p_task_sensor_analog_dta->valor1)*100/ (HUM_SUELO_MAX-HUM_SUELO_MIN) );
        				}else if(ID_SEN_NIVEL_AGUA == p_task_sensor_analog_cfg->identifier){
        					put_nivel_tanque_value(p_task_sensor_analog_dta->valor1, 0);
        				}else if(ID_SEN_NIVEL_FERTILIZANTE == p_task_sensor_analog_cfg->identifier){
        					put_nivel_tanque_value(p_task_sensor_analog_dta->valor1, 1);
        				}

        			}else{
        					p_task_sensor_analog_dta->state= ST_SEN_ADC_REVISION;
        					p_task_sensor_analog_dta->tick= DEL_SEN_ADC_MAX;
        			}

        			break;
        		case ST_SEN_ADC_REVISION:

        			if(p_task_sensor_analog_dta->tick > 0){
        					p_task_sensor_analog_dta->tick--;
        			}else if(p_task_sensor_analog_dta->valor1 >=p_task_sensor_analog_cfg->valor_min && p_task_sensor_analog_dta->valor1 <=p_task_sensor_analog_cfg->valor_max){
        					p_task_sensor_analog_dta->state= ST_SEN_ADC_SETUP;
        			}else{
        					p_task_sensor_analog_dta->state= ST_SEN_ADC_FALLO;
        			}
        			break;
        		case ST_SEN_ADC_FALLO:

        			if(p_task_sensor_analog_dta->valor1 >=p_task_sensor_analog_cfg->valor_min && p_task_sensor_analog_dta->valor1 <=p_task_sensor_analog_cfg->valor_max)
        				{
        					p_task_sensor_analog_dta->state= ST_SEN_ADC_REVISION;
        					p_task_sensor_analog_dta->tick= DEL_SEN_ADC_MAX;
        				}
        			break;


        		default:
        			p_task_sensor_analog_dta->tick  = DEL_SEN_ADC_MIN;
        			p_task_sensor_analog_dta->state = ST_SEN_ADC_SETUP;

        			break;
        	}
    }



}


/********************** end of file ******************************************/
