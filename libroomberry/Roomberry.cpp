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
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <unistd.h>
#include <stdint.h>

#include "Roomberry.h"

#define DELAY(a) usleep(1000*a)

#ifndef WORD
#define WORD(a,b)    ((b&0x00ff)|((a<< 8)&0xff00))
#define HIGH_BYTE(a) ((a&0xff00)>>8)
#define LOW_BYTE(a)  (a&0x00ff)
#endif


/* ------------------------------------------------------------------------------ *
 * Initialize Roomba
 * ------------------------------------------------------------------------------ */

Roomberry::Roomberry ( const char* tty ) {
    struct termios tio;
    
    // intitialize internal state variables
    motorState        = 0;
    ledState          = 0;
    powerLedColor     = 0;
    powerLedIntensity = 0;

    ttyFd = open(tty, O_RDWR | O_NOCTTY | O_NDELAY );
    if (ttyFd <0) {perror(tty); exit(-1); }
    
    if (tcgetattr(ttyFd, &tio) < 0) {
        perror("Init: Couldn't get term attributes");
        exit(-1);
    }

    cfsetispeed(&tio, ROOMBERRY_BAUDRATE);
    cfsetospeed(&tio, ROOMBERRY_BAUDRATE);
    
    
    // 8N1
    tio.c_cflag &= ~PARENB;
    tio.c_cflag &= ~CSTOPB;
    tio.c_cflag &= ~CSIZE;
    tio.c_cflag |= CS8;
    
    tio.c_cflag &= ~CRTSCTS; // no flow control
    tio.c_cflag    |= CREAD | CLOCAL;  // turn on READ & ignore ctrl lines
    tio.c_iflag    &= ~(IXON | IXOFF | IXANY); // turn off s/w flow ctrl
    tio.c_lflag    &= ~(ICANON | ECHO | ECHOE | ISIG); // make raw
    tio.c_oflag    &= ~OPOST; // make raw
    
    tio.c_cc[VMIN]  = 26;
    tio.c_cc[VTIME] = 2;           // FIXME: not sure about this
    
    if ( tcsetattr(ttyFd, TCSANOW, &tio) < 0 ) {
        perror("Init: Couldn't set term attributes");
        exit (-1);
    }
    
    if ( ! passiveMode() ) {
        exit (-1);
    }
}

/* ------------------------------------------------------------------------------ *
 * Mode Commands
 * ------------------------------------------------------------------------------ */

bool Roomberry::passiveMode(void) {
    uint8_t buffer[] = { 128 };
    bool success = sendCommand ( 1, 0, buffer );
    DELAY(20);
    return ( success );
}

bool Roomberry::fullMode(void) {
    uint8_t buffer[] = { 132 };
    bool success = sendCommand ( 1, 0, buffer );
    DELAY(20);
    return ( success );
}

bool Roomberry::safeMode(void) {
    uint8_t buffer[] = { 131 };
    bool success = sendCommand ( 1, 0, buffer );
    DELAY(20);
    return ( success );
}

bool Roomberry::powerDown(void) {
    uint8_t buffer[] = {133};
    bool success = sendCommand ( 1, 0, buffer );
    DELAY(20);
    return ( success );
}

/* ------------------------------------------------------------------------------ *
 * LED Control
 * ------------------------------------------------------------------------------ */

bool Roomberry::printLed(const char* string ) {
    char buffer[5];
    bool success = TRUE;
    
    buffer[0] = 164;
    buffer[1] = string[0];
    buffer[2] = string[1];
    buffer[3] = string[2];
    buffer[4] = string[3];

    if ( write ( ttyFd, buffer, 5) != 5 ) {
        perror("printLed");
        success = FALSE;
    }
    return(success);
}

bool Roomberry::powerLed(uint8_t color, uint8_t intensity) {
    uint8_t buffer[] = { 139, ledState, 0, 0 };

    powerLedColor = color;
    powerLedIntensity = intensity;
    buffer[2] = powerLedColor;
    buffer[3] = powerLedIntensity;
    return ( sendCommand ( 4, 0, buffer ) );
}



