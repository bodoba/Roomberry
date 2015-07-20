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
#include "Roomberry.h"

int main ( int argc, char* argv[] ) {
    roomberryNote song[16];
    
    Roomberry Roomba("/dev/ttyAMA0");
    Roomba.safeMode();
    
    while ( TRUE ) {
        for (uint8_t note=31; note<=105; note++) {
            song[0].note = note;
            song[0].duration = 16;

            song[1].note = note+1;
            song[1].duration = 16;

            song[2].note = note+2;
            song[2].duration = 16;
            
            printf("Note: %d\n", note);
            
            if ( Roomba.uploadSong(1,3,song) ) {
                Roomba.playSong(1);
            } else {
                printf("ERROR\n");
            }
            sleep(1);
        }
    }
    return(0);
}
