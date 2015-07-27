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

#include "Display.h"

extern pthread_mutex_t camDataMutex;


Display::Display( EnvData *data, CommandQ *myCmd, int sMin ) {
    envData     = data;
    sonarMin    = sMin;
    cmd         = myCmd;
    
    cmdQ = cmd->getCmdQ();
    argQ = cmd->getArgQ();
    
    initscr();
    start_color();
    init_pair(1, COLOR_BLACK, COLOR_WHITE);
    init_pair(2, COLOR_BLUE, COLOR_WHITE);
    init_pair(3, COLOR_WHITE, COLOR_RED);
    
    bkgd(COLOR_PAIR(1));
    color_set(1, 0);
    clear();
    noecho();
    timeout(0);
    cbreak();
    curs_set(0);
    
    mvprintw (  5, 5, "      WSK\\        ooo OOOOOOO ooo        /WSK       ");
    mvprintw (  6, 5, "          \\   oOO xxxx IR|IR xxxx OOo   /           ");
    mvprintw (  7, 5, "           oOO <+---------+---------+> OOo           ");
    mvprintw (  8, 5, "        oOO PIR |D dddddd |H   aaaa | PIR OOo        ");
    mvprintw (  9, 5, "      oOO <-----+---------+---------+-----> OOo      ");
    mvprintw ( 10, 5, "    oOO xxxx IR|          ^          |IR xxxx OOo    ");
    mvprintw ( 11, 5, "   oOO <-------+        +---+        +-------> OOo   ");
    mvprintw ( 12, 5, "  oOO xx Sonar|        <|000|>        |Sonar xx OOo  ");
    mvprintw ( 13, 5, " oOO <--------+         +---+         +--------> OOo ");
    mvprintw ( 14, 5, " oOO xxxx IR|             v             |IR xxxx OOo ");
    mvprintw ( 15, 5, " oOO <------+                           +------> OOo ");
    mvprintw ( 16, 5, " oOO                                             OOo ");
    mvprintw ( 17, 5, " oOO                                             OOo ");
    mvprintw ( 18, 5, "  oOO                                           OOo  ");
    mvprintw ( 19, 5, "   oOO                                         OOo   ");
    mvprintw ( 20, 5, "    oOO         +-------------------+         OOo    ");
    mvprintw ( 21, 5, "      oOO       | R O O M B E R R Y |       OOo      ");
    mvprintw ( 22, 5, "        oO      +-------------------+     OOo        ");
    mvprintw ( 23, 5, "           oOO    Sensor FPS Video     OOo           ");
    mvprintw ( 24, 5, "               oOO                 OOo               ");
    mvprintw ( 25, 5, "                   ooo OOO OOO ooo                   ");
    refresh();

    frames = 0;
    gettimeofday(&startTime, NULL);
}

