
#ifndef TASK_INC_TASK_SYSTEM_MEMORY_ATTRIBUTE_H_
#define TASK_INC_TASK_SYSTEM_MEMORY_ATTRIBUTE_H_

/********************** CPP guard ********************************************/
#ifdef __cplusplus
extern "C" {
#endif

/********************** inclusions *******************************************/
#include <stdint.h>
#include <stdbool.h>
/********************** macros ***********************************************/

/********************** typedef **********************************************/
/* System Memory Statechart - State Transition Table */
/* ----------------------------+-----------------------+-----------------------+--------------------------+------------------------
 * | Current                   | Event                 |                       | Next                     |                       |
 * | State                     | (Parameters)          | [Guard]               | State                    | Actions               |
 * |===========================+=======================+=======================+=======================---+=======================|
 * | ST_SYS_MEMORY_IDLE        | flag == true          | [b_i2c_ready == T]    | ST_SYS_MEMORY_WAIT_WRITE | write_eeprom()        |
 * |                           |                       |                       |                          | tick = 50ms, flag = F |
 * |---------------------------+-----------------------+-----------------------+--------------------------+-----------------------|
 * | ST_SYS_MEMORY_WAIT_WRITE  | tick == 0             | [b_i2c_ready == T]    | ST_SYS_MEMORY_WAIT_READ  | read_eeprom()         |
 * |                           |                       |                       |                          | tick = 50ms           |
 * |                           +-----------------------+-----------------------+--------------------------+-----------------------|
 * |                           | tick > 0              |                       | ST_SYS_MEMORY_WAIT_WRITE | tick--                |
 * |---------------------------+-----------------------+-----------------------+--------------------------+-----------------------|
 * | ST_SYS_MEMORY_WAIT_READ   | tick == 0             | [b_i2c_ready == T]    | ST_SYS_MEMORY_MANDAR_DATO| Carga buffer a DTA    |
 * |                           |                       |                       |                          |                       |
 * |                           +-----------------------+-----------------------+--------------------------+-----------------------|
 * |                           | tick > 0              |                       | ST_SYS_MEMORY_WAIT_READ  | tick--                |
 * |---------------------------+-----------------------+-----------------------+--------------------------+-----------------------|
 * | ST_SYS_MEMORY_MANDAR_DATO | (Immediate)           | [Valid Range]         | ST_SYS_MEMORY_IDLE       | Distribuye config OK  |
 * |                           |                       | [Invalid Range]       | ST_SYS_MEMORY_IDLE       | Distribuye Defaults   |
 * |---------------------------+-----------------------+-----------------------+--------------------------+-----------------------|
 * | DEFAULT                   | Any                   |                       | ST_SYS_MEMORY_WAIT_READ  | flag = false          |
 * ----------------------------+-----------------------+-----------------------+--------------------------+------------------------ */

/* State of Task System */
typedef enum task_system_memory_st {
	ST_SYS_MEMORY_IDLE,
	    ST_SYS_MEMORY_ACTUALIZAR_DATO,
	    ST_SYS_MEMORY_WAIT_WRITE,
		ST_SYS_MEMORY_WAIT_READ,
	    ST_SYS_MEMORY_MANDAR_DATO
} task_system_memory_st_t;

typedef struct
{
	uint32_t			tick;
	task_system_memory_st_t	state;
	bool				flag;
	uint8_t                 color_led;
	uint8_t                 intensidad_led;
	uint8_t  			    humedad_suelo;
	uint8_t                 humedad_aire;
	uint8_t                 temperatura;

} task_system_memory_dta_t;

/********************** external data declaration ****************************/
extern task_system_memory_dta_t task_system_memory_dta;

/********************** external functions declaration ***********************/

/********************** End of CPP guard *************************************/
#ifdef __cplusplus
}
#endif

#endif /* TASK_INC_TASK_SYSTEM_MEMORY_ATTRIBUTE_H_ */

/********************** end of file ******************************************/
