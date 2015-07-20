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

#ifndef ENVDATA_H
#define ENVDATA_H

#include <stdint.h>
#include <pthread.h>
#include <time.h>
#include <sys/time.h>

#include "Roomberry.h"
#include "MCP23016.h"
#include "SRF02.h"
#include "RoboCam.h"

#define PIR_RIGHT        MCP23016_GPIO1_0
#define PIR_LEFT         MCP23016_GPIO1_1

#define BTN_WSK_RIGHT    MCP23016_GPIO1_6
#define BTN_WSK_LEFT     MCP23016_GPIO1_7

#define SONAR_MIN        18

#define VIDEO_WIDTH      320
#define VIDEO_HEIGHT     240
#define MIN_OBJ_SIZE      25

void   *cameraControl( void *tArg );

typedef struct {
    bool showPreviewWindow;
    bool objDetected;
    int  objSize;
    int  objX;
    int  objY;
    bool run;
    unsigned int frames;
} camData_t;

class EnvData {
public:
    EnvData(Roomberry *myRoomba, bool useCam, bool pv );
    void stop (void);
    int readSensors(void);

    bool object(void);
    bool objectLeft(void);
    bool objectRight(void);
    bool objectFront(void);
    
    // getter functions
    bool getPirRight(void);
    bool getPirLeft(void);

    unsigned int getSonarRight(void);
    unsigned int getSonarLeft(void);
    
    bool getBumperRight(void);
    bool getBumperLeft(void);
    
    bool getWhiskerRight(void);
    bool getWhiskerLeft(void);

    bool getLightBumperRight(void);
    bool getLightBumperLeft(void);
    bool getLightBumperFrontRight(void);
    bool getLightBumperFrontLeft(void);
    bool getLightBumperCenterRight(void);
    bool getLightBumperCenterLeft(void);
    
    uint16_t getLightBumperSignalRight(void);
    uint16_t getLightBumperSignalLeft(void);
    uint16_t getLightBumperSignalFrontRight(void);
    uint16_t getLightBumperSignalFrontLeft(void);
    uint16_t getLightBumperSignalCenterRight(void);
    uint16_t getLightBumperSignalCenterLeft(void);

    bool readCamData(void);

    bool  getCamObjDetected(void);
    int   getCamObjSize(void);
    int   getCamObjX(void);
    int   getCamObjY(void);
    float getCamFPS(void);

private:
    Roomberry *roomba;
    
    // I2C devices
    MCP23016 *i2cIoExp;
    SRF02    *i2cSonarLeft, *i2cSonarRight;
    
    // Roombas Sensors
    roomberryLightBumperSignals_t lightBumperSignals;
    roomberryLightBumper_t        lightBumper;
    roomberryBumper_t             bumper;
    
    // PIR sensors
    bool pirRight;
    bool pirLeft;
    
    // Whiskers
    bool whiskerRight;
    bool whiskerLeft;
    
    // Sonar Data
    unsigned int sonarRight;
    unsigned int sonarLeft;
    
    
    // Camera
    int       camThreadID;
    pthread_t camThread;
    camData_t camData;
    
    bool objDetected;
    int  objSize;
    int  objX;
    int  objY;
    bool run;
    
    unsigned int     cameraFrames;
    struct   timeval cameraStartTime;

    RoboCam *cam;
    bool    showPreviewWindow;

};

#endif
