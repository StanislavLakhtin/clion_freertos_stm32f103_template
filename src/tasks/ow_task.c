#include <FreeRTOS.h>
#include <task.h>

#include <defs.h>

#include <ds18b20.h>
#include <semphr.h>

#define LONGWAIT 10000
SemaphoreHandle_t xOwInUse = NULL;

SemaphoreHandle_t get_ow_semaphore() {
  return xOwInUse;
}

void load_default( OW_State_type * state ) {
  state->update_period = 20000;
}

float ow_get_temp_by_num( uint8_t rom_num ) {
  if ( rom_num < state.ow.bus->state.devicesQuantity )
    return state.ow.last_temp[ rom_num ];
  else
    return -1000;
}

RomCode * ow_get_rom( uint8_t rom_num ) {
  if ( rom_num < state.ow.bus->state.devicesQuantity )
    return &state.ow.bus->rom[ rom_num ];
  else
    return 0x00;
}

void one_wire_task( void * arg ) {
  xOwInUse = xSemaphoreCreateMutex();

  OneWire * ow_dev = arg;
  state.ow.bus = ow_dev;

  load_default( &state.ow );
  while ( TRUE ) {
    if ( xSemaphoreTake( xOwInUse, ( TickType_t ) LONGWAIT ) == pdTRUE) {
      uint8_t rslt = ow_scan( ow_dev );
      if ( rslt ) {
        for ( uint8_t i = 0 ; i < ow_dev->state.devicesQuantity ; i++ ) {
          if ( ow_dev->rom[ i ].family == 0x28 ) {  // Found DS18B20 Temp sensor
            state.ow.last_temp[ i ] = read_temperature( ow_dev, &ow_dev->rom[ i ] );
          }
        }
      }
      xSemaphoreGive( xOwInUse );  // освобождаем мютекс
      vTaskDelay(pdMS_TO_TICKS( state.ow.update_period ));
    } else
      vTaskDelay(pdMS_TO_TICKS( 1000 )); // не смогли захватить управление. Ждём 1 сек и повторяем
  }
}