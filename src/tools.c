#include <FreeRTOS.h>
#include <task.h>

#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/i2c.h>
#include <libopencm3/cm3/nvic.h>
#include <libopencm3/stm32/usart.h>

#include <ow.h>

void clock_setup( void ) {
  rcc_clock_setup_in_hse_8mhz_out_72mhz();

  rcc_periph_clock_enable( RCC_GPIOA );   // USB
  rcc_periph_clock_enable( RCC_GPIOB );   // I2C USART (OW)
  rcc_periph_clock_enable( RCC_GPIOC );   // LED

  rcc_periph_clock_enable( RCC_AFIO );    // EXTI

  rcc_periph_clock_enable( RCC_USART3 );  // USART (OW)
  rcc_periph_clock_enable( RCC_I2C1 );    // I2C

  rcc_periph_clock_enable( RCC_USB );     // USB
}

void gpio_setup() {
  gpio_set_mode(GPIOC, GPIO_MODE_OUTPUT_2_MHZ,
                GPIO_CNF_OUTPUT_PUSHPULL, GPIO13);
  gpio_set(GPIOC, GPIO13);

  gpio_set_mode(GPIOB, GPIO_MODE_OUTPUT_2_MHZ,
                GPIO_CNF_OUTPUT_ALTFN_OPENDRAIN, GPIO_USART3_TX);
  gpio_set_mode(GPIOA, GPIO_MODE_OUTPUT_50_MHZ, GPIO_CNF_OUTPUT_PUSHPULL, GPIO12);
  gpio_clear(GPIOA, GPIO12);  // pull down
  for ( uint16_t i = 0 ; i < 0x1fff ; i++ );
  gpio_set_mode(GPIOA, GPIO_MODE_OUTPUT_2_MHZ, GPIO_CNF_OUTPUT_ALTFN_PUSHPULL, GPIO11 | GPIO12);

  /* Setup GPIO pins for USB D+/D-. */
  //gpio_set(GPIOA, GPIO11 | GPIO12);
  //gpio_set_mode(GPIOA, GPIO_MODE_OUTPUT_2_MHZ, GPIO_CNF_OUTPUT_ALTFN_PUSHPULL, GPIO11 | GPIO12);

  AFIO_MAPR |= AFIO_MAPR_SWJ_CFG_FULL_SWJ_NO_JNTRST;
}

void i2c_bme_setup() {
  i2c_reset(I2C1);
  gpio_set_mode(GPIOB, GPIO_MODE_OUTPUT_50_MHZ,
                GPIO_CNF_OUTPUT_ALTFN_OPENDRAIN, GPIO_I2C1_SDA | GPIO_I2C1_SCL);     // I2C
  gpio_set(GPIOB, GPIO_I2C1_SDA | GPIO_I2C1_SCL);                            // Idle high
  i2c_peripheral_disable(I2C1);
  i2c_reset(I2C1);
  //I2C_CR1(I2C1) &= ~I2C_CR1_STOP;  // Clear stop
  i2c_set_speed(I2C1, i2c_speed_fm_400k, I2C_CR2_FREQ_36MHZ );
  i2c_set_trise(I2C1, 36 );    // 1000 ns for 100k and 300 for 400k
  i2c_set_dutycycle(I2C1, I2C_CCR_DUTY_DIV2 );
  i2c_set_ccr(I2C1, 360 );    // 100 kHz <= 180 * 1 /36M
  i2c_peripheral_enable(I2C1);
}

// Метод для использования USART (1,2,3,4,5) в режиме single-wire half duplex
// Для использвания этого режима GPIO вывода должно быть настроено либо
// в OUTPUT OPENDRAIN или как INPUT FLOAT
// Должны быть сброшены биты USART_CR2 (LINEN, CLKEN)
// Должны быть сброшены биты USART_CR3 (SCEN, IREN)
// Должен быть установлен режим полудуплекса USART_CR3(HDSEL)
// Порт RX в этом случае замыкается внутрисхемно на TX. Вывод остаётся свободен для других назначений
void usart_enable_halfduplex( uint32_t usart ) {
  USART_CR2( usart ) &= ~USART_CR2_LINEN;
  USART_CR2( usart ) &= ~USART_CR2_CLKEN;
  USART_CR3( usart ) &= ~USART_CR3_SCEN;
  USART_CR3( usart ) &= ~USART_CR3_IREN;
  USART_CR3( usart ) |= USART_CR3_HDSEL;
}

void onewire_usart_init() {
  nvic_disable_irq( NVIC_USART3_IRQ );
  usart_disable(USART3);

  // Настраиваем
  usart_set_baudrate(USART3, 9600 );
  usart_set_databits(USART3, 8 );     // 8 bits
  usart_set_stopbits(USART3, USART_STOPBITS_1);
  usart_set_mode(USART3, USART_MODE_TX_RX);
  usart_set_parity(USART3, USART_PARITY_NONE );
  usart_set_flow_control(USART3, USART_FLOWCONTROL_NONE );
  usart_enable_halfduplex(USART3);

  usart_enable_rx_interrupt(USART3);
  usart_disable_tx_interrupt(USART3);

  usart_enable(USART3);
  nvic_enable_irq( NVIC_USART3_IRQ );
}

/** Метод реализует переключение выбранного USART в нужный режим
 * @param[in] baud Скорость в бодах (9600, 115200, etc...)
 */

void onewire_usart_setup( uint32_t baud ) {
  usart_disable(USART3);

  // Настраиваем
  usart_set_baudrate(USART3, baud );

  usart_enable(USART3);
}

void onewire_send( uint16_t data ) {
  usart_send_blocking(USART3, data );
  while ( !usart_get_flag(USART3, USART_SR_TC));
}