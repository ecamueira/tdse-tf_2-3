Description:
 Bare Metal - Event-Triggered Systems (ETS)
 Automated Greenhouse Control System (SCADA Architecture)
 App - retarget_printf_to_Console
 Project for STM32 Project (STM32CubeIDE Version: 1.17.0)

  SystemCoreClock     => 64MHz (15.625nS)
  SysTick Rate Hertz  => 1000 ticks per second (1mS)

  app.c (app.h)
   Endless loops, which execute tasks with fixed computing time. This 
   sequential execution is only deviated from when an interrupt event occurs.
   Cyclic Executive (Update by Time Code, period = 1mS). Orchestrates the 
   Greenhouse Control Layers.

  =========================================================================
  [ COMMUNICATION & PERSISTENCE LAYER ]
  =========================================================================
  task_system_memory.c (task_system_memory.h, task_system_memory_attribute.h)
   Non-Blocking & Update By Time Code -> I2C EEPROM Modeling.
   Handles BLE/App incoming preferences, saving them securely to EEPROM, 
   and distributing setpoints (T_set, H_set, S_set) to the rest of the system.
   
   
   =========================================================================
  [ SENSOR LAYER - DATA ACQUISITION ]
  =========================================================================
  task_sensor_BLE.c (task_sensor_BLE.h, task_sensor_BLE_attribute.h)
   Non-Blocking -> UART/Wireless Interface.
   Captures user configurations, light controls, and setpoint updates.

  task_sensor_digital.c (task_sensor_digital.h, task_sensor_digital_attribute.h)
   Non-Blocking -> Digital Hardware Interface -> I2C. 
   Acquires Ambient Temperature and Relative Humidity data.

  task_sensor_analog.c (task_sensor_analog.h, task_sensor_analog_attribute.h)
   Non-Blocking -> ADC Hardware Interface (Multi-channel). 
   Reads and filters analog values for:
   
	Capacitive Soil Moisture.
	Water Tank Level.
	Fertilizer Tank Level.

  =========================================================================
  [ INTELLIGENCE LAYER - SYSTEM MODELING ]
  =========================================================================
  task_system_ventilacion.c (task_system_ventilacion.h, task_system_ventilacion_attribute.h) 
   Non-Blocking Code -> Climate Control Logic.
   Evaluates Temp/Hum, calculates proportional PWM, and manages Soft Start.
  
  task_system_riego.c (task_system_riego.h, task_system_riego_attribute.h) 
   Non-Blocking Code -> Irrigation Fuzzy Logic.
   Evaluates soil moisture and climate coefficients, calculates irrigation ticks, 
   and handles soil absorption delays.

  task_system_level_tanque.c (task_system_level_tanque.h, task_system_level_tanque_attribute.h) 
   Non-Blocking Code -> Tank Safety Logic.
   Monitors water/fertilizer stock. Flags safe operation for water pump and 
   triggers automatic daily fertilization cycles.

  task_system_LED.c (task_system_LED.h, task_system_LED_attribute.h)
   Non-Blocking Code.
   Simplified Logic: Routes user configuration from BLE/Memory to the LED actuator.
   Supports manual ON/OFF or brightness levels via user command.

  * All system tasks have their respective *_interface.c files for Event handling.

  =========================================================================
  [ ACTUATOR LAYER - HARDWARE CONTROL ]
  =========================================================================
  task_actuator_cooler.c (task_actuator_cooler.h, task_actuator_cooler_attribute.h) 
   Non-Blocking & Update By Time Code -> Actuator Modeling.
   Hardware execution of fan speed via PWM signals.

  task_actuator_bomba_agua.c (task_actuator_bomba_agua.h, task_actuator_bomba_agua_attribute.h) 
   Non-Blocking & Update By Time Code -> Actuator Modeling.
   Manages Dual Relay module. 
   - Water Pump: Short active cycles based on irrigation ticks.
   - Fertilizer Pump: 5-second active cycle followed by a 24-hour hardware lock.

  task_actuator_LED.c (task_actuator_LED.h, task_actuator_LED_attribute.h)
   Non-Blocking & Update By Time Code -> Actuator Modeling.
   Hardware execution of LED panel power states.

  * All actuator tasks have their respective *_interface.c files for Event handling.

Build procedures:
Visit the Getting started with STM32: STM32 step-by-step at 
"https://wiki.st.com/stm32mcu/wiki/STM32StepByStep:Getting_started_with_STM32_:_STM32_step_by_step"
to get started building STM32 Projects.