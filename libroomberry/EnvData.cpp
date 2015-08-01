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

#include "EnvData.h"
pthread_mutex_t camDataMutex;

uint16_t encDifference( uint16_t oldReading, uint16_t newReding, int16_t velo );

EnvData::EnvData(Roomberry *myRoomba, bool useCam, bool pv ) {
    roomba             = myRoomba;
    showPreviewWindow  = pv;
    i2cIoExp      = new MCP23016(0x20, "/dev/i2c-1");
    i2cSonarLeft  = new SRF02(0x70, "/dev/i2c-1");
    i2cSonarRight = new SRF02(0x72, "/dev/i2c-1");

    camDataMutex = PTHREAD_MUTEX_INITIALIZER;
    
    if ( useCam ) {
        objDetected     = FALSE;
        objSize         = 0;
        objX            = 0;
        objY            = 0;
        cameraFrames    = 0;
        camData.showPreviewWindow = showPreviewWindow;
        gettimeofday(&cameraStartTime, NULL);
        camThreadID = pthread_create( &camThread, NULL, &cameraControl, (void *)&camData );
    } else {
        camThreadID = 0;
    }
    
    sonarRight = 1000;
    sonarLeft  = 1000;
    
    bumper.right            = FALSE;
    bumper.left             = FALSE;
    
    lightBumper.right       = FALSE;
    lightBumper.left        = FALSE;
    lightBumper.frontRight  = FALSE;
    lightBumper.frontLeft   = FALSE;
    lightBumper.centerRight = FALSE;
    lightBumper.centerLeft  = FALSE;
    
    lightBumperSignals.right       = 0;
    lightBumperSignals.left        = 0;
    lightBumperSignals.frontRight  = 0;
    lightBumperSignals.frontLeft   = 0;
    lightBumperSignals.centerRight = 0;
    lightBumperSignals.centerLeft  = 0;

    distance = 0;
    heading  = 0;
    encRight = 0;
    encLeft  = 0;
    
    i2cIoExp->reset();
    i2cIoExp->setMode(PIR_RIGHT|PIR_LEFT|BTN_WSK_LEFT|BTN_WSK_RIGHT);
}

void *cameraControl( void *tArg ) {
    camData_t *camData = (camData_t*) tArg;

    pthread_mutex_lock( &camDataMutex );
    
    camData->run    = TRUE;
    camData->frames = 0;
    
    pthread_mutex_unlock( &camDataMutex );

    RoboCam *cam = new RoboCam(VIDEO_WIDTH, VIDEO_HEIGHT, camData->showPreviewWindow);
    
    while ( camData->run ) {
        int          objX, objY, objSize, key;
        bool         objDetected;
        
        objDetected = cam->getObjectPosition( MIN_OBJ_SIZE, &objX, &objY, &objSize, &key) ;

        pthread_mutex_lock( &camDataMutex );
        camData->objDetected = objDetected;
        camData->objSize     = objSize;
        camData->objX        = objX;
        camData->objY        = objY;
        camData->frames++;
        pthread_mutex_unlock( &camDataMutex );

    }
    return NULL;
}

bool EnvData::readCamData( void ) {
    pthread_mutex_lock( &camDataMutex );
    objDetected  = camData.objDetected;
    objSize      = camData.objSize;
    objX         = camData.objX;
    objY         = camData.objY;
    cameraFrames = camData.frames;
    pthread_mutex_unlock( &camDataMutex );
    return TRUE;
}

uint16_t encDifference( uint16_t oldReading, uint16_t newReding, int16_t velo ) {
    uint16_t diff;
    
    return diff;
}

int EnvData::readSensors(void) {
    int key=0;
    // read sensor data
    if ( roomba ) {
        uint16_t encRightOld = encRight;
        uint16_t encLeftOld  = encLeft;
        
        roomba->getSensors (&bumper, &lightBumper, &lightBumperSignals,
                            &encRight, &encLeft );
    
    }
    
    sonarRight = i2cSonarRight->getRealRangeCm();
    sonarLeft  = i2cSonarLeft->getRealRangeCm();

    uint16_t in=0;
    i2cIoExp->readInput(&in);
    pirLeft      = in & PIR_LEFT;
    pirRight     = in & PIR_LEFT;
    whiskerLeft  = in & BTN_WSK_LEFT;
    whiskerRight = in & BTN_WSK_RIGHT;

    return key;
}

