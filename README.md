# STM32 FreeRTOS DHT22 Sensor Project

## 🇬🇧 English Description
This project implements a temperature and humidity monitoring system using an **STM32F103** microcontroller and a **DHT22** sensor.
*   **Real-time OS**: Built on **FreeRTOS** with two independent tasks.
*   **Inter-task Communication**: Uses **Queues** to pass sensor data to the display task.
*   **Efficiency**: Custom `delay_us` implementation and critical sections for precise sensor timing.
*   **Output**: Data is formatted as integers (to save resources) and sent via **UART** to a PC terminal.

## 🇸🇪 Svensk beskrivning (Swedish)
Detta projekt är ett system för övervakning av temperatur och luftfuktighet med en **STM32F103**-mikrokontroller och en **DHT22**-sensor.
*   **Realtidsoperativsystem**: Byggt på **FreeRTOS** med två oberoende uppgifter (tasks).
*   **Kommunikation**: Använder **köer (Queues)** för att skicka sensordata till display-uppgiften.
*   **Effektivitet**: Anpassad `delay_us` och kritiska sektioner för exakt tidtagning av sensorn.
*   **Utmatning**: Data skickas via **UART** till en PC-terminal.

---
*Developed by PeterSysEng - Electronics Technician and Programmer*
