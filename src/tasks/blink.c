//
// Created by Stanislav Lakhtin on 06/11/2019.
//

#include <FreeRTOS.h>
#include <task.h>
#include <libopencm3/stm32/gpio.h>

void blink_led( void * arg __attribute((unused))) {
  int delta = 20;
  int delay = delta;
  for ( ;; ) {
    gpio_toggle(GPIOC, GPIO13);
    delay += delta;
    if ( delay > 400 )
      delta = -20;
    else if ( delay < 200 )
      delta = 20;
    vTaskDelay(pdMS_TO_TICKS( delay ));
  }
}