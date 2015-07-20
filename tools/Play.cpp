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
    Roomberry      Roomba("/dev/ttyAMA0");
    roomberryNote  song[16];
    uint8_t        songLength=0;
    uint16_t       waitTime=0;
    
    
    for ( uint8_t i=1; i<argc && songLength<16 ; i=i+2, songLength++ ) {
        song[songLength].note     = atoi(argv[i]);
        song[songLength].duration = atoi(argv[i+1]);
        waitTime+=song[songLength].duration;
    }

    printf ( "Song length: %02d\n", songLength );
    printf ( "  #   Note\tDuration\n");
    
    for ( uint8_t i=0; i<songLength; i++ ) {
        printf ( "  %02d: %03d\t%03d\n", i, song[i].note, song[i].duration );
    }
    
    Roomba.safeMode();
    Roomba.uploadSong(1,songLength,song);
    Roomba.playSong(1);
    sleep(waitTime/64 + 1);
    Roomba.passiveMode();
    
    return(0);
}
