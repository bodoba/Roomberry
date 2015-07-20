# Tools

add_executable(Drive Drive.cpp)
add_executable(GetBatteryData GetBatteryData.cpp)
add_executable(Play Play.cpp)
add_executable(Roam Roam.cpp)
add_executable(Spin Spin.cpp)
add_executable(Stop Stop.cpp)

target_link_libraries(Roam ${OPENCV_LIBS})
target_link_libraries(Roam ${RASPICAM_LIBS})
target_link_libraries(Roam ncurses)
target_link_libraries(Roam pthread)

