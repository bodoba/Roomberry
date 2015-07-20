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

#ifndef DISPLAY_H
#define DISPLAY_H

#include <stdio.h>
#include <time.h>
#include <sys/time.h>
#include <ncurses.h>
#include <string.h>
#include "Roomberry.h"
#include "EnvData.h"
#include "Command.h"

#define K_ESC     27

#define K_SPACE   32
#define K_COMMA   44
#define K_DOT     46

#define K_UP      65
#define K_DOWN    66
#define K_RIGHT   67
#define K_LEFT    68



class Display {
public:
    Display( EnvData *data, CommandQ *myCmd, int sMin );
    int update( void );
    
private:
    EnvData       *envData;
    uint16_t      sonarMin;
    CommandQ      *cmd;
    uint64_t      *argQ;
    command_t     *cmdQ;
    
    unsigned int frames;
    struct timeval startTime;
};

#endif
