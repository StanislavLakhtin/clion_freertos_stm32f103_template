//
// Created by Stanislav Lakhtin on 11/11/2019.
//

#include <FreeRTOS.h>
#include <task.h>
#include <defs.h>

#include <tools.h>
#include <bme280.h>
#include <bme280_driver.h>
#include <usbcdc.h>

int8_t set_forced_mode( struct bme280_dev * dev ) {
  uint8_t settings_sel;

  dev->settings.osr_h = BME280_OVERSAMPLING_1X;
  dev->settings.osr_p = BME280_OVERSAMPLING_16X;
  dev->settings.osr_t = BME280_OVERSAMPLING_2X;
  dev->settings.filter = BME280_FILTER_COEFF_16;

  settings_sel = BME280_OSR_PRESS_SEL | BME280_OSR_TEMP_SEL | BME280_OSR_HUM_SEL | BME280_FILTER_SEL;

  bme280_soft_reset( dev );
  int8_t rslt = bme280_set_sensor_settings( settings_sel, dev );

  return rslt;
}


void stream_sensor_data_forced_mode( struct bme280_dev * dev ) {
  struct bme280_data comp_data;
  set_forced_mode( dev );

  for ( ;; ) {
    uint8_t rslt = bme280_set_sensor_mode( BME280_FORCED_MODE, dev );
    if ( rslt == BME280_OK ) {
      /* Wait for the measurement to complete and print data @25Hz */
      dev->delay_ms( 3000 );
      bme280_get_sensor_data( BME280_ALL, &comp_data, dev );
      state.bme280.temperature = ( float ) comp_data.temperature / 100;
      state.bme280.pressure = comp_data.pressure / 100;
      state.bme280.humidity = comp_data.humidity / 1024;
      bme280_set_sensor_mode( BME280_SLEEP_MODE, dev );
    }
    vTaskDelay(pdMS_TO_TICKS( 10000 ));
  }

}

int8_t set_normal_mode( struct bme280_dev * dev ) {
  int8_t rslt;
  uint8_t settings_sel;

  /* Recommended mode of operation: Indoor navigation */
  dev->settings.osr_h = BME280_OVERSAMPLING_1X;
  dev->settings.osr_p = BME280_OVERSAMPLING_16X;
  dev->settings.osr_t = BME280_OVERSAMPLING_2X;
  dev->settings.filter = BME280_FILTER_COEFF_16;
  dev->settings.standby_time = BME280_STANDBY_TIME_1000_MS;

  settings_sel = BME280_OSR_PRESS_SEL | BME280_OSR_TEMP_SEL | BME280_OSR_HUM_SEL | BME280_STANDBY_SEL;
  bme280_set_sensor_settings( settings_sel, dev );
  rslt = bme280_set_sensor_mode( BME280_NORMAL_MODE, dev );

  return rslt;
}

void stream_sensor_data_normal_mode( struct bme280_dev * dev ) {
  struct bme280_data comp_data;
  uint8_t rslt = set_normal_mode( dev );
  for ( ;; ) {
    if ( rslt == BME280_OK ) {
      /* Delay while the sensor completes a measurement */
      dev->delay_ms( 2000 );
      bme280_get_sensor_data( BME280_ALL, &comp_data, dev );
      state.bme280.temperature = ( float ) comp_data.temperature / 100;
      state.bme280.pressure = comp_data.pressure / 100;
      state.bme280.humidity = comp_data.humidity / 1024;
    }
    vTaskDelay(pdMS_TO_TICKS( 10000 ));
  }
}

void bme280_probe( void * arg ) {
  struct bme280_dev bme_dev;
  uint8_t rslt = init_bme280( &bme_dev );
  if ( rslt == BME280_OK ) {
    int8_t selfTest = bme280_crc_selftest( &bme_dev );
    if ( selfTest != BME280_OK ) {
      //TODO error
    }
    // Choose one
    // stream_sensor_data_normal_mode(&bme_dev);
    stream_sensor_data_forced_mode( &bme_dev );
  }
}