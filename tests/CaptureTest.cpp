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
/** @file
 * \addtogroup Utilities
 * 
 * CaptureTest
 * -----------
 * This utility allows to test and calibrate the object recocnition
 */

#include <stdio.h>
#include <time.h>
#include <sys/time.h>

#include "RoboCam.h"

#define VIDEO_WIDTH    320
#define VIDEO_HEIGHT   240

uint64_t getUsecTime(void);

int main(int argc, char* argv[])
{
    RoboCam cam(VIDEO_WIDTH, VIDEO_HEIGHT, TRUE);
    int x, y ,size, key;
    int frames = 0;
    
    uint64_t start, end;
    
    start = getUsecTime();
    
    while (1) {
        if ( cam.getObjectPosition( 100, &x, &y, &size, &key ) ) {
            printf ( "S.%04d X.%04d Y.%04d\n", size, x, y);
        }
        frames++;
        if ( key == 27 ) {
            break;
        }
    }
    
    end = getUsecTime();
    
    printf("FPS: %2.3f\n", ((float)frames/(float)((end-start)/1000000)));
    
    return 0;
}

/** get time in microseconds
 *
 * \return current time of day in microseconds
 */

uint64_t getUsecTime(void) {
    struct timeval tp;
    uint64_t sec, usec;
    
    gettimeofday( &tp, NULL );
    sec  = (uint64_t)( tp.tv_sec );
    usec = (uint64_t)(( tp.tv_usec ));
    return ( sec * 1000000 + usec);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////
