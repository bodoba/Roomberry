/*
 * ---------------------------------------------------------------------------------------
 * Copyright 2014 by Bodo Bauer <bb@bb-zone.com>
 *
 *
 * This file is part of I2C Arduino-Slave Library.
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
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "MCP23016.h"

#define LED_RIGHT_RED    MCP23016_GPIO0_7
#define LED_RIGHT_GREEN  MCP23016_GPIO0_6

#define LED_LEFT_RED     MCP23016_GPIO0_5
#define LED_LEFT_GREEN   MCP23016_GPIO0_4

#define PIR_RIGHT        MCP23016_GPIO1_0
#define PIR_LEFT         MCP23016_GPIO1_1

int main(int argc, char **argv)
{
    uint16_t in=0, out=0;
    bool blink = FALSE;
    
    MCP23016 ioExp = MCP23016(0x20, "/dev/i2c-1");
    ioExp.reset();
    // Set pin mode
    ioExp.setMode(PIR_RIGHT|PIR_LEFT);
    
    while(TRUE) {
        ioExp.readInput(&in);
        out=0;
        if ( in & PIR_LEFT ) {
            out |= LED_LEFT_RED;
        }
        
        if ( in & PIR_RIGHT ) {
            out |= LED_RIGHT_RED;
        }

        if (blink) {
            blink = FALSE;
            out |= LED_RIGHT_GREEN;
        } else {
            out |= LED_LEFT_GREEN;
            blink = TRUE;
        }
        ioExp.setOutput(out);
        usleep(100000);
    }
    return(0);
}
