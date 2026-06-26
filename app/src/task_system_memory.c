/********************** inclusions *******************************************/
/* Project includes */
#include <task_system_memory_attribute.h>
#include <task_system_memory_interface.h>
#include "main.h"
#include "stm32f1xx_hal.h"

/* Demo includes */
#include "logger.h"

/* Application & Tasks includes */
#include "board.h"
#include "app.h"
#include "task_system_LED_interface.h"
#include "task_system_riego_interface.h"
#include "task_system_ventilacion_interface.h"

/********************** macros and definitions *******************************/
#define G_TASK_SYS_MEMORY_CNT_INI			0ul
#define G_TASK_SYS_MEMORY_TICK_CNT_INI		0ul

#define DEL_SYS_MEMORY_MIN					0ul
#define EEPROM_WRITE_TIME_MS				50ul // Tiempo de escritura interno de la AT24C256

#define AT24C256_ADDR           0xA0
#define EEPROM_STORAGE_ADDR     0x0000
#define DATA_SIZE               5

/********************** internal data declaration ****************************/
task_system_memory_dta_t task_system_memory_dta = {DEL_SYS_MEMORY_MIN, ST_SYS_MEMORY_WAIT_READ,false,0, 0, 0, 0, 0};

#define SYSTEM_DTA_QTY	(sizeof(task_system_memory_dta)/sizeof(task_system_memory_dta_t))
volatile bool b_i2c_ready = true;


/********************** internal functions declaration ***********************/
void task_system_memory_statechart(void);
static void write_eeprom(task_system_memory_dta_t *p_task_system_memory_dta);
static void read_eeprom(task_system_memory_dta_t *p_dta, uint8_t *read_buffer);

void HAL_I2C_MemTxCpltCallback(I2C_HandleTypeDef *hi2c) {
    if (hi2c->Instance == I2C2) b_i2c_ready = true;
}
void HAL_I2C_MemRxCpltCallback(I2C_HandleTypeDef *hi2c) {
    if (hi2c->Instance == I2C2) b_i2c_ready = true;
}

/********************** internal data definition *****************************/
const char *p_task_system_memory 		= "Task System Memory(System Statechart)";
const char *p_task_system_memory_ 		= "Non-Blocking & Update By Time Code";
static uint8_t read_buffer[DATA_SIZE];


/********************** external data declaration ****************************/
uint32_t g_task_system_memory_cnt;
volatile uint32_t g_task_system_memory_tick_cnt;
extern I2C_HandleTypeDef hi2c2;

/********************** external functions definition ************************/

void task_system_memory_init(void *parameters)
{
	task_system_memory_dta_t 	*p_task_system_memory_dta;

	/* Update Task Actuator Configuration & Data Pointer */
	p_task_system_memory_dta = &task_system_memory_dta;

	/* Print out: Task Initialized */
	LOGGER_INFO(" ");
	LOGGER_INFO("  %s is running - %s", GET_NAME(task_system_memory_init), p_task_system_memory);
	LOGGER_INFO("  %s is a %s", GET_NAME(task_system_memory), p_task_system_memory_);

	/* Init & Print out: Task execution counter */
	g_task_system_memory_cnt = G_TASK_SYS_MEMORY_CNT_INI;
	LOGGER_INFO("   %s = %lu", GET_NAME(g_task_system_memory_cnt), g_task_system_memory_cnt);

	/* 1. Intentamos leer la EEPROM apenas arranca el micro */
	read_eeprom(p_task_system_memory_dta, read_buffer);
}

void task_system_memory_update(void *parameters)
{
	bool b_time_update_required = false;

	/* Protect shared resource */
	__asm("CPSID i");	/* disable interrupts */
    if (G_TASK_SYS_MEMORY_TICK_CNT_INI < g_task_system_memory_tick_cnt)
    {
		/* Update Tick Counter */
    	g_task_system_memory_tick_cnt--;
    	b_time_update_required = true;
    }
    __asm("CPSIE i");	/* enable interrupts */

    while (b_time_update_required)
        {
    		g_task_system_memory_cnt++;
        	task_system_memory_statechart();

    		__asm("CPSID i");	/* disable interrupts */
    		if (G_TASK_SYS_MEMORY_TICK_CNT_INI < g_task_system_memory_tick_cnt)
    		{
    			g_task_system_memory_tick_cnt--;
    			b_time_update_required = true;
    		}
    		else
    		{
    			b_time_update_required = false;
    		}
    		__asm("CPSIE i");	/* enable interrupts */
        }
}

