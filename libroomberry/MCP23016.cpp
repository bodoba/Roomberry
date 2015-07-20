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

#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include "MCP23016.h"



#ifdef TARGET_RPI
#include <linux/i2c-dev.h>
#else
// Dummy define, wont work ...
#define I2C_SLAVE 0
#endif

MCP23016::MCP23016( uint8_t adr, const char *dev ) {
    address    = adr;
    i2cDevice  = dev;
    errorCount = 0;
    lastStatus = TRUE;
}

void MCP23016::reset(void) {
    i2cWrite(MCP23016_GP0,    0x0000);
    i2cWrite(MCP23016_IODIR0, 0xFFFF);
    i2cWrite(MCP23016_IPOL0,  0x0000);
}


void MCP23016::setMode(uint16_t mode) {
    if (!i2cWrite(MCP23016_IODIR0, mode)) {
        fprintf(stderr, "[ERROR] MCP23016::setMode %s\n", i2cErrorString);
    }
}

void MCP23016::setLatch(uint16_t mode) {
    if ( !i2cWrite(MCP23016_OLAT0, mode) ){
        fprintf(stderr, "[ERROR] MCP23016::setLatch %s\n", i2cErrorString);
    }
}

void MCP23016::setOutput(uint16_t value) {
    if (!i2cWrite(MCP23016_GP0, value) ){
        fprintf(stderr, "[ERROR] MCP23016::setOutput %s\n", i2cErrorString);
    }
}

void MCP23016::setPolarity(uint16_t polarity){
    if ( !i2cWrite(MCP23016_GP0, polarity) ){
        fprintf(stderr, "[ERROR] MCP23016::setPolarity %s\n", i2cErrorString);
    }
}

void MCP23016::readInput(uint16_t *in){
    uint8_t data[2];
    if(!i2cRead(MCP23016_GP0, data)) {
        fprintf(stderr, "[ERROR] MCP23016::readInput %s\n", i2cErrorString);
    } else {
      *in = WORD(data[1], data[0]);
    }
}

bool MCP23016::i2cWrite( uint8_t reg, uint16_t data ) {
    int8_t buf[3];
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
            buf[0] = reg;
            buf[1] = (data & 0xFF); // LSB
            buf[2] = (data >> 8);   // MSB
            if (write(fd, buf, 3) != 3 ) {
                sprintf(i2cErrorString, "Failed to write to device %s; Register 0x%02x Data 0x%04x: %s", i2cDevice, reg, data, ::strerror(errno));
            } else {
                success = TRUE;
            }
            close(fd);
        }
    }
    return(success);
}

bool MCP23016::i2cRead (uint8_t reg, uint8_t *data) {
    bool success = FALSE;
    i2cErrorString[0] = (char)0;
    int8_t count = 0;
    int fd = open(i2cDevice, O_RDWR);
    
    if (fd < 0) {
        sprintf(i2cErrorString, "Failed to open device %s: %s", i2cDevice, strerror(errno));
    } else {
        if (ioctl(fd, I2C_SLAVE, address) < 0) {
            sprintf(i2cErrorString, "Failed to select device %s: %s", i2cDevice, strerror(errno));
        } else {
            data[0] = reg;
            write(fd, data, 1);
            count = read(fd, data, 2);
            if (count < 0) {
                sprintf(i2cErrorString, "Failed to read device %s(%d): %s", i2cDevice, count, ::strerror(errno));
            } else {
                if (count != 2) {
                    sprintf(i2cErrorString, "Short read  from device %s, expected 2, got %d", i2cDevice, count);
                } else {
                    success = TRUE;
                }
            }
        }
        close(fd);
    }
    return(success);
}
