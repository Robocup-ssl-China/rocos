cmake_minimum_required(VERSION 3.9)

project(Client LANGUAGES CXX)

set(VERSION "0.0.1")
set(VENDOR "ZJUNlict")
set(MAINTAINER "ZJUNlict <zjunlict.ssl@gmail.com>")
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17 -pthread")
set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake/modules)
include(${PROJECT_SOURCE_DIR}/cmake/Utils.cmake)
standard_config()
standard_paths(${PROJECT_SOURCE_DIR} ../ZBin lib)
if (NOT UNIX AND NOT APPLE)
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /SUBSYSTEM:WINDOWS")
endif()

#add_definitions(-DZSPLUGIN_DEBUG -DSIM_TIME_DEBUG)
#include(ProcessorCount)
#ProcessorCount(N)
#if(NOT N EQUAL 0)
#    SET(CMAKE_C_FLAGS   "${CMAKE_C_FLAGS}   /MP${N} ")
#    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /MP${N} ")
#endif()
set(libs)

# define protobuf generater
if (NOT UNIX AND NOT APPLE)
    set(PROTOBUF_INCLUDE_DIRS "C:/usr/local/protobuf/include")
    set(Protobuf_LIBRARIES "C:/usr/local/protobuf/lib/libprotobuf.lib")
else()
    find_package(Protobuf REQUIRED)
endif()
function(get_pb_file H CPP)
    list(APPEND PH ${ARGN})
    list(APPEND PC ${ARGN})
    list(TRANSFORM PH PREPEND ${CMAKE_BINARY_DIR}/proto/)
    list(TRANSFORM PH APPEND .pb.h)
    list(TRANSFORM PC PREPEND ${CMAKE_BINARY_DIR}/proto/)
    list(TRANSFORM PC APPEND .pb.cc)
    set(${H} ${PH} PARENT_SCOPE)
    set(${CPP} ${PC} PARENT_SCOPE)
endfunction()
SET(PROTO_META_BASE_DIR ${CMAKE_CURRENT_BINARY_DIR}/proto)
file(MAKE_DIRECTORY ${PROTO_META_BASE_DIR})

add_subdirectory(plugins)

# find packages
## find protobuf
LIST(APPEND PROTO_FLAGS -I${CMAKE_CURRENT_SOURCE_DIR}/../Medusa/share/proto)
file(GLOB PROTO_FILES "${CMAKE_SOURCE_DIR}/../Medusa/share/proto/*.proto")
## protobuf_generate_cpp(PROTO_CPP PROTO_H ${PROTO_FILES})
FOREACH(FIL ${PROTO_FILES})
    GET_FILENAME_COMPONENT(ABS_FIL ${FIL} ABSOLUTE)
    GET_FILENAME_COMPONENT(FIL_WE ${ABS_FIL} NAME_WE)
    EXECUTE_PROCESS(
        COMMAND ${PROTOBUF_PROTOC_EXECUTABLE} ${PROTO_FLAGS} --cpp_out=${PROTO_META_BASE_DIR} ${FIL}
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    )
ENDFOREACH()

include_directories(${PROTOBUF_INCLUDE_DIRS})
list(APPEND libs ${Protobuf_LIBRARIES})

## find Eigen Zlib
if (NOT UNIX AND NOT APPLE)
    set(EIGEN3_INCLUDE_DIR "C:/usr/local/Eigen")
    set(ZLIB_INCLUDE_DIRS "C:/usr/local/zlib/include")
    set(ZLIB_LIBRARIES "C:/usr/local/zlib/lib/zlib.lib")
else()
    find_package(Eigen3 REQUIRED)
    find_package(ZLIB)
endif()
include_directories(${EIGEN3_INCLUDE_DIR})
include_directories(${ZLIB_INCLUDE_DIRS})
list(APPEND libs ${ZLIB_LIBRARIES})
## find Qt
find_package(Qt5 COMPONENTS Core Quick SerialPort REQUIRED)
list(APPEND libs Qt5::Core Qt5::Quick Qt5::SerialPort)

# project files
## protobuf files
set(PROTO_LIST
    grSim_Commands
    grSimMessage
    grSim_Packet
    grSim_Replacement
    log_labeler_data
    log_labels
    messages_robocup_ssl_detection
    messages_robocup_ssl_geometry_legacy
    messages_robocup_ssl_geometry
    messages_robocup_ssl_refbox_log
    messages_robocup_ssl_wrapper_legacy
    messages_robocup_ssl_wrapper
    ssl_game_controller_auto_ref
    ssl_game_controller_common
    ssl_game_controller_team
    ssl_game_event_2019
    ssl_game_event
    ssl_referee
    vision_detection
    zss_cmd
    zss_debug
    zss_rec_old
)
get_pb_file(PROTO_H PROTO_CPP ${PROTO_LIST})

set(PROJECT_INCLUDE_DIRS
    ${CMAKE_BINARY_DIR}/proto
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_CURRENT_SOURCE_DIR}/src/utils
    ${CMAKE_CURRENT_SOURCE_DIR}/src/vision
    ${CMAKE_CURRENT_SOURCE_DIR}/src/vision/log
    ${CMAKE_CURRENT_SOURCE_DIR}/plugins
    ${CMAKE_SOURCE_DIR}/../Medusa/share
)
include_directories(${PROJECT_INCLUDE_DIRS})
## core cpp files
file(GLOB CPPFILES
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/utils/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/vision/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/vision/log/*.cpp"
    "${CMAKE_SOURCE_DIR}/../Medusa/share/*.cpp"
)
set(SRC
    ${HEADERS}
    ${SOURCES}
    ${PROTO_CPP}
    ${PROTO_H}
    ${CPPFILES}
)
set(zss_libs
    zss_simulator
)
add_executable(${PROJECT_NAME} ${SRC} "Client.qrc" "Client.rc")
target_compile_definitions(${PROJECT_NAME} PRIVATE $<$<OR:$<CONFIG:Debug>,$<CONFIG:RelWithDebInfo>>:QT_QML_DEBUG>)
target_link_libraries(${PROJECT_NAME} PRIVATE ${libs} ${zss_libs})
