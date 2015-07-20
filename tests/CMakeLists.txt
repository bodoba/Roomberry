# Tests

add_executable(BumberTest BumperTest.cpp)
add_executable(CaptureTest CaptureTest.cpp)
add_executable(MCP23016Test MCP23016test.cpp)
add_executable(SongTest SongTest.cpp)
add_executable(DetectObject DetectObject.cpp)

target_link_libraries(DetectObject ${OPENCV_LIBS})
target_link_libraries(DetectObject ${RASPICAM_LIBS})

target_link_libraries(CaptureTest ${OPENCV_LIBS})
target_link_libraries(CaptureTest ${RASPICAM_LIBS})
