/********************** inclusions *******************************************/
#include "main.h"
#include "logger.h"
#include "board.h"
#include "app.h"
#include "task_sensor_attribute_BLE.h"
#include "task_system_memory_interface.h"

#include <string.h> // Necesario para memcmp y memcpy

/********************** macros and definitions *******************************/
#define G_TASK_SEN_BLE_CNT_INIT         0ul
#define G_TASK_SEN_BLE_TICK_CNT_INI     0ul

/********************** internal data declaration ****************************/
extern UART_HandleTypeDef huart3;
void task_sensor_BLE_statechart(void);


static uint8_t rx_buffer[5] = {0};
static uint8_t backup_buffer[5] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF}; // Inicializado distinto para forzar primera lectura
volatile bool b_ble_rx_complete = false;
const task_sensor_BLE_cfg_t task_sensor_BLE_cfg_list[] = {
    {ID_CONFIG_APP, &huart3}
};

/* Lista de datos en RAM para otros módulos */
task_sensor_BLE_rx task_sensor_BLE_RAM_list[] = {{0, 0, 0, 0, 0}};

/* Estado de la tarea */
task_sensor_BLE_dta_t task_sensor_BLE_dta_list[] = {
    {ST_CHECK_BUFFER}
};

#define SENSOR_BLE_DTA_QTY (sizeof(task_sensor_BLE_dta_list)/sizeof(task_sensor_BLE_dta_t))

/********************** internal data definition *****************************/
const char *p_task_sensor_BLE   = "Task Sensor (BLE Config)";
const char *p_task_sensor_BLE_  = "Non-Blocking DMA Mode";

/********************** external data declaration ****************************/
uint32_t g_task_sensor_BLE_cnt;
volatile uint32_t g_task_sensor_BLE_tick_cnt;

/********************** external functions definition ************************/

void task_sensor_BLE_init(void *parameters)
{
    HAL_StatusTypeDef status;

    LOGGER_INFO(" ");
    LOGGER_INFO("  %s is running", GET_NAME(task_sensor_BLE_init));
	LOGGER_INFO("  %s is a %s", GET_NAME(task_sensor_BLE), p_task_sensor_BLE_);

    g_task_sensor_BLE_cnt = G_TASK_SEN_BLE_CNT_INIT;
	LOGGER_INFO("   %s = %lu", GET_NAME(g_task_sensor_BLE_cnt), g_task_sensor_BLE_cnt);

    /* Iniciamos el DMA en modo Circular (se reinicia solo al llegar a 5 bytes) */
    status = HAL_UART_Receive_DMA(&huart3, rx_buffer, 5);

    if (status != HAL_OK)
    {
        LOGGER_LOG("ERROR: DMA BLE no pudo iniciar. Codigo: %d", status);
    }
    else
    {
        LOGGER_INFO("DMA BLE iniciado correctamente.");
    }
}

void task_sensor_BLE_update(void *parameters)
{
    bool b_time_update_required = false;

    __asm("CPSID i");
    if (G_TASK_SEN_BLE_TICK_CNT_INI < g_task_sensor_BLE_tick_cnt)
    {
        g_task_sensor_BLE_tick_cnt--;
        b_time_update_required = true;
    }
    __asm("CPSIE i");

    while (b_time_update_required)
    {
        task_sensor_BLE_statechart();
        g_task_sensor_BLE_cnt++;

        __asm("CPSID i");
        if (G_TASK_SEN_BLE_TICK_CNT_INI < g_task_sensor_BLE_tick_cnt)
        {
            g_task_sensor_BLE_tick_cnt--;
            b_time_update_required = true;
        }
        else
        {
            b_time_update_required = false;
        }
        __asm("CPSIE i");
    }
}

void task_sensor_BLE_statechart(void)
{
    uint32_t index;
    task_sensor_BLE_dta_t *p_task_sensor_BLE_dta;
    task_sensor_BLE_rx *p_task_sensor_BLE_rx;
    bool flag_nuevos_datos=false;
    for (index = 0; index < SENSOR_BLE_DTA_QTY; index++)
    {
        p_task_sensor_BLE_dta = &task_sensor_BLE_dta_list[index];
        p_task_sensor_BLE_rx = &task_sensor_BLE_RAM_list[index];

        switch (p_task_sensor_BLE_dta->state)
        {
            case ST_CHECK_BUFFER:

            	if ( b_ble_rx_complete == true)
                {

                    if ((rx_buffer[0] <= 4)   && (rx_buffer[1] <= 100) &&
                        (rx_buffer[2] <= 100) && (rx_buffer[3] <= 100) &&
                        (rx_buffer[4] <= 100))
                    {
                    	if(memcmp(rx_buffer, backup_buffer, 5) != 0)
                    	{
                    	flag_nuevos_datos=false;
                        p_task_sensor_BLE_dta->state=ST_WRITE_MEMORY;
                    	}
                    }
                    else
                    {
                    	p_task_sensor_BLE_dta->state=ST_BLE_FALLO;
                    }

                    b_ble_rx_complete = false;
                }
                break;
            case ST_WRITE_MEMORY:

            	p_task_sensor_BLE_rx->COLOR_LED              = rx_buffer[0];
            	p_task_sensor_BLE_rx->intensidad_LED         = rx_buffer[1];
            	p_task_sensor_BLE_rx->hum_tierra_preferida   = rx_buffer[2];
            	p_task_sensor_BLE_rx->hum_aire_preferida     = rx_buffer[3];
            	p_task_sensor_BLE_rx->temp_aire_preferida    = rx_buffer[4];
            	memcpy(backup_buffer, rx_buffer, 5);
            	flag_nuevos_datos=true;
            	put_system_memory_user_config(p_task_sensor_BLE_rx->COLOR_LED, p_task_sensor_BLE_rx->intensidad_LED, p_task_sensor_BLE_rx->temp_aire_preferida,  p_task_sensor_BLE_rx->hum_aire_preferida, p_task_sensor_BLE_rx->hum_tierra_preferida);
            	put_system_memory_user_config_flag(flag_nuevos_datos);
				p_task_sensor_BLE_dta->state=ST_CHECK_BUFFER;

            	break;
            case ST_BLE_FALLO:
            	if ( b_ble_rx_complete == true)
            	{
            		if(memcmp(rx_buffer, backup_buffer, 5) != 0)
            		{
            			if ((rx_buffer[0] <= 4)   && (rx_buffer[1] <= 100) && (rx_buffer[2] <= 100) && (rx_buffer[3] <= 100) && (rx_buffer[4] <= 100))
            			{
            				flag_nuevos_datos=false;
            				p_task_sensor_BLE_dta->state=ST_WRITE_MEMORY;
            			}
            		}
            		 b_ble_rx_complete = false;
            	}
            		break;

            default:
                p_task_sensor_BLE_dta->state = ST_CHECK_BUFFER;
                break;
        }
    }
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart) {
    if (huart->Instance == USART3) {
        b_ble_rx_complete = true;
    }
}
