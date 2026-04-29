
/********************** inclusions *******************************************/
/* Project includes */
#include "main.h"
#include <math.h>
/* Demo includes */
#include "logger.h"
/* Application & Tasks includes */
#include "board.h"
#include "app.h"
#include "task_sensor_digital_attribute.h"
#include "task_system_riego_interface.h"
#include "task_system_ventilacion_interface.h"

/********************** macros and definitions *******************************/
#define G_TASK_SEN_DIG_CNT_INIT			0ul
#define G_TASK_SEN_DIG_TICK_CNT_INI		0ul
#define DEL_SEN_DIG_MIN				0ul
#define DEL_SEN_DIG_MAX				1000ul
#define HUM_MAX_DELTA 10.0f  // Máximo cambio de humedad permitido (10%) en 200ms
#define TEMP_MAX_DELTA 5.0f  // Máximo cambio de temperatura (5°C) en 200ms

uint8_t AHT10_RX_Data[6];
uint32_t AHT10_ADC_Raw;

float AHT10_Temperature;
float delta_temp;
float AHT10_Humidity;
float delta_hum;
uint8_t AHT10_TmpHum_Cmd[3] = {0xAC, 0x33, 0x00};

#define AHT10_ADRESS (0x38 << 1) // 0b1110000; Adress[7-bit]Wite/Read[1-bit]

uint8_t T_100ms = 255;
uint8_t AHT10_Switcher = 255;
bool data_ready_AHT10;
/********************** internal data declaration ****************************/
task_sensor_dig_dta_t task_sensor_dig_dta_list[] = {
	{DEL_SEN_DIG_MIN,ST_DIG_INIT, 0,0},
};

#define SENSOR_DIG_DTA_QTY	(sizeof(task_sensor_dig_dta_list)/sizeof(task_sensor_dig_dta_t))
/********************** internal functions declaration ***********************/
void task_sensor_digital_statechart(void);

/********************** internal data definition *****************************/
const char *p_task_sensor_dig 		= "Task Sensor (Sensor Statechart)";
const char *p_task_sensor_dig_ 		= "Non-Blocking & Update By Time Code";

/********************** external data declaration ****************************/
uint32_t g_task_sensor_digital_cnt;
volatile uint32_t g_task_sensor_digital_tick_cnt;

extern I2C_HandleTypeDef hi2c1;
extern TIM_HandleTypeDef htim3;

/********************** external functions definition ************************/
void task_sensor_digital_init(void *parameters)
{
	/* Print out: Task Initialized */
	LOGGER_INFO(" ");
	LOGGER_INFO("  %s is running - %s", GET_NAME(task_sensor_dig_init), p_task_sensor_dig);
	LOGGER_INFO("  %s is a %s", GET_NAME(task_sensor), p_task_sensor_dig_);

	/* Init & Print out: Task execution counter */
	g_task_sensor_digital_cnt = G_TASK_SEN_DIG_CNT_INIT;
	LOGGER_INFO("   %s = %lu", GET_NAME(g_task_sensor_digital_cnt), g_task_sensor_digital_cnt);

}

void task_sensor_digital_update(void *parameters)
{
	bool b_time_update_required = false;

	/* Protect shared resource */
	__asm("CPSID i");	/* disable interrupts */
    if (G_TASK_SEN_DIG_TICK_CNT_INI < g_task_sensor_digital_tick_cnt)
    {
		/* Update Tick Counter */
    	g_task_sensor_digital_tick_cnt--;
    	b_time_update_required = true;
    }
    __asm("CPSIE i");	/* enable interrupts */

    while (b_time_update_required)
    {

    	task_sensor_digital_statechart();
    	g_task_sensor_digital_cnt++;

    	/* Protect shared resource */
		__asm("CPSID i");	/* disable interrupts */
		if (G_TASK_SEN_DIG_TICK_CNT_INI < g_task_sensor_digital_tick_cnt)
		{
			/* Update Tick Counter */
			g_task_sensor_digital_tick_cnt--;
			b_time_update_required = true;
		}
		else
		{
			b_time_update_required = false;
		}
		__asm("CPSIE i");	/* enable interrupts */
    }
}

