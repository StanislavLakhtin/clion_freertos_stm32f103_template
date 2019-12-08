//
// Created by Stanislav Lakhtin on 08.11.2019.
//

#include <FreeRTOS.h>
#include <task.h>
#include <libopencm3/stm32/gpio.h>

// Начинаем неистово дергать C13 в случае, если у нас переполнение стека. Сигнал об ошибке
// todo проверить стратегию увеличения размера стека для задачи, другие идеи

void vApplicationStackOverflowHook( TaskHandle_t xTask,
                                    signed char * pcTaskName ) {
  for ( ;; ) {
    gpio_toggle(GPIOC, GPIO13);
    vTaskDelay(pdMS_TO_TICKS( 100 ));
  }
}