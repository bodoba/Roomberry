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

#include <stdio.h>
#include <time.h>
#include <sys/time.h>
#include <string.h>

#include "Display.h"
#include "Roomberry.h"
#include "EnvData.h"
#include "Command.h"
#include "CmdLine.h"

#define LED_RIGHT_RED    MCP23016_GPIO0_7
#define LED_RIGHT_GREEN  MCP23016_GPIO0_6

#define LED_LEFT_RED     MCP23016_GPIO0_5
#define LED_LEFT_GREEN   MCP23016_GPIO0_4

#define LED_WSK_LEFT     MCP23016_GPIO0_3
#define LED_WSK_RIGHT    MCP23016_GPIO0_2

#define PIR_RIGHT        MCP23016_GPIO1_0
#define PIR_LEFT         MCP23016_GPIO1_1

#define BTN_WSK_RIGHT    MCP23016_GPIO1_6
#define BTN_WSK_LEFT     MCP23016_GPIO1_7


#define DEFAULT_SPEED 75

#define RAND(a) (rand()%a)

void executeCommand( CommandQ *cmdQueue, Roomberry *roomba );
bool kbdControl( int key, CommandQ *cmdQueue );

int main(int argc, char* argv[])
{
    
    srand(time(NULL));                 // initialize random number generator
    CommandQ cmdQueue;                 // Create command queue
    cmdQueue.setSpeed(DEFAULT_SPEED);
    
    // parse command line
    CmdLine opt(argc, argv );

    // Roomba connected via serial interface
    Roomberry  Roomba("/dev/ttyAMA0");
    
    // Inititlize hardware sensors (on Roomba and connected via I2C)
    EnvData    *sensorData;
    if ( opt.useRoomba() ) {
        sensorData = new EnvData ( &Roomba, opt.useCamera(), opt.showPreviewWindow() );
    } else {
        sensorData = new EnvData ( NULL, opt.useCamera(), opt.showPreviewWindow() );
    }
    MCP23016 ioExp   = MCP23016(0x20, "/dev/i2c-1");   // IO Expander to control LEDs

    Display display(sensorData, &cmdQueue, SONAR_MIN);
    
    Roomba.fullMode();
    
    ioExp.reset();
    ioExp.setMode(PIR_RIGHT|PIR_LEFT|BTN_WSK_LEFT|BTN_WSK_RIGHT);

    uint16_t out=0;

    
    // Enter main Loop
    bool run = TRUE;
    while (run) {
        // read sensor data
        sensorData->readSensors();
        
        
        out=0;
        if ( sensorData->getPirLeft() ) {
            out |= LED_LEFT_RED;
        }
        
        if ( sensorData->getPirRight() ) {
            out |= LED_RIGHT_RED;
        }
        
        if ( sensorData->objectLeft() && sensorData->objectRight() ) {
            out |= LED_LEFT_GREEN;
            out |= LED_RIGHT_GREEN;

            switch ( cmdQueue.getCurrent() ) {
                case CmdForward:
                    cmdQueue.insert(CmdSpinRight,  100+RAND(100));
                    cmdQueue.insert(CmdReverse,   50+RAND(50));
                    break;
                case CmdSpinRight:
                case CmdSpinLeft:
                    cmdQueue.incArg(10);
                    break;
            }
            
        } else if ( sensorData->objectLeft() ) {
            out |= LED_LEFT_GREEN;
            switch ( cmdQueue.getCurrent() ) {
                case CmdForward:
                    cmdQueue.insert(CmdSpinRight,  20+RAND(20));
                    cmdQueue.insert(CmdReverse,   100+RAND(100));
                    break;
                case CmdSpinRight:
                case CmdSpinLeft:
                    cmdQueue.incArg(10);
                    break;
            }
        } else if ( sensorData->objectRight() ) {
            out |= LED_RIGHT_GREEN;
            switch ( cmdQueue.getCurrent() ) {
                case CmdForward:
                    cmdQueue.insert(CmdSpinLeft,  20+RAND(20));
                    cmdQueue.insert(CmdReverse,   100+RAND(100));
                    break;
                case CmdSpinRight:
                case CmdSpinLeft:
                    cmdQueue.incArg(10);
                    break;
            }
        }

        if ( ! sensorData->getWhiskerLeft() ) {
            out |= LED_WSK_LEFT;
        }
        
        if ( ! sensorData->getWhiskerRight() ) {
            out |= LED_WSK_RIGHT;
        }
        
        ioExp.setOutput(out);
        
        run = kbdControl( display.update(), &cmdQueue );
        
        cmdQueue.advanceQueue();

        executeCommand ( &cmdQueue, &Roomba );
        
    }
    
    sensorData->stop();
    
    Roomba.stop();
    Roomba.passiveMode();
    endwin();
    
    return 0;
}


bool kbdControl( int key, CommandQ *cmdQueue ) {
    bool run = TRUE;
    uint16_t spd = cmdQueue->getSpeed();
    
    if ( key ) {
        switch ( key ) {
            case K_ESC:  // Esc
                run = FALSE;
                break;
            case K_SPACE:  // Spacebar
                cmdQueue->clear();
                cmdQueue->insert(CmdStop, 0);
                break;
            case K_DOT:  // . >
                if ( spd < 500 ) {
                    cmdQueue->setSpeed(spd+5);
                } else {
                    cmdQueue->setSpeed(500);
                }
                break;
            case K_COMMA:  // , <
                if ( spd > 15 ) {
                    cmdQueue->setSpeed(spd-5);
                } else {
                    cmdQueue->setSpeed(15);
                }
                break;
            case K_UP:  // Arrow UP
                cmdQueue->clear();
                cmdQueue->insert(CmdForward, 0);
                break;
            case K_DOWN:  // Arrow DOWN
                cmdQueue->clear();
                cmdQueue->insert(CmdReverse, 0);
                break;
            case K_RIGHT:  // Arrow RIGHT
                cmdQueue->clear();
                cmdQueue->insert(CmdSpinRight, 0);
                break;
            case K_LEFT:  // Arrow LEFT
                cmdQueue->clear();
                cmdQueue->insert(CmdSpinLeft, 0);
                break;
        }
    }
    return run;
}

void executeCommand( CommandQ *cmdQueue, Roomberry *roomba ) {
    static command_t oldCmd   = CmdNOP;
    static uint16_t  oldSpeed = DEFAULT_SPEED;

    command_t currentCmd   = cmdQueue->getCurrent();
    uint16_t  currentSpeed = cmdQueue->getSpeed();
    
    if ( currentCmd != oldCmd || currentSpeed != oldSpeed ) {
        switch ( currentCmd ) {
            case CmdStop:
                roomba->stop();
                break;
                
            case CmdForward:
                roomba->driveStraight(currentSpeed);
                break;
                
            case CmdReverse:
                roomba->driveStraight(-1 * currentSpeed);
                break;
                
            case CmdSpinRight:
                roomba->spin( currentSpeed, TRUE );
                break;
                
            case CmdSpinLeft:
                roomba->spin( currentSpeed, FALSE );
                break;
                
            default:
                roomba->stop();
                break;
        }
        oldCmd   = currentCmd;
        oldSpeed = currentSpeed;
    }
}


