cmake_minimum_required(VERSION 3.16)
project(main)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_BUILD_TYPE Debug) #remeber to delet this if you want a user app, this increase the app length to better debug
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

#find_package(Qt6 REQUIRED COMPONENTS Widgets)
find_package( OpenCV REQUIRED )
find_package(wxWidgets REQUIRED)
find_package(PkgConfig REQUIRED)
find_package(X11 REQUIRED)



pkg_check_modules(lz4 REQUIRED IMPORTED_TARGET liblz4)
pkg_check_modules(PORTAUDIO REQUIRED portaudio-2.0)

include_directories( ${OpenCV_INCLUDE_DIRS})
include_directories(${PORTAUDIO_INCLUDE_DIRS})

include(UsewxWidgets)



add_executable(main main.cpp cApp.cpp cMain.cpp networking.cpp client.cpp server.cpp config.cpp login.cpp camera.cpp soundIO.cpp screenCapturing.cpp)
#target_link_libraries(main Qt6::Widgets)
target_link_libraries( main ${OpenCV_LIBS} ${wxWidgets_LIBRARIES} ${PORTAUDIO_LIBRARIES} X11::X11 PkgConfig::lz4)
