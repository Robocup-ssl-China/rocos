cmake_minimum_required (VERSION 3.12)
set(PROJ_NAME "Crazy")
project(${PROJ_NAME} LANGUAGES CXX)

if(WIN32)
     set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /SUBSYSTEM:WINDOWS /ENTRY:mainCRTStartup")
    
     include(ProcessorCount)
    ProcessorCount(N)
    if(NOT N EQUAL 0)
        SET(CMAKE_C_FLAGS   "${CMAKE_C_FLAGS}   /MP${N} ")
        SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /MP${N} ")
    endif()

    ## if you have different Qt visions and CMake cannot link the right one
    #set(CMAKE_PREFIX_PATH D:/App/Qt/5.14.1/msvc2015_64/lib/cmake)
    set(thirdParty ${CMAKE_CURRENT_SOURCE_DIR}/../3rdParty)
endif()

if (NOT UNIX AND NOT APPLE)
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /SUBSYSTEM:WINDOWS")
endif()

set(libs)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17 -pthread")
find_package(Qt5 COMPONENTS Core Quick SerialPort REQUIRED)
list(APPEND libs Qt5::Core Qt5::Quick Qt5::SerialPort)
if(WIN32)
    list(APPEND libs Qt5::WinMain)
endif()
if(WIN32)
    set(PROTOBUF_INCLUDE_DIRS "${thirdParty}/protobuf/include")
    set(Protobuf_LIBRARIES "$${thirdParty}/protobuf/lib/libprotobuf.lib")
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

SET(PROTO_META_BASE_DIR ${CMAKE_BINARY_DIR}/proto/)
file(MAKE_DIRECTORY ${PROTO_META_BASE_DIR})

LIST(APPEND PROTO_FLAGS -I${CMAKE_CURRENT_SOURCE_DIR}/src/lib/share)
file(GLOB PROTO_FILES "${CMAKE_CURRENT_SOURCE_DIR}/src/lib/share/*.proto")

FOREACH(FIL ${PROTO_FILES})
    GET_FILENAME_COMPONENT(ABS_FIL ${FIL} ABSOLUTE)
    GET_FILENAME_COMPONENT(FIL_WE ${ABS_FIL} NAME_WE)
    EXECUTE_PROCESS(
        COMMAND ${PROTOBUF_PROTOC_EXECUTABLE} ${PROTO_FLAGS} --experimental_allow_proto3_optional --cpp_out=${PROTO_META_BASE_DIR} ${FIL}
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    )
ENDFOREACH()

include_directories(${PROTOBUF_INCLUDE_DIRS})
list(APPEND libs ${Protobuf_LIBRARIES})


set(
    PROTO_LIST
    zss_cmd
    zss_cmd_type
)
get_pb_file(PROTO_H PROTO_CPP ${PROTO_LIST})

set(PROJECT_INCLUDE_DIRS
    ${CMAKE_BINARY_DIR}/proto
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_CURRENT_SOURCE_DIR}/src/lib/share
    ${CMAKE_CURRENT_SOURCE_DIR}/src/lib/crc
)
include_directories(${PROJECT_INCLUDE_DIRS})
file(GLOB CPPFILES
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/lib/crc*.cpp"
)

# all head files
file(GLOB HEADERS
    ${CMAKE_CURRENT_SOURCE_DIR}/src/serialobject.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/radiopacket.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/udp24l01object.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/udpwifobject.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/lib/crc/crc.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/translation.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/settings.hpp
)

# all source files
file(GLOB SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/serialobject.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/udp24l01object.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/udpwifobject.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/radiopacket.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/lib/crc/crc.cpp
)

set(SRC
    ${HEADERS}
    ${SOURCES}
    ${PROTO_CPP}
    ${PROTO_H}
    ${CPPFILES}
)
add_executable(${PROJECT_NAME} ${SRC} "qml.qrc" "qml.rc")
target_compile_definitions(${PROJECT_NAME} PRIVATE $<$<OR:$<CONFIG:Debug>,$<CONFIG:RelWithDebInfo>>:QT_QML_DEBUG>)
target_link_libraries(${PROJECT_NAME} PRIVATE ${libs} )
