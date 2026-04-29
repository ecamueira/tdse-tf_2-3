/********************** inclusions *******************************************/
/* Project includes */
#include <task_system_riego_attribute.h>
#include <task_system_riego_interface.h>
#include "main.h"

/* Demo includes */
#include "logger.h"

/* Application & Tasks includes */
#include "board.h"
#include "app.h"
#include "task_actuator_bomba_agua_attribute.h"
#include "task_actuator_bomba_agua_interface.h"

/********************** macros and definitions *******************************/
#define G_TASK_SYS_RIEGO_CNT_INI			0ul
#define G_TASK_SYS_RIEGO_TICK_CNT_INI		0ul

#define DEL_SYS_RIEGO_MIN					0ul
#define DEL_SYS_RIEGO_HA_MAX					60
#define DEL_SYS_RIEGO_HS_MIN					30
#define DEL_SYS_RIEGO_TA_MAX					35
#define DEL_SYS_RIEGO_HS_MAX_MEASURE			3300
#define MULTIPLICADOR_TICKS   250.0f   // Riego conservador e iterativo
#define MINIMO_TICKS_BOMBA    2000     // 2 seg (Aprox 60 ml) - Tiempo de cebado
#define MAXIMO_TICKS_BOMBA    12000    // 12 seg (Aprox 360 ml) - Límite anti-desborde
#define ESPERA_ABSORCION      30000    // 30 seg de pausa para que la tierra absorba

/********************** internal data declaration ****************************/
task_system_riego_dta_t task_system_riego_dta =
	{DEL_SYS_RIEGO_MIN, ST_SYS_RIEGO_NO_REGAR, false, 3300,0,0,DEL_SYS_RIEGO_HS_MIN,DEL_SYS_RIEGO_HA_MAX,DEL_SYS_RIEGO_TA_MAX};

#define SYSTEM_DTA_QTY	(sizeof(task_system_riego_dta)/sizeof(task_system_riego_dta_t))
float C_riego;
uint32_t ticks_calculados;
/********************** internal functions declaration ***********************/
void task_system_riego_statechart(void);
void calcular_coeficientes_riego(
		uint8_t S_act, float T_act, float H_act,
		uint8_t S_set, uint8_t T_set, uint8_t H_set,
    float* out_C_riego);
/********************** internal data definition *****************************/
const char *p_task_system_riego_ 		= "Task System Riego(System Statechart)";
const char *p_task_system_riego 		= "Non-Blocking & Update By Time Code";
/********************** external data declaration ****************************/
uint32_t g_task_system_riego_cnt;
volatile uint32_t g_task_system_riego_tick_cnt;
/********************** external functions definition ************************/
void task_system_riego_init(void *parameters)
{
	/* Print out: Task Initialized */
	LOGGER_INFO(" ");
	LOGGER_INFO("  %s is running - %s", GET_NAME(task_system_riego_init), p_task_system_riego);
	LOGGER_INFO("  %s is a %s", GET_NAME(task_system_riego), p_task_system_riego_);

	/* Init & Print out: Task execution counter */
	g_task_system_riego_cnt = G_TASK_SYS_RIEGO_CNT_INI;
	LOGGER_INFO("   %s = %lu", GET_NAME(g_task_system_riego_cnt), g_task_system_riego_cnt);
}

void task_system_riego_update(void *parameters)
{
	bool b_time_update_required = false;

	/* Protect shared resource */
	__asm("CPSID i");	/* disable interrupts */
    if (G_TASK_SYS_RIEGO_TICK_CNT_INI < g_task_system_riego_tick_cnt)
    {
		/* Update Tick Counter */
    	g_task_system_riego_tick_cnt--;
    	b_time_update_required = true;
    }
    __asm("CPSIE i");	/* enable interrupts */

    while (b_time_update_required)
    {
		/* Update Task Counter */
		g_task_system_riego_cnt++;

		/* Run Task Statechart */
    	task_system_riego_statechart();

    	/* Protect shared resource */
		__asm("CPSID i");	/* disable interrupts */
		if (G_TASK_SYS_RIEGO_TICK_CNT_INI < g_task_system_riego_tick_cnt)
		{
			/* Update Tick Counter */
			g_task_system_riego_tick_cnt--;
			b_time_update_required = true;
		}
		else
		{
			b_time_update_required = false;
		}
		__asm("CPSIE i");	/* enable interrupts */
    }
}

void task_system_riego_statechart(void)
{

	    task_system_riego_dta_t *p_task_system_riego_dta = &task_system_riego_dta;
	    C_riego = 0.0f;
	    bool flag_regar=false;

	    calcular_coeficientes_riego(
	        p_task_system_riego_dta->humedad_suelo, p_task_system_riego_dta->temperatura, p_task_system_riego_dta->humedad_ambiente,
	        p_task_system_riego_dta->humedad_suelo_min, p_task_system_riego_dta->temperatura_ambiente_max, p_task_system_riego_dta->humedad_ambiente_min,
	        &C_riego
	    );
	    switch (p_task_system_riego_dta->state)
	    {
	        case ST_SYS_RIEGO_NO_REGAR:

	            if (C_riego > 3.0f && p_task_system_riego_dta->flag_tanque== true)
	            {
	            	flag_regar=true;
	            	ticks_calculados = (uint32_t)(C_riego * MULTIPLICADOR_TICKS);


	            	if(ticks_calculados < MINIMO_TICKS_BOMBA){
	            		ticks_calculados = MINIMO_TICKS_BOMBA;
	            	}
	            	if(ticks_calculados > MAXIMO_TICKS_BOMBA){
	            	    ticks_calculados = MAXIMO_TICKS_BOMBA;
	            	}

	                p_task_system_riego_dta->tick = ticks_calculados + ESPERA_ABSORCION;
	                put_event_task_actuator_bomba_agua(flag_regar, ticks_calculados,0);
	                p_task_system_riego_dta->state = ST_SYS_RIEGO_ESPERA_ABSORCION;
	                flag_regar=false;
	            }
	            break;

	        case ST_SYS_RIEGO_ESPERA_ABSORCION:

	            if (p_task_system_riego_dta->tick > 0) {
	                p_task_system_riego_dta->tick--;
	            } else {
	                p_task_system_riego_dta->state = ST_SYS_RIEGO_NO_REGAR;
	            }
	            break;

	        default:
	            p_task_system_riego_dta->state = ST_SYS_RIEGO_NO_REGAR;
	            break;
	    }
}

void calcular_coeficientes_riego(
	uint8_t S_act, float T_act, float H_act,
	uint8_t S_set, uint8_t T_set, uint8_t H_set,
    float* out_C_riego)
{
    // 2. Errores Base
    float E_s = (float)S_set - (float)S_act;
    float E_t = T_act - (float)T_set;
    float E_h = (float)H_set - H_act;

    // 3. Constantes K
    float K_t;
        if (E_t > 0.0f) {
            K_t = 0.02f;   // Calor: Acelera rápido la evaporación
        } else {
            K_t = 0.015f;  // Frío: Frena despacio el riego
        }
     // 4. Constantes de Humedad
     float K_h;
     	 if (E_h > 0.0f) {
             K_h = 0.01f;   // Aire Seco: Acelera moderadamente
         } else {
        	 // El agua de riego no se evaporará. Penalizamos el coeficiente.
        	 K_h = 0.15f;
         }

    // 4. Factor Climático
    float Factor_clima = 1.0f + (K_t * E_t) + (K_h * E_h);
    if (Factor_clima < 0.4f) {
        Factor_clima = 0.4f;
    }

    *out_C_riego = E_s*Factor_clima;
}

/********************** end of file ******************************************/