/* ------------------------------------------------------------------------------ *
 * Private Helper
 * ------------------------------------------------------------------------------ */

bool Roomberry::sendCommand( uint8_t sendBytes, uint8_t receiveBytes, uint8_t* buffer) {
    bool success=FALSE;
    if ( write ( ttyFd, buffer, sendBytes) != sendBytes ) {
        perror("sendCommand(write)");
        success = FALSE;
    } else {
        if ( receiveBytes ) {
            DELAY(35);
            if ( read(ttyFd, buffer, receiveBytes) != receiveBytes ) {
                perror("sendCommand(read)");
            } else {
                success=TRUE;
            }
        } else {
            success=TRUE;
        }
    }
    return(success);
}

/* ------------------------------------------------------------------------------ *
 * Read bumper Status
 * ------------------------------------------------------------------------------ */

bool Roomberry::getBumper(roomberryBumper_t *bumper) {
    uint8_t buffer[] = { 142, 7 }; // Read Bumps and Wheel Drops
    bool success = FALSE;
    
    if (sendCommand ( 2, 1, buffer )){
        bumper->right = ( buffer[0] & 1 );
        bumper->left  = ( buffer[0] & 2 );
        success = TRUE;
    }
    return(success);
}

bool Roomberry::getLightBumper( roomberryLightBumper_t *lightBumper ) {
    uint8_t buffer[] = { 142, 45 }; // Read Light Bumper
    bool success = FALSE;
    
    if (sendCommand ( 2, 1, buffer )) {
        lightBumper->left        = buffer[0] &  1;
        lightBumper->frontLeft   = buffer[0] &  2;
        lightBumper->centerLeft  = buffer[0] &  4;
        lightBumper->centerRight = buffer[0] &  8;
        lightBumper->frontRight  = buffer[0] & 16;
        lightBumper->right       = buffer[0] & 32;
        success = TRUE;
    }
    return(success);
}

bool Roomberry::getLightBumpSignal ( uint8_t pID, uint16_t *signal ) {
    uint8_t buffer[] = { 142, pID };
    bool success = FALSE;
    
    if (sendCommand ( 2, 2, buffer )) {
        *signal = WORD(buffer[0], buffer[1]);
        success = TRUE;
    }
    return(success);
}

bool Roomberry::getLightBumpLeftSignal ( uint16_t *signal ) {
    return ( getLightBumpSignal( 46, signal ) );
}

bool Roomberry::getLightBumpFrontLeftSignal ( uint16_t *signal ) {
    return ( getLightBumpSignal( 47, signal ) );
}

bool Roomberry::getLightBumpCenterLeftSignal ( uint16_t *signal ) {
    return ( getLightBumpSignal( 48, signal ) );
}

bool Roomberry::getLightBumpCenterRightSignal ( uint16_t *signal ) {
    return ( getLightBumpSignal( 49, signal ) );
}

bool Roomberry::getLightBumpFrontRightSignal ( uint16_t *signal ) {
    return ( getLightBumpSignal( 50, signal ) );
}

bool Roomberry::getLightBumpRightSignal ( uint16_t *signal ) {
    return ( getLightBumpSignal( 51, signal ) );
}

bool Roomberry::getLightBumperSignals( roomberryLightBumperSignals_t *lightBumperSignals ) {
    uint8_t buffer[] = { 142, 106, 0,0,0,0,0,0,0,0,0,0 };
    bool success = FALSE;
    
    if (sendCommand ( 2, 12, buffer )) {
        lightBumperSignals->left        = WORD(buffer[0],  buffer[1]);
        lightBumperSignals->frontLeft   = WORD(buffer[2],  buffer[3]);
        lightBumperSignals->centerLeft  = WORD(buffer[4],  buffer[5]);
        lightBumperSignals->centerRight = WORD(buffer[6],  buffer[7]);
        lightBumperSignals->frontRight  = WORD(buffer[8],  buffer[9]);
        lightBumperSignals->right       = WORD(buffer[10], buffer[11]);
        success=TRUE;
    }
    return (success);
}

