#ifndef LIB_USBCDC_H
#define LIB_USBCDC_H

/*
 * This file is part of the libopencm3 project.
 *
 * Copyright (C) 2010 Gareth McMullin <gareth@blacksphere.co.nz>
 *
 * This library is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this library.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/usb/usbd.h>
#include <libopencm3/usb/cdc.h>

#include <stdarg.h>

// Need to be wrapped for C++
BEGIN_DECLS

#define TX_RX_QUEUE_LENGTH 128

void cdcacm_data_rx_cb( usbd_device * usbd_dev, uint8_t ep );

void cdcacm_set_config( usbd_device * usbd_dev, uint16_t wValue );

usbd_device * usb_setup( void );

void usb_processor( void * arg );       // Task для FreeRTOS

void _putchar( char ch );

void usb_puts( const char * buf );

void usb_write( const char * buf, unsigned bytes );

int usb_getc( void );

int usb_vprintf( const char * format, va_list ap );

int usb_printf( const char * format, ... );

int usb_getline( char * buf, unsigned buf_size );

END_DECLS

#endif /* LIBUSBCDC_H */

/* End libusbcdc.h */
