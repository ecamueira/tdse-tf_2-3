 # **Monitoreo Electrónico de Invernadero**
 ## **Informe de Avances**


**Autores: Camueira Elias; Cascia Serena; Donadello Nicolas**

**Padrón: 110772; 107946; 112007**

**Fecha: 2do cuatrimestre 2025**

**Fecha: 06/02/2026**

A continuación se detalla el informe de avances del proyecto a partir de los requerimientos, ademas se detalla si la misma fue modificada o adaptada.

| Estado | Descripción      |
|-----|---------------------|
| 🟢 | Ya implementado |
| 🟡 | En proceso de implementarse |
| 🔴 | No se implementará |

| Grupo | ID | Descripción | Estado |Modificado|
| :---- | :---- | :---- | :---- | :---- |
| Sensores ambientales | 1.1 | El sistema contará con un sensor de temperatura y humedad ambiente (DHT22) para supervisar condiciones del cultivo.  |🟡 | No |
|  | 1.2 | El sistema contará con un sensor de humedad de suelo (preferentemente capacitivo) por maceta/zona para gobernar el riego automático. |🟢| No|
|  | 1.3 | El sistema realizará lecturas periódicas de los sensores y enviará valores al sistema. | 🟢 | Si |
| Actuadores — Riego | 2.1 | El sistema controlará una bomba para activar riego en función del umbral de humedad de suelo configurado. | 🟡 | No |
|  | 2.2 | El sistema controlará una bomba para activar la fertilización líquida en función del tiempo o de la cantidad de riegos de agua efectuados, lo que ocurra primero.|🟡|No|
|  | 2.3 | El riego será interrumpido automáticamente si se detecta falta de agua (sensor de nivel de tanque) o marcha en seco (protección). |🟢|No|
| Actuadores — Iluminación | 2.4 | El sistema contará con una tira LED RGB (analógica o direccionable) para iluminación artificial del cultivo, controlada por PWM. |🟢|No|
|  | 2.5 | La intensidad y el espectro (combinación R/G/B) serán configurables desde la aplicación para definir fotoperíodos y etapas (crecimiento / fructificación). |🟡|No|
| Actuadores — Ventilación | 2.6 | El sistema contará con un ventilador tipo PC (cooler) controlable por PWM y por MOSFET de ser necesario, para renovación de aire y control térmico local. |🟡|No|
|  | 2.7 | El ventilador podrá operar en modos definidos por firmware (p. ej. ON/OFF, control proporcional por temperatura/humedad), seleccionables desde la app. |🟡|No|
|  | 2.8 | Si se desea, el sistema leerá la señal tach del ventilador para medir RPM y validar que el ventilador está funcionando. |🟡|No|
| Almacenamiento | 3.1 | La configuración del sistema (umbrales, fotoperíodos, parámetros) se persistirá en la **Flash interna** del microcontrolador. |🟡|No|
|  | 3.2 | El sistema recuperará la configuración guardada al iniciar y validará la integridad de la misma. |🟡|No|
| Interfaz/App | 4.1 | Toda la interacción de usuario, notificaciones y alarmas se realizará mediante la aplicación móvil conectada por BLE. |🟡|No|
|  | 4.2 | La app permitirá configurar umbrales, umbrales de riego, programar fotoperíodos y controlar el ventilador. |🟡|Si|
|  | 4.3 | El sistema enviará a la app lecturas periódicas y eventos críticos (ej. falta de agua, sensor desconectado, fallo de ventilador). |🟡|No|
| Operación segura | 5.1 | Si ocurre un evento inesperado que suponga un riesgo al sistema, el mismo deberá reiniciar con los actuadores en estado pasivo |🟡|No|

<p align="center"><em>Tabla 2: Requisitos del proyecto</em></p>

