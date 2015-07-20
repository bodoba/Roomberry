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


/** Bumper Test
 * 
 * Read bumper and display results
 */
int main ( int argc, char* argv[] ) {
    roomberryLightBumper_t        lightBumper;
    roomberryBumper_t             bumper;
    roomberryLightBumperSignals_t lightBumperSignals;
    
    Roomberry Roomba("/dev/ttyAMA0");
    
    while ( TRUE ) {
        Roomba.getBumper( &bumper );
        Roomba.getLightBumper( &lightBumper );
        Roomba.getLightBumperSignals ( &lightBumperSignals );
        
        printf ( "%c%03d%c %c%03d%c %c%03d%c %c%c %c%03d%c %c%03d%c %c%03d%c\n",
                lightBumper.left ? '[' : ' ', lightBumperSignals.left, lightBumper.left ? ']' : ' ',
                lightBumper.frontLeft ? '[' : ' ', lightBumperSignals.frontLeft, lightBumper.frontLeft ? ']' : ' ',
                lightBumper.centerLeft ? '[' : ' ', lightBumperSignals.centerLeft, lightBumper.centerLeft ? ']' : ' ',
                bumper.left             ? '*' : ' ',
                bumper.right            ? '*' : ' ',
                lightBumper.centerRight ? '[' : ' ', lightBumperSignals.centerRight, lightBumper.centerRight ? ']' : ' ',
                lightBumper.frontRight  ? '[' : ' ', lightBumperSignals.frontRight, lightBumper.frontRight  ? ']' : ' ',
                lightBumper.right ? '[' : ' ' , lightBumperSignals.right, lightBumper.right ? ']' : ' ' );
        }
    return(0);
}
