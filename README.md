Warning: I apologize for the possible mistakes: English is not my native language.

JetBrains CLion is a great tool for developing in C / C ++. But it's based on `cmake` and I had a problem when trying to use it for microcontroller project based on stm32F10X (Cortex-M3).

For hobby purposes, I developed a small project that solves the following tasks:

• Building the [FreeRTOS](https://freertos.org/) project (10.2.x) + [libopencm3](https://github.com/libopencm3/libopencm3) in `cmake`
• Implementation of the USB stack initialization mechanism
• Implementation on the basis of [1] and [2] of the model for interviewing temperature sensors DS18B20 and BME280.
• State management.

The goal was to get a rational in terms of size, fast and convenient technological stack for various hobby tasks. I really like the libopencm3 project for its beauty, conciseness, precision and elegance.

FreeRTOS is also an example of very elegant code. By combining them together, I got the foundation for the project, and assembly under Clion allowed me to use this wonderful developer tool.

I perform intra circuit debugging under the [blackmagic probe](https://github.com/blacksphere/blackmagic/wiki).

To load the example with some meaningful logic, the task of interrogating a large number of temperature sensors DS18B20 was chosen. This is a slow and time expensive operation that must be performed in parallel with the interaction interface on the USB bus.

To complicate the task, in parallel it initializes and query the BME280 sensor. Its exclusion from the project is not difficult if you don't want use it. The task was to interrogate several types of sensors in parallel and implement the user interface at the same time.

As a result, the task was to obtain an elegant and responsive device that can be connected to and controlled by any computer, interacting via the USB bus using the minicom, putty, etc programs (depending on the type of the main OS).