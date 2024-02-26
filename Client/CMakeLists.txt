set(TARGET_NAME Client)
# Client
if (WIN32)
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /SUBSYSTEM:WINDOWS /ENTRY:mainCRTStartup")

    # muti-processors build for VS
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

add_subdirectory(plugins)

set(libs)

# define protobuf generater
if (WIN32)
    set(PROTOBUF_INCLUDE_DIRS "${thirdParty}/protobuf/include")
    set(Protobuf_LIBRARIES "${thirdParty}/protobuf/lib/libprotobuf.lib")
else()
    find_package(Protobuf REQUIRED)
endif()
include_directories(${PROTOBUF_INCLUDE_DIRS})
list(APPEND libs ${Protobuf_LIBRARIES})

## find Zlib
if (WIN32)
    set(ZLIB_INCLUDE_DIRS "${thirdParty}/zlib/include")
    set(ZLIB_LIBRARIES "${thirdParty}/zlib/lib/zlib.lib")
else()
    find_package(ZLIB)
endif()

include_directories(${ZLIB_INCLUDE_DIRS})
list(APPEND libs ${ZLIB_LIBRARIES})
## find Qt
find_package(Qt5 COMPONENTS Core Quick SerialPort REQUIRED)
list(APPEND libs Qt5::Core Qt5::Quick Qt5::SerialPort)
if (WIN32)
    list(APPEND libs Qt5::WinMain)
endif()
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
    zss_geometry
    zss_rec_old
)
get_pb_file(PROTO_H PROTO_CPP ${PROTO_LIST})

set(PROJECT_INCLUDE_DIRS
    ${CMAKE_BINARY_DIR}
    ${CMAKE_BINARY_DIR}/proto
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_CURRENT_SOURCE_DIR}/src/utils
    ${CMAKE_CURRENT_SOURCE_DIR}/src/vision
    ${CMAKE_CURRENT_SOURCE_DIR}/src/vision/log
    ${CMAKE_CURRENT_SOURCE_DIR}/plugins
    ${CMAKE_SOURCE_DIR}/share
)
include_directories(${PROJECT_INCLUDE_DIRS})
## core cpp files
file(GLOB CPPFILES
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/utils/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/vision/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/vision/log/*.cpp"
    "${CMAKE_SOURCE_DIR}/share/*.cpp"
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
add_executable(${TARGET_NAME} ${SRC} "Client.qrc" "Client.rc")
target_compile_definitions(${TARGET_NAME} PRIVATE $<$<OR:$<CONFIG:Debug>,$<CONFIG:RelWithDebInfo>>:QT_QML_DEBUG>)
target_link_libraries(${TARGET_NAME} PRIVATE ${libs} ${zss_libs})
add_dependencies(${TARGET_NAME} proto_gen)