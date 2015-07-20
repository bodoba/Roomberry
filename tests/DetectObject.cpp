#include "opencv2/objdetect/objdetect.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"

#include <iostream>
#include <stdio.h>

#ifdef __linux__
#ifdef __arm__
// assume we are compiling on Raspberry PI
#define TARGET_RPI
#endif
#endif

#ifdef TARGET_RPI
#include "RaspiCamCV.h"
#endif

#define video_width   320
#define video_height  240

using namespace std;
using namespace cv;

/** Function Headers */
void detectAndDisplay( Mat frame );

/** Global variables */
String object_cascade_name = "lbp-cascade.xml";

CascadeClassifier face_cascade;
string window_name = "Capture - Object detection";

/** @function main */
int main( int argc, const char** argv ) {
#ifdef TARGET_RPI
    RaspiCamCvCapture *capture;
#else
    CvCapture *capture;
#endif
    Mat frame;
    
    //-- 1. Load the cascades
    if( !face_cascade.load( object_cascade_name ) ) {
        printf("--(!)Error object cascade\n");
        return -1;
    }
    
    //-- 2. Read the video stream
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
    cvSetCaptureProperty(capture, CV_CAP_PROP_FRAME_WIDTH,  video_width);
    cvSetCaptureProperty(capture, CV_CAP_PROP_FRAME_HEIGHT, video_height);
#endif
    
    if( capture ) {
        while( true ) {
#ifdef TARGET_RPI
            frame = raspiCamCvQueryFrame(capture);
#else
            frame = cvQueryFrame( capture );
#endif
            //-- 3. Apply the classifier to the frame
            if( !frame.empty() ) {
                detectAndDisplay( frame );
            } else {
                printf(" --(!) No captured frame -- Break!\n");
                break;
            }
            
            int c = waitKey(10);
            if( c == 27 ) {
                break;
            }
        }
    } else {
        printf(" --(!) No capture object\n");
    }
    return 0;
}

/** @function detectAndDisplay */
void detectAndDisplay( Mat frame )
{
    std::vector<Rect> faces;
    Mat frame_gray;
    
    cvtColor( frame, frame_gray, CV_BGR2GRAY );
    equalizeHist( frame_gray, frame_gray );
    
    //-- Detect faces
    face_cascade.detectMultiScale( frame_gray, faces, 1.1, 2, 0, Size(80, 80) );
    
    for( size_t i = 0; i < faces.size(); i++ ) {
        Point center( faces[i].x + faces[i].width*0.5, faces[i].y + faces[i].height*0.5 );
        ellipse( frame, center, Size( faces[i].width*0.5, faces[i].height*0.5), 0, 0, 360, Scalar( 255, 0, 255 ), 4, 8, 0 );
        
        Mat faceROI = frame_gray( faces[i] );
        std::vector<Rect> eyes;
    }
    //-- Show what you got
    imshow( window_name, frame );
}