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

#include <string.h>
#include "EventLog.h"

int main ( int argc, char* argv[] ) {
    EventLog  myLog(100);
    char      logText[100];
    uint16_t  index;
    logEntry_t entry;
    
    for ( int i=1; i<=198; i++ ) {
        sprintf ( logText, "Log entry number %d 0x%x", i, i );
        index = myLog.addLogEntry( logText );
    }
    
    for ( int i=0; i<100; i++ ) {
        entry = myLog.getLogEntry( i );
        printf("[%04d] %04d %020llu - %s\n", i, entry.count, entry.timestamp, entry.text );
    }
    
    printf ( "First: %04d   Last: %04d\n", myLog.getIndexFirst(), myLog.getIndexLast() );
    
}
