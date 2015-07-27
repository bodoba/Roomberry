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
#include <iostream>
#include <ctime>

#include <stdio.h>
#include <time.h>

#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "RoboCam.h"
#ifdef TARGET_RPI
#include "RaspiCamCV.h"
#endif
using namespace cv;
using namespace std;

RoboCam::RoboCam(int width, int height, bool previewWindow, bool calibrationControls) {
    
    video_width       = width;
    video_height      = height;
    showPreviewWindow = previewWindow;
    showControls      = calibrationControls;
    
    iLowH        = 65;
    iHighH       = 121; //  85;
    
    iLowS        = 0;   // 196;
    iHighS       = 255;
    
    iLowV        = 100;  //  72;
    iHighV       = 245; // 255;
    
#ifdef TARGET_RPI
    RASPIVID_CONFIG *config = (RASPIVID_CONFIG*)malloc(sizeof(RASPIVID_CONFIG));
    
    config->width      = video_width;
    config->height     = video_height;
    config->bitrate    = 0;      // zero: leave as default
    config->framerate  = 0;
    config->monochrome = 0;
    
    capture = (RaspiCamCvCapture *) raspiCamCvCreateCameraCapture2(0, config);
    free(config);
#else 
    
    capture = cvCaptureFromCAM( 0 );
#endif
    
    if (calibrationControls) {
        //Create trackbars in "Control" window
        namedWindow("Control", CV_WINDOW_AUTOSIZE);
        namedWindow("Threshold", CV_WINDOW_AUTOSIZE);
        
        cvCreateTrackbar("LowH", "Control", &iLowH, 179); //Hue (0 - 179)
        cvCreateTrackbar("HighH", "Control", &iHighH, 179);
        
        cvCreateTrackbar("LowS", "Control", &iLowS, 255); //Saturation (0 - 255)
        cvCreateTrackbar("HighS", "Control", &iHighS, 255);
        
        cvCreateTrackbar("LowV", "Control", &iLowV, 255); //Value (0 - 255)
        cvCreateTrackbar("HighV", "Control", &iHighV, 255);
    }
    
    sElement = getStructuringElement(MORPH_ELLIPSE, Size(5,5));
    
    if ( previewWindow ) {
        colRed   = Scalar( 0, 0, 255 );
        colGreen = Scalar( 0, 255, 0 );
        colBlack = Scalar( 255, 1, 1);
    }
    
    sLow  = Scalar(iLowH, iLowS, iLowV);
    sHigh = Scalar(iHighH, iHighS, iHighV);
}

bool RoboCam::getObjectPosition( int minSize, int *x, int *y, int *size, int *key ) {
    bool retVal = FALSE;
    *x = 0;
    *y = 0;
    
    Mat imgHSV, imgThresholded, imgOriginal;
    vector< vector<Point> > contours;
    vector<Vec4i>         hierarchy;
    vector<Point>         contours_poly;
    Point2f               center, maxCenter;
    float                 radius, maxRadius = -1;

#ifdef TARGET_RPI
    imgOriginal = raspiCamCvQueryFrame(capture);
#else
    imgOriginal = cvQueryFrame( capture );
#endif
    
    //Convert the captured frame from BGR to HSV
    cvtColor(imgOriginal, imgHSV, COLOR_BGR2HSV);
    
    if ( showControls ) {
        sLow     = Scalar(iLowH, iLowS, iLowV);
        sHigh    = Scalar(iHighH, iHighS, iHighV);
    }
    
    //Threshold the image
    inRange(imgHSV, sLow, sHigh, imgThresholded);
    
    if ( ! showControls ) {
        //morphological opening (remove small objects from the foreground)
        erode( imgThresholded, imgThresholded, sElement );
        dilate( imgThresholded, imgThresholded, sElement );
        
        //morphological closing (fill small holes in the foreground)
        dilate( imgThresholded, imgThresholded, sElement);
        erode(imgThresholded, imgThresholded, sElement );
        
        //detect contours
        findContours( imgThresholded, contours, hierarchy, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE, Point(0, 0) );
        
        // find largest surrounding circle
        for( uint i=0; i<contours.size(); i++ )
        {
            approxPolyDP( Mat(contours[i]), contours_poly, 3, true );
            minEnclosingCircle( (Mat)contours_poly, center, radius );
            if ( radius > maxRadius ) {
                maxCenter = center;
                maxRadius = radius;
            }
        }
        contours.clear();
    }
    // Draw contours
    /*
    if ( maxRadius >= 0 ) {
        if ( showPreviewWindow ) {
            circle( imgOriginal, maxCenter, (int)maxRadius, colGreen, 2, 8, 0 );
        }
        
        retVal = TRUE;
        *x = (int)maxCenter.x;
        *y = (int)maxCenter.y;
        *size = (int)maxRadius;
    }
    */
    if ( showPreviewWindow ) {
        imshow("Original", imgOriginal); //show the original image
        if ( showControls ) {
            imshow("Threshold",imgThresholded);
        }
        *key = waitKey(1);
    }
    return retVal;
}