void task_sensor_digital_statechart(void)
{
	uint32_t index;

	task_sensor_dig_dta_t *p_task_sensor_dig_dta;

	for (index = 0; SENSOR_DIG_DTA_QTY > index; index++)
	{
		/* Update Task Sensor Configuration & Data Pointer */
		p_task_sensor_dig_dta = &task_sensor_dig_dta_list[index];

		if(T_100ms)
		{
			if(AHT10_Switcher)
			{
				HAL_I2C_Master_Transmit_IT(&hi2c1, AHT10_ADRESS, (uint8_t*)AHT10_TmpHum_Cmd, 3); /* Send command (trigger measuremetns) + parameters */
			}
			else
			{
				HAL_I2C_Master_Receive_IT(&hi2c1, AHT10_ADRESS, (uint8_t*)AHT10_RX_Data, 6); /* Receive data: STATUS[1]:HIMIDITY[2.5]:TEMPERATURE[2.5] */
			}

			if(~AHT10_RX_Data[0] & 0x80)
			{
				/* Convert to Temperature in °C */
				AHT10_ADC_Raw = (((uint32_t)AHT10_RX_Data[3] & 15) << 16) | ((uint32_t)AHT10_RX_Data[4] << 8) | AHT10_RX_Data[5];
				AHT10_Temperature = (float)(AHT10_ADC_Raw * 200.00 / 1048576.00) - 50.00;
				delta_temp=fabs(AHT10_Temperature-p_task_sensor_dig_dta->temperatura_anterior);
				/* Convert to Relative Humidity in % */
				AHT10_ADC_Raw = ((uint32_t)AHT10_RX_Data[1] << 12) | ((uint32_t)AHT10_RX_Data[2] << 4) | (AHT10_RX_Data[3] >> 4);
				AHT10_Humidity = (float)(AHT10_ADC_Raw*100.00/1048576.00);
				delta_hum=fabs(AHT10_Humidity-p_task_sensor_dig_dta->hum_ambiente_anterior);
				}
				data_ready_AHT10=true;
				AHT10_Switcher = ~AHT10_Switcher; /* Invert */
				T_100ms = 0; /* Nulify */
		}

		if(data_ready_AHT10)
		{
		switch (p_task_sensor_dig_dta->state)
		{
			case ST_DIG_INIT:
				if(AHT10_Humidity >= 0.0f && AHT10_Humidity <= 100.0f && AHT10_Temperature >= -40.0f && AHT10_Temperature <=85.0f){
				p_task_sensor_dig_dta->hum_ambiente_anterior=AHT10_Humidity;
				p_task_sensor_dig_dta->temperatura_anterior=AHT10_Temperature;
				delta_temp=fabs(AHT10_Temperature-p_task_sensor_dig_dta->temperatura_anterior);
				delta_hum=fabs(AHT10_Humidity-p_task_sensor_dig_dta->hum_ambiente_anterior);
				p_task_sensor_dig_dta->state = ST_DIG_SETUP;
				}
				break;

			case ST_DIG_SETUP:

				if (AHT10_Humidity >= 0.0f && AHT10_Humidity <= 100.0f && delta_hum < HUM_MAX_DELTA && delta_temp < TEMP_MAX_DELTA && AHT10_Temperature >= -40.0f && AHT10_Temperature <=85.0f) {
					p_task_sensor_dig_dta->hum_ambiente_anterior=p_task_sensor_dig_dta->hum_ambiente;
					p_task_sensor_dig_dta->temperatura_anterior=p_task_sensor_dig_dta->temperatura;
					p_task_sensor_dig_dta->hum_ambiente =AHT10_Humidity;
				    p_task_sensor_dig_dta->temperatura=AHT10_Temperature;
				    put_riego_humedad_temperatura_ambiente_value(p_task_sensor_dig_dta->hum_ambiente, p_task_sensor_dig_dta->temperatura);
				    put_ventilacion_humedad_temperatura_ambiente_value(p_task_sensor_dig_dta->hum_ambiente, p_task_sensor_dig_dta->temperatura);
				}
				else{
				    p_task_sensor_dig_dta->state = ST_DIG_REVISION;
				    p_task_sensor_dig_dta->tick = DEL_SEN_DIG_MAX;
				    }

				break;

			case ST_DIG_REVISION:
    			if(p_task_sensor_dig_dta->tick> 0){
    					p_task_sensor_dig_dta->tick--;
    			}else if(AHT10_Humidity >= 0.0f && AHT10_Humidity <= 100.0f && delta_hum < HUM_MAX_DELTA && delta_temp < TEMP_MAX_DELTA && AHT10_Temperature >= -40.0f && AHT10_Temperature <=85.0f){
    					p_task_sensor_dig_dta->state= ST_DIG_SETUP;
    			}else{
    					p_task_sensor_dig_dta->state= ST_DIG_FALLO;
    			}
				break;

			case ST_DIG_FALLO:
				if (AHT10_Humidity >= 0.0f && AHT10_Humidity <= 100.0f && delta_hum < HUM_MAX_DELTA && delta_temp < TEMP_MAX_DELTA && AHT10_Temperature >= -40.0f && AHT10_Temperature <=85.0f) {
				     p_task_sensor_dig_dta->state = ST_DIG_REVISION;
				     p_task_sensor_dig_dta->tick = DEL_SEN_DIG_MAX;
				}
				break;

			default:
				p_task_sensor_dig_dta->tick  = DEL_SEN_DIG_MIN;
				p_task_sensor_dig_dta->state = ST_DIG_INIT;
				break;
		}

		data_ready_AHT10=false;
		}

	}

}
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
 if(htim->Instance == TIM3)
 {
	/* Set every 100ms */
	T_100ms = 255;
 }
}

/********************** end of file ******************************************/
