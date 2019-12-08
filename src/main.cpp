#include <FreeRTOS.h>
#include <task.h>
#include <queue.h>

#include <defs.h>
#include <libopencm3/cm3/nvic.h>   // without that ISR didn't linked properly
#include <libopencm3/stm32/usart.h>

#include <tools.h>

int main() {
  clock_setup();
  gpio_setup();
  i2c_bme_setup();
  onewire_usart_init();

  ow_dev.usart_setup = onewire_usart_setup;
  ow_dev.send_usart = onewire_send;

  usbd = usb_setup();

  TaskHandle_t xCDCHandle;
  TaskHandle_t xUIHandle;

  xTaskCreate( one_wire_task, "OW", configMINIMAL_STACK_SIZE, &ow_dev, configMAX_PRIORITIES - 1, NULL );
  xTaskCreate( usb_processor, "CDC", configMINIMAL_STACK_SIZE, usbd, configMAX_PRIORITIES - 1, &xCDCHandle );
  xTaskCreate( bme280_probe, "BME280", configMINIMAL_STACK_SIZE, NULL, configMAX_PRIORITIES - 1, NULL );
  xTaskCreate( usb_ui, "USB_UI", 400, NULL, configMAX_PRIORITIES - 1, &xUIHandle );

  vTaskStartScheduler();
}

void usart3_isr() {
  /* Проверяем, что мы вызвали прерывание из-за RXNE. */
  while (((USART_CR1( USART3 ) & USART_CR1_RXNEIE) != 0 ) &&
         ((USART_SR( USART3 ) & USART_SR_RXNE) != 0 )) {
    /* Получаем данные из периферии и сбрасываем флаг*/
    uint8_t data = usart_recv_blocking(USART3);
    ow_bus_get_echo_data( &ow_dev, data );
  }
}