bool Roomberry::getAllBumper (roomberryBumper_t *bumper,
                              roomberryLightBumper_t *lightBumper,
                              roomberryLightBumperSignals_t *lightBumperSignals ) {
    uint8_t buffer[] = { 149, 3, 7, 45, 106, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
    bool success = FALSE;
    
    if (sendCommand ( 5, 14, buffer )) {
        bumper->right = ( buffer[0] & 1 );
        bumper->left  = ( buffer[0] & 2 );

        lightBumper->left        = buffer[1] &  1;
        lightBumper->frontLeft   = buffer[1] &  2;
        lightBumper->centerLeft  = buffer[1] &  4;
        lightBumper->centerRight = buffer[1] &  8;
        lightBumper->frontRight  = buffer[1] & 16;
        lightBumper->right       = buffer[1] & 32;
        
        lightBumperSignals->left        = WORD(buffer[2],  buffer[3]);
        lightBumperSignals->frontLeft   = WORD(buffer[4],  buffer[5]);
        lightBumperSignals->centerLeft  = WORD(buffer[6],  buffer[7]);
        lightBumperSignals->centerRight = WORD(buffer[8],  buffer[9]);
        lightBumperSignals->frontRight  = WORD(buffer[10], buffer[11]);
        lightBumperSignals->right       = WORD(buffer[12], buffer[13]);
        success=TRUE;
    }
    return (success);
}

bool Roomberry::getSensors ( roomberryBumper_t *bumper,
                             roomberryLightBumper_t *lightBumper,
                             roomberryLightBumperSignals_t *lightBumperSignals,
                             uint16_t *encRight, uint16_t *encLeft ) {
    
    uint8_t buffer[] = { 149, 5, 7, 45, 106, 43, 44, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
    bool success = FALSE;
    
    if (sendCommand ( 7, 18, buffer )) {
        bumper->right = ( buffer[0] & 1 );
        bumper->left  = ( buffer[0] & 2 );
        
        lightBumper->left        = buffer[1] &  1;
        lightBumper->frontLeft   = buffer[1] &  2;
        lightBumper->centerLeft  = buffer[1] &  4;
        lightBumper->centerRight = buffer[1] &  8;
        lightBumper->frontRight  = buffer[1] & 16;
        lightBumper->right       = buffer[1] & 32;
        
        lightBumperSignals->left        = WORD(buffer[2],  buffer[3]);
        lightBumperSignals->frontLeft   = WORD(buffer[4],  buffer[5]);
        lightBumperSignals->centerLeft  = WORD(buffer[6],  buffer[7]);
        lightBumperSignals->centerRight = WORD(buffer[8],  buffer[9]);
        lightBumperSignals->frontRight  = WORD(buffer[10], buffer[11]);
        lightBumperSignals->right       = WORD(buffer[12], buffer[13]);
        
        *encRight = WORD(buffer[14], buffer[15]);
        *encLeft  = WORD(buffer[16], buffer[17]);
        
        success=TRUE;
    }
    return (success);
}
/* ------------------------------------------------------------------------------ *
 * Notice Movement
 * ------------------------------------------------------------------------------ */

bool Roomberry::getDistance( int16_t *distance ) {
    uint8_t buffer[] = { 142, 19 };
    bool success = FALSE;
    
    if (sendCommand ( 2, 2, buffer )) {
        *distance = WORD(buffer[0], buffer[1]);
        success = TRUE;
    }
    return(success);
}

bool Roomberry::getAngle( int16_t *angle ) {
    uint8_t buffer[] = { 142, 20 };
    bool success = FALSE;
    
    if (sendCommand ( 2, 2, buffer )) {
        *angle = WORD(buffer[0], buffer[1]);
        success = TRUE;
    }
    return(success);
}

bool Roomberry::getEncoderCounts( uint16_t *encRight, uint16_t *encLeft ) {
    uint8_t buffer[] = { 149, 2, 43, 44 };
    bool success = FALSE;
    
    if (sendCommand ( 4, 4, buffer )) {
        *encRight = WORD(buffer[0], buffer[1]);
        *encLeft  = WORD(buffer[0], buffer[1]);
        success = TRUE;
    }
    return(success);
}



/* ------------------------------------------------------------------------------ *
 * Get environemtnal data
 * ------------------------------------------------------------------------------ */
bool Roomberry::getBatteryData ( roomberryBatteryData_t *batteryData ) {
    uint8_t buffer[] = { 142, 3, 0,0,0,0,0,0,0,0 };
    bool success = FALSE;
    
    if (sendCommand ( 2, 10, buffer )) {
        batteryData->chargingState   = buffer[0];
        batteryData->voltage         = WORD(buffer[1],  buffer[2]);
        batteryData->current         = WORD(buffer[3],  buffer[4]);
        batteryData->temperature     = buffer[5];
        batteryData->batteryCharge   = WORD(buffer[6],  buffer[7]);
        batteryData->batteryCapacity = WORD(buffer[8],  buffer[9]);
        success=TRUE;
    }
    return (success);
}

/* ------------------------------------------------------------------------------ *
 * Control Motors
 * ------------------------------------------------------------------------------ */

bool Roomberry::turn(int16_t velocity, int16_t radius) {
    uint8_t buffer[] = { 137,
        (uint8_t)(HIGH_BYTE(velocity)), (uint8_t)(LOW_BYTE(velocity)),
        (uint8_t)(HIGH_BYTE(radius)),   (uint8_t)(LOW_BYTE(radius)) };
    return( sendCommand ( 5, 0, buffer ) );
}

bool Roomberry::driveStraight(int16_t velocity) {
    return( turn( velocity, 0x8000 ) );
}

bool Roomberry::spin(int16_t velocity, bool clockwise) {
    return( turn( velocity, clockwise ? -1 : 1 ) );
}

bool Roomberry::stop(void) {
    return( turn( 0, 0 ) );
}

void Roomberry::getWheelDirection ( roomberryDirection_t *rightWheel, roomberryDirection_t *leftWheel) {
    *rightWheel = rightWheelDirection;
    *leftWheel  = leftWheelDirection;
}

bool Roomberry::vacuum(bool on) {
    uint8_t buffer[] = {138, 0};
    if ( on ) {
        motorState = motorState | 0x02;
    } else {
        motorState = motorState & !0x02;
    }
    buffer[1] = motorState;
    return( sendCommand ( 2, 0, buffer ) );
}

bool Roomberry::sideBrush(bool on) {
    uint8_t buffer[] = {138, 0};
    if ( on ) {
        motorState = motorState | 0x01;
    } else {
        motorState = motorState & !0x01;
    }
    buffer[1] = motorState;
    return( sendCommand ( 2, 0, buffer ) );
}

/* ------------------------------------------------------------------------------ *
 * Sounds
 * ------------------------------------------------------------------------------ */

bool Roomberry:: uploadSong (uint8_t songNumber, uint8_t songLength, roomberryNote *song) {
    uint8_t buffer[20];

    buffer[0] = 140;
    buffer[1] = songNumber;
    buffer[2] = songLength;
    for ( uint8_t i=0; i<songLength; i++ ) {
        buffer[(2*i)+3] = song[i].note;
        buffer[(2*i)+4] = song[i].duration;
    }
    return(sendCommand((songLength*2)+3, 0, buffer));
}

bool Roomberry::playSong(uint8_t songNumber) {
    uint8_t buffer[2];
    
    buffer[0] = 141;
    buffer[1] = songNumber;
    return(sendCommand(2, 0, buffer));
}







