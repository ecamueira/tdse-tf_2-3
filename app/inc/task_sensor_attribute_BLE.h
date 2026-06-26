

#ifndef TASK_INC_TASK_SENSOR_BLE_ATTRIBUTE_H_
#define TASK_INC_TASK_SENSOR_BLE_ATTRIBUTE_H_

/********************** CPP guard ********************************************/
#ifdef __cplusplus
extern "C" {
#endif

/********************** inclusions *******************************************/

/********************** macros ***********************************************/

/********************** typedef **********************************************/
/* BLE Sensor Statechart - State Transition Table */
/* ------------------------+-----------------------+-----------------------+-----------------------+------------------------
 * | Current               | Event                 |                       | Next                  |                       |
 * | State                 | (Parameters)          | [Guard]               | State                 | Actions               |
 * |=======================+=======================+=======================+=======================+=======================|
 * | ST_CHECK_BUFFER       | b_ble_rx_complete     | [valid protocol &     | ST_WRITE_MEMORY       | flag_nuevos_datos = F |
 * |                       | == true               |  new data]            |                       | b_ble_rx_complete = F |
 * |                       +-----------------------+-----------------------+-----------------------+-----------------------|
 * |                       | b_ble_rx_complete     | [invalid protocol]    | ST_BLE_FALLO          | b_ble_rx_complete = F |
 * |                       | == true               |                       |                       |                       |
 * |-----------------------+-----------------------+-----------------------+-----------------------+-----------------------|
 * | ST_WRITE_MEMORY       | (Immediate)           |                       | ST_CHECK_BUFFER       | memcpy(backup)        |
 * |                       |                       |                       |                       | flag_nuevos_datos = T |
 * |                       |                       |                       |                       | put_sys_memory(...)   |
 * |-----------------------+-----------------------+-----------------------+-----------------------+-----------------------|
 * | ST_BLE_FALLO          | b_ble_rx_complete     | [valid protocol &     | ST_WRITE_MEMORY       | flag_nuevos_datos = F |
 * |                       | == true               |  new data]            |                       | b_ble_rx_complete = F |
 * |                       +-----------------------+-----------------------+-----------------------+-----------------------|
 * |                       | b_ble_rx_complete     | [ELSE]                | ST_BLE_FALLO          | b_ble_rx_complete = F |
 * |                       | == true               |                       |                       |                       |
 * ------------------------+-----------------------+-----------------------+-----------------------+------------------------ */
/* States of Task Sensor */
typedef enum task_sensor_BLE_st {ST_CHECK_BUFFER,
								ST_WRITE_MEMORY,
							    ST_BLE_FALLO} task_sensor_BLE_st_t;

/* Identifier of Task Sensor */
typedef enum task_sensor_BLE_id {ID_CONFIG_APP} task_sensor_BLE_id_t;

typedef struct
{
	uint8_t COLOR_LED;
	uint8_t intensidad_LED;
	uint8_t hum_tierra_preferida;
	uint8_t hum_aire_preferida;
	uint8_t temp_aire_preferida;
} task_sensor_BLE_rx;

typedef struct {
	task_sensor_BLE_id_t	identifier;
	UART_HandleTypeDef *huart;

} task_sensor_BLE_cfg_t;

typedef struct
{
	task_sensor_BLE_st_t	state;
} task_sensor_BLE_dta_t;




/********************** external data declaration ****************************/
extern task_sensor_BLE_dta_t task_sensor_BLE_dta_list[];
extern task_sensor_BLE_rx task_sensor_BLE_RAM_list[];
/********************** external functions declaration ***********************/

/********************** End of CPP guard *************************************/
#ifdef __cplusplus
}
#endif

#endif /* TASK_INC_TASK_SENSOR_ATTRIBUTE_H_ */

/********************** end of file ******************************************/
