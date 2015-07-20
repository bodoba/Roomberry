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

#ifndef CAMERA_H
#define CAMERA_H
#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <unistd.h>
#include <stdint.h>

#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "RoboCam.h"

using namespace cv;
using namespace std;

#ifdef __linux__
#ifdef __arm__
// assume we are compiling on Raspberry PI
#define TARGET_RPI
#endif
#endif

#ifdef TARGET_RPI
/*
 * Use RaspiCam interface to get image at usable FPS rate
 * (see: https://github.com/robidouille/robidouille/tree/master/raspicam_cv)
 * 
 * Follow instructions and install header and librbaies in /opt/raspicam:
 *
 * cd ~/git/robidouille/raspicam_cv
 * sudo mkdir -p /opt/raspicamcv/include
 * sudo mkdir -p /opt/raspicamcv/lib
 * sudo cp RaspiCamCV.h /opt/raspicamcv/include
 * sudo cp libraspicamcv.*  /opt/raspicamcv/lib
 * Add line /opt/raspicamcv/lib to /etc/ld.so.conf.d/raspicamcv.conf
 * run sudo ldconfig
 */
#include "RaspiCamCV.h"
#endif

#ifndef TRUE
#define TRUE	(1==1)
#define FALSE	(0==1)
#endif

class RoboCam {
    
public:
    /** Create RoboCam Object
     *
     * \param width  video width   (default 320, max 1280)
     * \param height video height  (default 240, max  920)
     * \param previewWindow Show preview window? (default False)
     * \param calibrationControls Show window with controls to adjust calibration? (default False)
     */
    RoboCam(int width=320,
            int height=240,
            bool previewWindow=FALSE,
            bool calibrationControls=FALSE);

    /** get Position of Object in Image
     *
     * Detects largest object matching the calibration settings. Preview image 
     * will be displayed if parameter Preview window has been set TRUE in
     * constructor
     *
     * \param minSize Minimal size for object to be considered
     *
     * \param x horizontal position (0 - video width)
     * \param y verticalal position (0 - video height)
     * \param size radius of detected object
     * \param key keycode in case a key
     * \return TRUE -> Object has been detected, FALSE otherwise
     */
    bool getObjectPosition( int minSize, int *x, int *y, int *size, int *key );
    
private:
    int    iLowH, iHighH;
    int    iLowS, iHighS;
    int    iLowV, iHighV;
    Scalar sLow, sHigh;
    
    int  video_width, video_height;
    bool showPreviewWindow, showControls;
    
    Scalar colRed, colGreen, colBlack;
    
    Mat sElement;
#ifdef TARGET_RPI
    RaspiCamCvCapture *capture;
#else
    CvCapture *capture;
#endif
};

#endif