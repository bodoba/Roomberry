# main
include(CheckIncludeFile)

cmake_minimum_required (VERSION 2.6)
project (Roomberry)

set (CMAKE_CXX_FLAGS "-Wall -std=gnu++0x")

# all executables end up in bin
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)
set(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/lib)

# Raspicam
include_directories(/opt/raspicamcv/include)
set (CMAKE_REQUIRED_INCLUDES "/opt/raspicamcv/include")

CHECK_INCLUDE_FILE(RaspiCamCV.h HAVE_RASPICAMCV)

IF (HAVE_RASPICAMCV)
  link_directories (/opt/raspicamcv/lib)
  set(RASPICAM_LIBS libraspicamcv.so)
ENDIF (HAVE_RASPICAMCV)

# OpenCV
set(OPENCV_LIBS opencv_calib3d opencv_contrib opencv_core opencv_features2d opencv_flann opencv_highgui opencv_imgproc opencv_legacy opencv_ml opencv_objdetect opencv_photo opencv_stitching opencv_ts opencv_video opencv_videostab)

# on OSX
include_directories(/opt/local/include)
link_directories (/opt/local/lib)

# subsystem support
add_subdirectory(libroomberry)
include_directories(libroomberry)
link_libraries (libroomberry)

# Test utilities
add_subdirectory(tests)

# Tools
add_subdirectory(tools)