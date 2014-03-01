/*
 * Copyright (c) 2010-2012 Digi International Inc.,
 * All rights not expressly granted are reserved.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * Digi International Inc. 11001 Bren Road East, Minnetonka, MN 55343
 * =======================================================================
 */
/**
    @addtogroup hal_arduino
    @{
    @file xbee_serial_arduino.cpp
    Serial Interface for Arduino's HardwareSerial
*/

// NOTE: Documentation for these functions can be found in xbee/serial.h.

#include <limits.h>
#include "xbee_serial.h"

HardwareSerial *XBeeSerial;

// Could change XBEE_SER_CHECK to an assert, or even ignore it if not in debug.
#if defined XBEE_SERIAL_DEBUG
    #define XBEE_SER_CHECK(ptr) \
        do { if (xbee_ser_invalid(ptr)) return -EINVAL; } while (0)
#else
    #define XBEE_SER_CHECK(ptr)
#endif

bool_t xbee_ser_invalid( xbee_serial_t *serial)
{
    return serial == NULL;
}


const char *xbee_ser_portname( xbee_serial_t *serial)
{
    if(serial != NULL) {
        return "(unknown)";
    }
    return "(invalid)";
}


int xbee_ser_write( xbee_serial_t *serial, const void FAR *buffer,
    int length)
{
    int wrote = length;
    const uint8_t FAR *b = (const uint8_t FAR *) buffer;

    XBEE_SER_CHECK(serial);

    if(length < 0) {
        return -EINVAL;
    }

    return serial->serial->write((const uint8_t FAR *)buffer, length);
}

int xbee_ser_read( xbee_serial_t *serial, void FAR *buffer, int bufsize)
{

    XBEE_SER_CHECK(serial);

    if(bufsize == 0) {
        return 0;
    }

    if(bufsize < 0) {
        return -EINVAL;
    }

    if(!serial->serial->available()) {
        return 0;
    }

    return serial->serial->readBytes((char FAR *)buffer, bufsize);
}


int xbee_ser_putchar( xbee_serial_t *serial, uint8_t ch)
{
    int retval;
    // the call to xbee_ser_write() does XBEE_SER_CHECK() for us
    retval = xbee_ser_write(serial, &ch, 1);
    switch(retval) {
        case 1:
            return 0;
        case 0:
            return -ENOSPC;
    }
    return retval;
}


int xbee_ser_getchar( xbee_serial_t *serial)
{

    XBEE_SER_CHECK(serial);

    if(!serial->serial->available()) {
        return -ENODATA;
    }

    return serial->serial->read();
}

int xbee_ser_tx_free( xbee_serial_t *serial)
{
    XBEE_SER_CHECK(serial);
    return INT_MAX;
}
int xbee_ser_tx_used( xbee_serial_t *serial)
{
    XBEE_SER_CHECK(serial);
    return 0;
}


int xbee_ser_tx_flush( xbee_serial_t *serial)
{
    XBEE_SER_CHECK(serial);
    serial->serial->flush();
    return 0;
}


int xbee_ser_rx_free( xbee_serial_t *serial)
{
    XBEE_SER_CHECK(serial);
    return INT_MAX;
}


int xbee_ser_rx_used( xbee_serial_t *serial)
{
    XBEE_SER_CHECK(serial);
    return 0;
}


int xbee_ser_rx_flush( xbee_serial_t *serial)
{
    XBEE_SER_CHECK(serial);
    serial->serial->flush();
    return 0;
}


int xbee_ser_baudrate( xbee_serial_t *serial, uint32_t baudrate)
{
    XBEE_SER_CHECK(serial);
    if(*serial->serial) {
        serial->serial->end();
    }
    serial->serial->begin(baudrate);
    serial->baudrate = baudrate;
    return 0;
}

int xbee_ser_open( xbee_serial_t *serial, uint32_t baudrate)
{
    XBEE_SER_CHECK(serial);

    XBeeSerial = serial->serial;

    return xbee_ser_baudrate(serial, baudrate);
}


int xbee_ser_close( xbee_serial_t *serial)
{
    XBEE_SER_CHECK(serial);
    serial->serial->end();
    return 0;
}


int xbee_ser_break( xbee_serial_t *serial, bool_t enabled)
{
    XBEE_SER_CHECK(serial);
    return 0;
}


int xbee_ser_flowcontrol( xbee_serial_t *serial, bool_t enabled)
{
    XBEE_SER_CHECK(serial);
    return 0;
}

int xbee_ser_set_rts( xbee_serial_t *serial, bool_t asserted)
{
    XBEE_SER_CHECK(serial);
    return 0;
}


int xbee_ser_get_cts( xbee_serial_t *serial)
{
    XBEE_SER_CHECK(serial);
    return 1;
}

//@}
