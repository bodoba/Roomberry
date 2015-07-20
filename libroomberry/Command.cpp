/*
 * ---------------------------------------------------------------------------------------
 * Copyright 2015 by Bodo Bauer <bb@bb-zone.com>
 *
 * This class is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This class is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this file.  If not, see <http://www.gnu.org/licenses/>.
 * --------------------------------------------------------------------------------------
 */

#include "Command.h"

CommandQ::CommandQ( void ) {
    clear();
}

bool CommandQ::clear(void) {
    cmdIndex     = 0;
    cmdStartTime = 0;
    cmdQ[0]      = CmdNOP;
    argQ[0]      = 0;
    return TRUE;
}

bool CommandQ::add(command_t cmd, uint32_t arg ) {
    bool retval = FALSE;
    
    if ( cmdIndex < CMD_QSIZE-1 ) {
        cmdIndex++;
        cmdQ[cmdIndex] = cmd;
        argQ[cmdIndex] = (uint64_t)(arg*10000);
        retval = TRUE;
    }
    return retval;
}

bool CommandQ::insert(command_t cmd, uint32_t arg ) {
    bool retval = FALSE;
    
    if ( cmdIndex < CMD_QSIZE-1 ) {
        for ( int i=cmdIndex+1; i>0; i-- ) {
            cmdQ[i] = cmdQ[i-1];
            argQ[i] = argQ[i-1];
        }
        cmdIndex++;
        cmdQ[0] = cmd;
        argQ[0] = (uint64_t)(arg*10000);
        cmdStartTime = arg ? getUsecTime() : 0;
        retval = TRUE;
    }
    return retval;
}

bool CommandQ::incArg( uint32_t arg ) {
    argQ[0] += (uint64_t)(arg*10000);
    return TRUE;
}

command_t CommandQ::getCurrent(void) {
    return cmdQ[0];
}

command_t CommandQ::advanceQueue( void ) {
    if ( argQ[0] > 0 && cmdQ[0] != CmdNOP ) {
        if ( getCmdAge() > argQ[0] ) {
            for ( int i=0; i<cmdIndex; i++ ) {
                cmdQ[i] = cmdQ[i+1];
                argQ[i] = argQ[i+1];
            }
            cmdStartTime = argQ[0] ? getUsecTime() : 0;
            cmdQ[cmdIndex] = CmdNOP;
            argQ[cmdIndex] = 0;
            if ( cmdIndex ) {
                cmdIndex--;
            }
        }
    }
    return cmdQ[0];
}

command_t *CommandQ::getCmdQ(void) {
    return cmdQ;
}

uint64_t   *CommandQ::getArgQ(void) {
    return argQ;
}

uint8_t CommandQ::getIndex(void) {
    return cmdIndex;
}

uint64_t CommandQ::getCmdAge(void) {
    uint64_t age = 0;
    if ( cmdStartTime && (getUsecTime() > cmdStartTime) ) {
        age = getUsecTime() - cmdStartTime;
    }
    return age;
}

bool CommandQ::setSpeed ( uint16_t spd ) {
    speed = spd;
    return TRUE;
}

uint16_t CommandQ::getSpeed ( void ) {
    return speed;
}

uint64_t CommandQ::getUsecTime(void) {
    struct timeval tp;
    gettimeofday( &tp, NULL );
    return ( ( (uint64_t)tp.tv_sec * 1000000 ) + (uint64_t)tp.tv_usec );
}