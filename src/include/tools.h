//
// Created by Stanislav Lakhtin on 06/11/2019.
//

#ifndef BBB_TOOLS_H
#define BBB_TOOLS_H

#ifdef __cplusplus
extern "C"
{
#endif

void clock_setup();

void gpio_setup();

void i2c_bme_setup();

void onewire_usart_init();

void onewire_usart_setup( uint32_t baud );

void onewire_send( uint16_t );

#ifdef __cplusplus
}
#endif

#endif //BBB_TOOLS_H
