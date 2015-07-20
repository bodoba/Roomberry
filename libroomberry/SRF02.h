/*
 * ---------------------------------------------------------------------------------------
 * Copyright 2014 by Bodo Bauer <bb@bb-zone.com>
 *
 * The I2C Arduino Slave Library is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * The I2C Arduino Slave Library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Foobar.  If not, see <http://www.gnu.org/licenses/>.
 * --------------------------------------------------------------------------------------
 */
#include <stdint.h>

#ifndef SRF02_H
#define SRF02_H

#ifndef TRUE
#define TRUE	(1==1)
#define FALSE	(0==1)
#endif

#define SRF02_BUFFER_SIZE 16


#ifndef WORD
#define WORD(a,b) ((b&0x00ff)|((a<< 8)&0xff00))
#endif

#define PING_DELAY 5000

#define RANGE_MIN_CM  10
#define RANGE_MAX_CM  90

class SRF02 {
public:
    SRF02(uint8_t, const char *);

    bool changeI2cAddress(uint8_t);

    uint16_t getRealRangeCm(void);
    uint16_t getRealRangeIn(void);
    uint16_t getRealRangeUs(void);

    uint16_t getFakeRangeCm(void);
    uint16_t getFakeRangeIn(void);
    uint16_t getFakeRangeUs(void);

    void sendBurst();
    
private:
    uint8_t    address;
    const char *i2cDevice;
    char       i2cErrorString[128];
    uint16_t   lastRange;

    bool i2cWriteCmd(uint8_t);
    bool i2cReadBytes(uint8_t*);
    bool i2cReadBytes(uint8_t*, uint8_t);
    
    uint16_t getRange(uint8_t);
};
#endif
