//
// Created by Stanislav Lakhtin on 06/11/2019.
//

#ifndef BBB_OS_TASKS_H
#define BBB_OS_TASKS_H

#ifdef __cplusplus
extern "C"
{
#endif

#include <ow.h>
#include <FreeRTOS.h>
#include <semphr.h>

void usb_ui( void * arg __attribute((unused)));

void bme280_probe( void * arg );

void one_wire_task( void * arg );

float ow_get_temp_by_num( uint8_t rom_num );

RomCode * ow_get_rom( uint8_t rom_num );

SemaphoreHandle_t get_ow_semaphore();

#ifdef __cplusplus
}
#endif

#endif //BBB_OS_TASKS_H
