//
// Created by Stanislav Lakhtin on 11/11/2019.
//
#include <FreeRTOS.h>
#include <task.h>

#include <libopencm3/stm32/i2c.h>

#include <bme280_driver.h>


void user_delay_ms( uint32_t period ) {
  vTaskDelay(pdMS_TO_TICKS( period ));
}

int8_t bme280_i2c_read( uint8_t dev_id, uint8_t reg_addr, uint8_t * data, uint16_t len ) {
  i2c_transfer7(BME280_I2C, dev_id, &reg_addr, 1, NULL, 0 );
  i2c_transfer7(BME280_I2C, dev_id, NULL, 0, data, len );
  return BME280_OK;
}

int8_t bme280_i2c_write( uint8_t dev_id, uint8_t reg_addr, uint8_t * data, uint16_t len ) {
  while ((I2C_SR2( BME280_I2C ) & I2C_SR2_BUSY)) {
  }

  i2c_send_start(BME280_I2C);

  /* Wait for master mode selected */
  while ( !((I2C_SR1( BME280_I2C ) & I2C_SR1_SB)
            & (I2C_SR2( BME280_I2C ) & (I2C_SR2_MSL | I2C_SR2_BUSY)))) taskYIELD();

  i2c_send_7bit_address(BME280_I2C, dev_id, I2C_WRITE );

  /* Waiting for address is transferred. */
  while ( !(I2C_SR1( BME280_I2C ) & I2C_SR1_ADDR));

  /* Clearing ADDR condition sequence. */
  ( void ) I2C_SR2( BME280_I2C );

  i2c_send_data(BME280_I2C, reg_addr );

  while ( !(I2C_SR1( BME280_I2C ) & (I2C_SR1_BTF))) taskYIELD();

  for ( size_t i = 0 ; i < len ; i++ ) {
    i2c_send_data(BME280_I2C, data[ i ] );
    while ( !(I2C_SR1( BME280_I2C ) & (I2C_SR1_BTF))) taskYIELD();
  }
  i2c_send_stop(BME280_I2C);
  return BME280_OK;
}

uint8_t init_bme280( struct bme280_dev * dev ) {
  dev->dev_id = BME280_I2C_ADDR_PRIM;
  dev->intf = BME280_I2C_INTF;
  dev->read = bme280_i2c_read;
  dev->write = bme280_i2c_write;
  dev->delay_ms = user_delay_ms;

  return bme280_init( dev );
}

/*!
 * @brief This API calculates the CRC
 *
 * @param[in] mem_values : reg_data parameter to calculate CRC
 * @param[in] mem_length : Parameter to calculate CRC
 *
 * @return Result of API execution status
 * @retval zero -> Success / +ve value -> Warning / -ve value -> Error
 */
static uint8_t crc_calculate( uint8_t * mem_values, uint8_t mem_length );

/*!
 * @brief This API reads the stored CRC and then compare with calculated CRC
 *
 * @param[in] dev : Structure instance of bme280_dev.
 *
 * @return Result of API execution status
 * @retval zero -> self test success / +ve value -> warning(self test fail)
 */
int8_t bme280_crc_selftest( const struct bme280_dev * dev ) {
  int8_t rslt;
  uint8_t reg_addr;
  uint8_t reg_data[64];

  uint8_t stored_crc = 0;
  uint8_t calculated_crc = 0;

  /* Read stored crc value from register */
  reg_addr = BME280_CRC_DATA_ADDR;
  rslt = bme280_get_regs( reg_addr, reg_data, BME280_CRC_DATA_LEN, dev );
  if ( rslt == BME280_OK ) {
    stored_crc = reg_data[ 0 ];
    /* Calculated CRC value with calibration register */
    reg_addr = BME280_CRC_CALIB1_ADDR;
    rslt = bme280_get_regs( reg_addr, &reg_data[ 0 ], BME280_CRC_CALIB1_LEN, dev );
    if ( rslt == BME280_OK ) {
      reg_addr = BME280_CRC_CALIB2_ADDR;
      rslt = bme280_get_regs( reg_addr, &reg_data[ BME280_CRC_CALIB1_LEN ], BME280_CRC_CALIB2_LEN, dev );
      if ( rslt == BME280_OK ) {
        calculated_crc = crc_calculate( reg_data, BME280_CRC_CALIB1_LEN + BME280_CRC_CALIB2_LEN );
        /* Validate CRC */
        if ( stored_crc == calculated_crc )
          rslt = BME280_OK;
        else
          rslt = BME280_W_SELF_TEST_FAIL;
      }
    }
  }

  return rslt;
}

/*!
 * @brief This API calculates the CRC
 *
 * @param[in] mem_values : reg_data parameter to calculate CRC
 * @param[in] mem_length : Parameter to calculate CRC
 *
 * @return Result of API execution status
 * @retval zero -> Success / +ve value -> Warning / -ve value -> Error
 */
static uint8_t crc_calculate( uint8_t * mem_values, uint8_t mem_length ) {
  uint32_t crc_reg = 0xFF;
  uint8_t polynomial = 0x1D;
  uint8_t bitNo, index;
  uint8_t din = 0;

  for ( index = 0 ; index < mem_length ; index++ ) {
    for ( bitNo = 0 ; bitNo < 8 ; bitNo++ ) {
      if ((( crc_reg & 0x80 ) > 0 ) ^ (( mem_values[ index ] & 0x80 ) > 0 ))
        din = 1;
      else
        din = 0;

      /* Truncate 8th bit for crc_reg and mem_values */
      crc_reg = ( uint32_t ) (( crc_reg & 0x7F ) << 1 );
      mem_values[ index ] = ( uint8_t ) (( mem_values[ index ] & 0x7F ) << 1 );
      crc_reg = ( uint32_t ) ( crc_reg ^ ( polynomial * din ));
    }
  }

  return ( uint8_t ) ( crc_reg ^ 0xFF );
}
