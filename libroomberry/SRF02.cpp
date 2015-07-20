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

#ifdef __linux__
#ifdef __arm__
// assume we are compiling on Raspberry PI
#define TARGET_RPI
#endif
#endif

#include <stdio.h>
#include <errno.h>
#include <stdlib.h>

#ifdef TARGET_RPI
#include <linux/i2c-dev.h>
#else
// Dummy define, wont work ...
#define I2C_SLAVE 0
#endif

#include <fcntl.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include "SRF02.h"

SRF02::SRF02( uint8_t adr, const char *dev ) {
    address    = adr;
    i2cDevice  = dev;
    lastRange  = 0;
}

bool SRF02::changeI2cAddress ( uint8_t address ) {
    bool success = FALSE;
    if (i2cWriteCmd(0xA0)) {
        printf ( "0xA0 ");
        usleep(200000);
        if (i2cWriteCmd(0xAA)) {
            printf ( "0xAA ");
            usleep(200000);
            if (i2cWriteCmd(0xA5)) {
                printf ( "0xA5 ");
                usleep(200000);
                success = i2cWriteCmd(address);
                printf ( "0x%02x\n", address);
            }
        }
    }
    if (!success) {
        fprintf(stderr, "[ERROR] SRF02::changeI2CAddress(...) failed. %s\n", i2cErrorString);
    }
    return(success);
}

uint16_t SRF02::getRange(uint8_t cmd) {
    uint16_t range=0;
    uint8_t  data[4];
    bool     success=FALSE;
    
    if ( i2cWriteCmd(cmd) ) {
        usleep(PING_DELAY);
        success = i2cReadBytes( data, 255 );
    }
    if (!success) {
        fprintf(stderr, "[ERROR] SRF02::getRange(...) failed. %s\n", i2cErrorString);
    } else {
        range = WORD(data[2],data[3]);
    }
    return(range);
}

uint16_t SRF02::getRealRangeIn(void) {
    return(getRange(0x50));
}

uint16_t SRF02::getRealRangeCm(void) {
    uint16_t range = getRange(0x51);
    if ( range < RANGE_MIN_CM ) {
        range = RANGE_MIN_CM;
    } else if ( range > RANGE_MAX_CM ) {
        range = RANGE_MAX_CM;
    }
    return(range);
}

uint16_t SRF02::getRealRangeUs(void) {
    return(getRange(0x52));
}

uint16_t SRF02::getFakeRangeCm(void) {
    return(getRange(0x56));
}

uint16_t SRF02::getFakeRangeIn(void) {
    return(getRange(0x57));
}

uint16_t SRF02::getFakeRangeUs(void) {
    return(getRange(0x58));
}

void SRF02::sendBurst() {
    if ( !i2cWriteCmd( 0x5C ) ) {
        fprintf(stderr, "[ERROR] SRF02::sendBurst() failed. %s\n", i2cErrorString);
    }
}

bool SRF02::i2cWriteCmd( uint8_t command ) {
    int8_t buf[2];
    int    fd;
    bool   success = FALSE;
    
    i2cErrorString[0] = (char)0;
    
    fd = open(i2cDevice, O_RDWR);
    if (fd < 0) {
        sprintf(i2cErrorString, "Failed to open device %s: %s", i2cDevice, strerror(errno));
    } else {
        if (ioctl(fd, I2C_SLAVE, address) < 0) {
            sprintf(i2cErrorString, "Failed to select device %s: %s", i2cDevice, strerror(errno));
        } else {
            buf[0] = 0;
            buf[1] = command;
            if (write(fd, buf, 2) != 2 ) {
                sprintf(i2cErrorString, "Failed to write command (0x%02x)to device %s: %s", command, i2cDevice, ::strerror(errno));
            } else {
                success = TRUE;
            }
            close(fd);
        }
    }
    return(success);
}

bool SRF02::i2cReadBytes ( uint8_t *data ) {
    return(i2cReadBytes(data,1));
}

bool SRF02::i2cReadBytes ( uint8_t *data, uint8_t max_retry ) {
    bool success = FALSE;
    i2cErrorString[0] = (char)0;
    int8_t count = -1;
    int fd = open(i2cDevice, O_RDWR);
    
    if (fd < 0) {
        sprintf(i2cErrorString, "Failed to open device %s: %s", i2cDevice, strerror(errno));
    } else {
        if (ioctl(fd, I2C_SLAVE, address) < 0) {
            sprintf(i2cErrorString, "Failed to select device %s: %s", i2cDevice, strerror(errno));
        } else {
            uint8_t retry=0;
            data[0] = 0;
            write(fd, data, 1);
            while((count<0) && (retry<=max_retry)) {
                count = read(fd, data, 4);
                retry++;
                usleep(500);
            }
            if (count < 0 && retry >= max_retry ) {
                sprintf(i2cErrorString, "Failed to read device %s(%d): %s", i2cDevice, count, ::strerror(errno));
            } else {
                if (count != 4) {
                    sprintf(i2cErrorString, "Short read  from device %s, expected 4, got %d", i2cDevice, count);
                } else {
                    success = TRUE;
                }
            }
        }
        close(fd);
    }
    return(success);
}
