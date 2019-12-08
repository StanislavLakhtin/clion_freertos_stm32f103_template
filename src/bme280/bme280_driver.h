//
// Created by Stanislav Lakhtin on 11/11/2019.
//

#ifndef BBB_BME280_DRIVER_H
#define BBB_BME280_DRIVER_H

#include <bme280_defs.h>
#include <bme280.h>

#define BME280_CRC_DATA_ADDR  UINT8_C(0xE8)
#define BME280_CRC_DATA_LEN  UINT8_C(1)
#define BME280_CRC_CALIB1_ADDR  UINT8_C(0x88)
#define BME280_CRC_CALIB1_LEN  UINT8_C(26)
#define BME280_CRC_CALIB2_ADDR  UINT8_C(0xE1)
#define BME280_CRC_CALIB2_LEN  UINT8_C(7)

#define BME280_W_SELF_TEST_FAIL UINT8_C(2)

#ifdef __cplusplus
extern "C"
{
#endif

#define BME280_I2C I2C1

#define BME280_PAUSE_TRY 10000

#ifdef BME280_TASK_YIELD_IN_PAUSE
#define BME_I2C_PAUSE vTaskDelay(BME280_PAUSE_TRY);
#else
#define BME_I2C_PAUSE taskYIELD();
#endif

uint8_t init_bme280( struct bme280_dev * dev );

void user_delay_ms( uint32_t period );

int8_t bme280_i2c_read( uint8_t dev_id, uint8_t reg_addr, uint8_t * reg_data, uint16_t len );

int8_t bme280_i2c_write( uint8_t dev_id, uint8_t reg_addr, uint8_t * data, uint16_t len );

int8_t bme280_crc_selftest( const struct bme280_dev * dev );

#ifdef __cplusplus
}
#endif

#endif //BBB_BME280_DRIVER_H
