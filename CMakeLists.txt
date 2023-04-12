cmake_minimum_required(VERSION 3.13)

project(ADAS)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_BUILD_TYPE "Release")
set(CMAKE_CXX_FLAGS_DEBUG "$ENV{CXXFLAGS} -O0 -Wall -g -ggdb")
set(CMAKE_CXX_FLAGS_RELEASE "$ENV{CXXFLAGS}  -O3 -g -Wall")
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS}")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS}")

find_package(Qt5 COMPONENTS Widgets REQUIRED)
find_package(OpenCV REQUIRED)
find_library(V4L2_LIBRARY v4l2)

include(CMakeParseArguments)

file(GLOB MAIN_SRC ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)
file(GLOB MAIN_HEADERS ${CMAKE_CURRENT_SOURCE_DIR}/*.h*)

# add_demo(NAME <target name>
#     SOURCES <source files>
#     [HEADERS <header files>]
#     [INCLUDE_DIRECTORIES <include dir>]
#     [OPENCV_VERSION_REQUIRED <X.Y.Z>]
#     [DEPENDENCIES <dependencies>])
macro(add_demo)
    set(oneValueArgs NAME OPENCV_VERSION_REQUIRED)
    set(multiValueArgs SOURCES HEADERS DEPENDENCIES INCLUDE_DIRECTORIES)
    cmake_parse_arguments(OMZ_DEMO "${options}" "${oneValueArgs}"
                          "${multiValueArgs}" ${ARGN})

    if(OMZ_DEMO_OPENCV_VERSION_REQUIRED AND OpenCV_VERSION VERSION_LESS OMZ_DEMO_OPENCV_VERSION_REQUIRED)
        message(WARNING "${OMZ_DEMO_NAME} is disabled; required OpenCV version ${OMZ_DEMO_OPENCV_VERSION_REQUIRED}, provided ${OpenCV_VERSION}")
        return()
    endif()

    # Create named folders for the sources within the .vcproj
    # Empty name lists them directly under the .vcproj
    source_group("src" FILES ${OMZ_DEMO_SOURCES})
    if(OMZ_DEMO_HEADERS)
        source_group("include" FILES ${OMZ_DEMO_HEADERS})
    endif()

    # Create executable file from sources
    add_executable(${OMZ_DEMO_NAME} ${OMZ_DEMO_SOURCES} ${OMZ_DEMO_HEADERS})

    if(WIN32)
        set_target_properties(${OMZ_DEMO_NAME} PROPERTIES COMPILE_PDB_NAME ${OMZ_DEMO_NAME})
    endif()

    if(OMZ_DEMO_INCLUDE_DIRECTORIES)
        target_include_directories(${OMZ_DEMO_NAME} PRIVATE ${OMZ_DEMO_INCLUDE_DIRECTORIES})
    endif()

    target_link_libraries(${OMZ_DEMO_NAME} PRIVATE ${OpenCV_LIBRARIES} openvino::runtime
                                                    Qt5::Widgets ${V4L2_LIBRARY} ${OpenCV_LIBS}
                                                    ${OMZ_DEMO_DEPENDENCIES} utils gflags X11)

    if(UNIX)
        target_link_libraries(${OMZ_DEMO_NAME} PRIVATE pthread)
    endif()
endmacro()

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(OpenCV REQUIRED COMPONENTS core highgui videoio imgproc imgcodecs)
find_package(OpenVINO REQUIRED COMPONENTS Runtime)

include_directories(${Qt5Widgets_INCLUDE_DIRS})
include_directories(${OpenCV_INCLUDE_DIRS})

add_demo(NAME ADAS
    SOURCES ${MAIN_SRC}
    HEADERS ${MAIN_HEADERS}
    DEPENDENCIES monitors models pipelines)


add_subdirectory(thirdparty/gflags)
add_subdirectory (common)

