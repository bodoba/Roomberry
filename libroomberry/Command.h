/*
 * ---------------------------------------------------------------------------------------
 * Copyright 2014 by Bodo Bauer <bb@bb-zone.com>
 *
 * This Roomba Open Interface class is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This Roomba Open Interface class is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this file.  If not, see <http://www.gnu.org/licenses/>.
 * --------------------------------------------------------------------------------------
 */

#ifndef COMMAND_H
#define COMMAND_H

#include <time.h>
#include <sys/time.h>
#include <stdint.h>

#ifndef TRUE
#define TRUE	(1==1)
#define FALSE	(0==1)
#endif

#define CMD_QSIZE 5

typedef enum {
    CmdNOP,
    CmdForward,
    CmdReverse,
    CmdSpinRight,
    CmdSpinLeft,
    CmdStop,
} command_t;

class CommandQ {
    
public:
    CommandQ( void );
    
    bool clear(void);
    bool add(command_t cmd,    uint32_t arg );
    bool insert(command_t cmd, uint32_t arg );
    bool incArg( uint32_t arg );
    
    command_t getCurrent(void);
    command_t advanceQueue(void);
    
    command_t *getCmdQ(void);
    uint64_t  *getArgQ(void);
    uint8_t   getIndex(void);
    uint64_t  getCmdAge(void);

    bool     setSpeed ( uint16_t spd );
    uint16_t getSpeed ( void );
    
private:
    command_t cmdQ[CMD_QSIZE];
    uint64_t  argQ[CMD_QSIZE];
    uint8_t   cmdIndex;
    uint64_t  cmdStartTime;
    uint16_t  speed;
    uint64_t  getUsecTime(void);
};

#endif