bool EnvData::object(void) {
    return ( objectLeft() || objectRight() || objectFront() );
}

bool EnvData::objectLeft(void) {
    return (   bumper.left
            || lightBumper.centerLeft
            || lightBumper.frontLeft
            || lightBumper.left
            || sonarLeft < SONAR_MIN
            || whiskerLeft );
}

bool EnvData::objectRight(void) {
    return (   bumper.right
            || lightBumper.centerRight
            || lightBumper.frontRight
            || lightBumper.right
            || sonarRight < SONAR_MIN
            || whiskerRight );
}

bool EnvData::objectFront(void) {
    return (   ( bumper.right && bumper.left )
            || ( lightBumper.centerRight && lightBumper.centerLeft )
            || ( whiskerRight && whiskerLeft ) );
}

float  EnvData::getCamFPS(void) {
    struct timeval end, diff;
    gettimeofday(&end, NULL);
    timersub(&end, &cameraStartTime, &diff);
    
    return ( (float)((float)cameraFrames / (float)diff.tv_sec));
}

void EnvData::stop (void) {
    camData.run = FALSE;
}

uint16_t EnvData::getHeading(void) {
    return heading;
}

uint16_t EnvData::resetHeading(void) {
    uint16_t retval = heading;
    heading = 0;
    return retval;
}

int32_t EnvData::getDistance(void) {
    return distance;
}

int32_t EnvData::resetDistance(void) {
    int32_t retval = distance;
    distance = 0;
    return retval;
}

uint16_t EnvData::getEncRight(void)                     { return ( encRight                       ); }
uint16_t EnvData::getEncLeft(void)                      { return ( encLeft                        ); }

bool     EnvData::getPirRight(void)                     { return ( pirRight                       ); }
bool     EnvData::getPirLeft(void)                      { return ( pirLeft                        ); }

unsigned int EnvData::getSonarRight(void)               { return ( sonarRight                     ); }
unsigned int EnvData::getSonarLeft(void)                { return ( sonarLeft                      ); }

bool     EnvData::getBumperRight(void)                  { return ( bumper.right                   ); }
bool     EnvData::getBumperLeft(void)                   { return ( bumper.left                    ); }

bool     EnvData::getWhiskerRight(void)                 { return ( whiskerRight                   ); }
bool     EnvData::getWhiskerLeft(void)                  { return ( whiskerLeft                    ); }

bool     EnvData::getLightBumperRight(void)             { return ( lightBumper.right              ); }
bool     EnvData::getLightBumperLeft(void)              { return ( lightBumper.left               ); }
bool     EnvData::getLightBumperFrontRight(void)        { return ( lightBumper.frontRight         ); }
bool     EnvData::getLightBumperFrontLeft(void)         { return ( lightBumper.frontLeft          ); }
bool     EnvData::getLightBumperCenterRight(void)       { return ( lightBumper.centerRight        ); }
bool     EnvData::getLightBumperCenterLeft(void)        { return ( lightBumper.centerLeft         ); }

uint16_t EnvData::getLightBumperSignalRight(void)       { return ( lightBumperSignals.right       ); }
uint16_t EnvData::getLightBumperSignalLeft(void)        { return ( lightBumperSignals.left        ); }
uint16_t EnvData::getLightBumperSignalFrontRight(void)  { return ( lightBumperSignals.frontRight  ); }
uint16_t EnvData::getLightBumperSignalFrontLeft(void)   { return ( lightBumperSignals.frontLeft   ); }
uint16_t EnvData::getLightBumperSignalCenterRight(void) { return ( lightBumperSignals.centerRight ); }
uint16_t EnvData::getLightBumperSignalCenterLeft(void)  { return ( lightBumperSignals.centerLeft  ); }

bool EnvData::getCamObjDetected(void)                   { return ( objDetected                    ); }
int  EnvData::getCamObjSize(void)                       { return ( objSize                        ); }
int  EnvData::getCamObjX(void)                          { return ( objX                           ); }
int  EnvData::getCamObjY(void)                          { return ( objY                           ); }