int Display::update() {
    struct timeval end, diff;
    int key = getch();
    
    gettimeofday(&end, NULL);
    timersub(&end, &startTime, &diff);
    
    color_set(2,0);
    mvprintw ( 24, 25, "%2.2f", ((float)frames / (float)diff.tv_sec));
    frames++;

    envData->readCamData();
    mvprintw ( 24, 33, "%2.2f", envData->getCamFPS() );
    
    // Distance & Angle
    mvprintw (  8, 24, "%06d", envData->getDistanceAccumulated() );
    mvprintw (  8, 36, "%04d", envData->getHeading() );

    // Light Bumper Signals
    color_set(envData->getLightBumperLeft() ? 3 : 2 ,0);
    mvprintw ( 14, 10, "%04d", envData->getLightBumperSignalLeft()  );
    color_set(envData->getLightBumperRight() ? 3 : 2 ,0);
    mvprintw ( 14, 49, "%04d", envData->getLightBumperSignalRight() );
    
    color_set(envData->getLightBumperFrontLeft() ? 3 : 2 ,0);
    mvprintw ( 10, 13, "%04d", envData->getLightBumperSignalFrontLeft()  );
    color_set(envData->getLightBumperFrontRight() ? 3 : 2 ,0);
    mvprintw ( 10, 46, "%04d", envData->getLightBumperSignalFrontRight() );
    
    color_set(envData->getLightBumperCenterLeft() ? 3 : 2 ,0);
    mvprintw ( 06, 23, "%04d", envData->getLightBumperSignalCenterLeft()  );
    color_set(envData->getLightBumperCenterRight() ? 3 : 2 ,0);
    mvprintw ( 06, 34, "%04d", envData->getLightBumperSignalCenterRight() );
    
    // Whisker
    color_set(envData->getWhiskerLeft() ? 3 : 1 ,0);
    mvprintw (  5, 11, "WSK" );
    color_set(envData->getWhiskerRight() ? 3 : 1 ,0);
    mvprintw (  5, 48, "WSK" );
    
    // light Bumper
    color_set(envData->getLightBumperLeft() ? 3 : 1 ,0);
    mvprintw ( 13,  6, "o" );
    mvprintw ( 14,  6, "o" );
    mvprintw ( 15,  6, "o" );
    
    color_set(envData->getLightBumperRight() ? 3 : 1 , 0);
    mvprintw ( 13, 56, "o" );
    mvprintw ( 14, 56, "o" );
    mvprintw ( 15, 56, "o" );
    
    color_set(envData->getLightBumperFrontLeft() ? 3 : 1 , 0);
    mvprintw (  9, 11, "o" );
    mvprintw ( 10,  9, "o" );
    mvprintw ( 11,  8, "o" );
    
    color_set(envData->getLightBumperFrontRight() ? 3 : 1 , 0);
    mvprintw (  9, 51, "o" );
    mvprintw ( 10, 53, "o" );
    mvprintw ( 11, 54, "o" );
    
    color_set(envData->getLightBumperCenterLeft() ? 3 : 1 , 0);
    mvprintw (  5, 24, "o" );
    mvprintw (  6, 20, "o" );
    mvprintw (  7, 16, "o" );
    
    color_set(envData->getLightBumperCenterRight() ? 3 : 1 , 0);
    mvprintw (  5, 38, "o" );
    mvprintw (  6, 42, "o" );
    mvprintw (  7, 46, "o" );
    
    // Left and right Bumper
    color_set(envData->getBumperLeft() ? 3 : 1 ,0);
    mvprintw (  5, 23, "ooo OOO");
    mvprintw (  6, 19, "oOO"    );
    mvprintw (  7, 16, "oOO"    );
    mvprintw (  8, 13, "oOO"    );
    
    color_set(envData->getBumperRight() ? 3 : 1 ,0);
    mvprintw (  5, 32, "OOO ooo");
    mvprintw (  6, 39, "OOo"    );
    mvprintw (  7, 44, "OOo"    );
    mvprintw (  8, 47, "OOo"    );
    
    // PIR Sensors
    color_set(envData->getPirLeft() ? 3 : 1 ,0);
    mvprintw (  8, 17, "PIR"    );
    
    color_set(envData->getPirRight() ? 3 : 1 ,0);
    mvprintw (  8, 43, "PIR"    );
    
    // Sonar Sensors
    color_set( envData->getSonarLeft() < sonarMin ? 3 : 2 ,0);
    mvprintw (  12, 11, "%02d", envData->getSonarLeft() );
    
    color_set( envData->getSonarRight() < sonarMin ? 3 : 2 ,0);
    mvprintw (  12, 50, "%02d", envData->getSonarRight() );
    
    color_set(   2,  0 );
    mvprintw (  12, 30, "%03d", cmd->getSpeed() );
    
    
    // Show command Queue
    uint8_t cmdIndex = cmd->getIndex();
    for (int i = 0; i<4; i++) {
        if ( i<=cmdIndex ) {
            switch (cmdQ[i]) {
                case CmdNOP:
                    mvprintw( 16+i, 25, "[NOP %08ld]", (long)argQ[i] );
                    break;
                case CmdForward:
                    mvprintw( 16+i, 25, "[FWD %08ld]", (long)argQ[i] );
                    break;
                case CmdReverse:
                    mvprintw( 16+i, 25, "[REV %08ld]", (long)argQ[i] );
                    break;
                case CmdSpinRight:
                    mvprintw( 16+i, 25, "[RGT %08ld]", (long)argQ[i] );
                    break;
                case CmdSpinLeft:
                    mvprintw( 16+i, 25, "[LFT %08ld]", (long)argQ[i] );
                    break;
                case CmdStop:
                    mvprintw( 16+i, 25, "[STP %08ld]", (long)argQ[i] );
                    break;
                default:
                    mvprintw( 16+i, 25, "[??? %08ld]", (long)argQ[i] );
                    break;
            }
        } else {
            mvprintw( 16+i, 25, "[-/- --------]" );
        }
        mvprintw ( 16, 30, "%08ld",(long)(argQ[0]-cmd->getCmdAge()));
    }
    
    // State
    char r=' ', l=' ', f=' ', b=' ';
    switch (cmdQ[0]) {
        case CmdForward:
            f='^';
            break;
        case CmdReverse:
            b='v';
            break;
        case CmdSpinLeft:
            l='<';
            break;
        case CmdSpinRight:
            r='>';
            break;
        default:
            r='o', l='o', f='o', b='o';
            break;
    }
    mvprintw (  12, 34, "%c", r );
    mvprintw (  12, 28, "%c", l );
    mvprintw (  10, 31, "%c", f );
    mvprintw (  14, 31, "%c", b );

    // read keyboard
    if ( key > 0 ) {
        if ( key == K_ESC ) {
            // ESC handling
            int key2 = getch();
            if ( key2 > 0 ) {
                key = key2;
            }
        }
       // mvprintw ( 20, 30, "%03d", key );
    } else {
        key = 0;
    }
    
    // refresh the screen
    refresh();
    
    return key;
}