void task_system_memory_statechart(void)
{
    task_system_memory_dta_t *p_task_system_memory_dta = &task_system_memory_dta;

    bool nueva_config=false;
    switch (p_task_system_memory_dta->state)
    {
        case ST_SYS_MEMORY_IDLE:
            if (p_task_system_memory_dta->flag == true && b_i2c_ready == true)
            {
                 write_eeprom(p_task_system_memory_dta);
                 p_task_system_memory_dta->tick = EEPROM_WRITE_TIME_MS;
                 p_task_system_memory_dta->state = ST_SYS_MEMORY_WAIT_WRITE;
                 p_task_system_memory_dta->flag = false;
            }
            break;

        case ST_SYS_MEMORY_WAIT_WRITE:
            if (p_task_system_memory_dta->tick > 0)
            {
                p_task_system_memory_dta->tick--;
            }
            else
            {
            	 p_task_system_memory_dta->tick=EEPROM_WRITE_TIME_MS;
                if (b_i2c_ready == true) {
                    p_task_system_memory_dta->state = ST_SYS_MEMORY_WAIT_READ;
                    read_eeprom(p_task_system_memory_dta, read_buffer);
                }
            }
            break;
        case ST_SYS_MEMORY_WAIT_READ:

        	if (p_task_system_memory_dta->tick > 0)
        	            {
        	                p_task_system_memory_dta->tick--;
        	            }
        	            else
        	            {
        	            	 p_task_system_memory_dta->tick=EEPROM_WRITE_TIME_MS;
          	                if (b_i2c_ready == true) {
        	                    p_task_system_memory_dta->state = ST_SYS_MEMORY_MANDAR_DATO;
        	                    p_task_system_memory_dta->color_led      = read_buffer[0];
        	                    p_task_system_memory_dta->intensidad_led = read_buffer[1];
        	                    p_task_system_memory_dta->humedad_suelo  = read_buffer[2];
        	                    p_task_system_memory_dta->humedad_aire   = read_buffer[3];
        	                    p_task_system_memory_dta->temperatura    = read_buffer[4];
        	                }
        	            }

        	break;

        case ST_SYS_MEMORY_MANDAR_DATO:

            	nueva_config=true;
				/* Distribución de datos a los otros módulos */
				 if (( p_task_system_memory_dta->color_led<= 4)   && ( p_task_system_memory_dta->intensidad_led <= 100) &&
										( p_task_system_memory_dta->humedad_suelo <= 100) && ( p_task_system_memory_dta->humedad_aire  <= 100) &&
										(p_task_system_memory_dta->temperatura <= 100))
				 {
					 put_riego_humedad_ambiente_value_min(p_task_system_memory_dta->humedad_aire);
					 put_riego_temperatura_ambiente_value_max(p_task_system_memory_dta->temperatura);
					 put_ventilacion_humedad_ambiente_value_min(p_task_system_memory_dta->humedad_aire);
					 put_ventilacion_temperatura_ambiente_value_max(p_task_system_memory_dta->temperatura);
					 put_humedad_suelo_value_min(p_task_system_memory_dta->humedad_suelo);
					 put_color_value(p_task_system_memory_dta->color_led);
					 put_intensidad_value(p_task_system_memory_dta->intensidad_led);
					 put_flag_new_data(nueva_config);
				 }
				else
					{
						 put_riego_humedad_ambiente_value_min(30);
						 put_riego_temperatura_ambiente_value_max(35);
						 put_ventilacion_humedad_ambiente_value_min(30);
						 put_ventilacion_temperatura_ambiente_value_max(35);
						 put_humedad_suelo_value_min(40);
						 put_color_value(0);
						 put_intensidad_value(50);
						 put_flag_new_data(nueva_config);
					}
            	nueva_config=false;
            	p_task_system_memory_dta->state = ST_SYS_MEMORY_IDLE;

            break;

        default:
            p_task_system_memory_dta->state = ST_SYS_MEMORY_WAIT_READ;
            p_task_system_memory_dta->flag=false;
            break;}
}


/********************** hardware internal functions **************************/

static void write_eeprom(task_system_memory_dta_t *p_dta) {

	if (b_i2c_ready == false) {
	        return;
	    }
	static uint8_t write_buffer[DATA_SIZE];

    write_buffer[0] = p_dta->color_led;
    write_buffer[1] = p_dta->intensidad_led;
    write_buffer[2] = p_dta->humedad_suelo;
    write_buffer[3] = p_dta->humedad_aire;
    write_buffer[4] = p_dta->temperatura;
    b_i2c_ready = false;
    HAL_I2C_Mem_Write_IT(&hi2c2, AT24C256_ADDR, EEPROM_STORAGE_ADDR, I2C_MEMADD_SIZE_16BIT, write_buffer, DATA_SIZE);
}

static void read_eeprom(task_system_memory_dta_t *p_dta, uint8_t *read_buffer) {
    b_i2c_ready = false;
    HAL_I2C_Mem_Read_IT(&hi2c2, AT24C256_ADDR, EEPROM_STORAGE_ADDR, I2C_MEMADD_SIZE_16BIT, read_buffer, DATA_SIZE);
}

/********************** end of file ******************************************/
