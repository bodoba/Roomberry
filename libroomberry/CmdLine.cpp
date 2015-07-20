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

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "CmdLine.h"

CmdLine::CmdLine ( int argc, char *argv[] ) {
    UseCamera = FALSE;
    ShowPreviewWindow = FALSE;
    UseRoomba = TRUE;    
    
    for ( int i=1; i<argc; i++ ) {
        if ( strcmp( argv[i], "-p" ) == 0 ) {
            UseCamera = TRUE;
            ShowPreviewWindow = TRUE;
        }
        if ( strcmp( argv[i], "-v" ) == 0 ) {
            UseCamera = TRUE;
        }
        if ( strcmp( argv[i], "-R" ) == 0 ) {
            UseRoomba = FALSE;
        }
        
        if ( strcmp( argv[i], "-h" ) == 0 ) {
            printf("%s [options]\n", argv[0] );
            printf(" Available Options:\n");
            printf("  -p           Show preview window (implies -v)\n");
            printf("  -R           Disable Roomba Sensors\n");
            printf("  -v           Enable Camera\n");
            printf("  -h           Show this help\n");
            exit(0);
        }
    }
}

bool CmdLine::useRoomba(void) {
    return UseRoomba;
}

bool CmdLine::useCamera(void) {
    return UseCamera;
}

bool CmdLine::showPreviewWindow(void) {
    return ShowPreviewWindow;
}