//
// Created by Stanislav Lakhtin on 18/11/2019.
//
#ifndef BBB_DEFS_H
#define BBB_DEFS_H

#include <os_tasks.h>
#include <ow.h>
#include <usbcdc.h>
#include <FreeRTOS.h>
#include <task.h>
#include <semphr.h>

#ifndef GLOBALSTATE
#define GLOBALSTATE

typedef struct {
  uint16_t update_period;
  float last_temp[ONEWIRE_MAXDEVICES_ON_THE_BUS];
  OneWire * bus;
} OW_State_type;

typedef struct {
  /*! Compensated pressure */
  uint32_t pressure;

  /*! Compensated temperature */
  float temperature;

  /*! Compensated humidity */
  uint32_t humidity;
} BME280_State;

typedef struct {
  OW_State_type ow;
  BME280_State bme280;
} StationState;

StationState state;

OneWire ow_dev;
usbd_device * usbd;

#endif // GLOBALSTATE

#endif // BBB_DEFS_H
