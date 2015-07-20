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
#include <sys/time.h>
#include <sys/types.h>
#include "Roomberry.h"


uint64_t getUsecTime(void);


int main ( int argc, char* argv[] ) {
    Roomberry Roomba("/dev/ttyAMA0");
    uint16_t          speed = 250;
    int16_t           msec  = 100;
    uint64_t          endTime, now;
    roomberryBumper_t bumper;
    bool              stop = FALSE;
    
    if ( argc >= 3 ) {
        speed = atoi(argv[2]);
        if ( speed < 0 ) {
            speed = 0;
        } else if ( speed > 500 ) {
            speed = 500;
        }
    }

    if ( argc >= 2 ) {
        msec = atoi(argv[1]);
        if ( msec < 0 ) {
            speed *= -1;
            msec  *= -1;
        }
    }
    Roomba.safeMode();
    
    now     = getUsecTime();
    endTime = now + ( msec * 10000 );
    
    Roomba.driveStraight(speed);

    while ( ! stop ) {
        now =  getUsecTime();
        Roomba.getBumper ( &bumper );
        
        if ( now >= endTime || bumper.right || bumper.left ) {
            stop = TRUE;
        } else {
            usleep(10000);
        }
    }
    Roomba.stop();
    Roomba.passiveMode();

    if ( bumper.right & bumper.left ) {
        printf ("Emergency Stop! (Full frontal hit)\n");
    } else if ( bumper.right ) {
        printf ("Emergency Stop! (Right side hit)\n");
    } else if ( bumper.left ) {
        printf ("Emergency Stop! (Left side hit)\n");
    }
    
    return(0);
}

uint64_t getUsecTime(void) {
    struct timeval tp;
    uint64_t sec, usec;

    gettimeofday( &tp, NULL );
    sec  = (uint64_t)( tp.tv_sec );
    usec = (uint64_t)(( tp.tv_usec ));
    return ( sec * 1000000 + usec);
}

