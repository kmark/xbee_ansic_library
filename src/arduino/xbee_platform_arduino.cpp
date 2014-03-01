#include "Arduino.h"
#include "xbee_platform.h"

HardwareSerial *StdioSerial = &Serial; // Serial you're using for user input

void xbee_platform_init( void)
{
    static bool_t init = TRUE;
    if(init) {
        init = FALSE;
        StdioSerial->begin(115200);
    }
}

uint32_t xbee_seconds_timer( void)
{
    return millis() / 1000L;
}

uint32_t xbee_millisecond_timer( void)
{
    return millis();
}

uint16_t _xbee_get_unaligned16( const void FAR *p)
{
    uint16_t retval;
    _f_memcpy(&retval, p, 2);
    return retval;
}

uint32_t _xbee_get_unaligned32( const void FAR *p)
{
    uint32_t retval;
    _f_memcpy(&retval, p, 4);
    return retval;
}

void _xbee_set_unaligned16( void FAR *p, uint16_t value)
{
    _f_memcpy(p, &value, 2);
}

void _xbee_set_unaligned32( void FAR *p, uint32_t value)
{
    _f_memcpy(p, &value, 4);
}

#include <ctype.h>

#define XBEE_READLINE_STATE_INIT            0
#define XBEE_READLINE_STATE_START_LINE      1
#define XBEE_READLINE_STATE_CONTINUE_LINE   2

// See xbee/platform.h for function documentation.
int xbee_readline( char *buffer, int length)
{
    static int state = XBEE_READLINE_STATE_INIT;
    int c;
    static char *cursor;

    if(buffer == NULL || length < 1) {
        return -EINVAL;
    }

    switch(state) {
        default:
        case XBEE_READLINE_STATE_INIT:
        case XBEE_READLINE_STATE_START_LINE: // Start of new line
            cursor = buffer;
            *buffer = '\0'; // reset string
            state = XBEE_READLINE_STATE_CONTINUE_LINE;
            // fall through to continued input
        case XBEE_READLINE_STATE_CONTINUE_LINE:
            if(!StdioSerial->available()) {
                return -EAGAIN;
            }
            c = StdioSerial->read();
            switch(c) {
                case '\n':
                case '\r':
                    StdioSerial->write('\n');
                    state = XBEE_READLINE_STATE_START_LINE;
                    return cursor - buffer;
                default:
                    if(isprint(c) && (cursor - buffer < length - 1)) {
                        *cursor++ = c;
                        StdioSerial->write(c);
                        break;
                    }
                    StdioSerial->write('\x07');
            }
            *cursor = 0;
            StdioSerial->flush();
    }
    return -EAGAIN;
}
