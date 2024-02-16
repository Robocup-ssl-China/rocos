set(TARGET_NAME Core)
set(libs)
set(incs)
# Core - Medusa
## Protobuf
find_package(Protobuf REQUIRED)
list(APPEND incs ${PROTOBUF_INCLUDE_DIRS})
list(APPEND libs ${PROTOBUF_LIBRARIES})
## Qt5
find_package(Qt5 COMPONENTS Core Qml Quick Network REQUIRED)
list(APPEND libs Qt5::Core Qt5::Qml Qt5::Quick Qt5::Network)
## Lua
find_package(Lua REQUIRED)
list(APPEND incs ${LUA_INCLUDE_DIR})
list(APPEND libs ${LUA_LIBRARIES})
## tolua++
find_package(toluapp REQUIRED)
list(APPEND incs ${TOLUAPP_INCLUDE_DIRS})
list(APPEND libs ${TOLUAPP_LIBRARIES})
EXECUTE_PROCESS(
    COMMAND ${TOLUAPP_EXECUTABLE} -n zeus -o ${CMAKE_CURRENT_BINARY_DIR}/tolua_zeus.cpp ${CMAKE_CURRENT_SOURCE_DIR}/src/LuaModule/zeus.pkg
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/src/LuaModule
)
## Eigen
find_package(Eigen3 REQUIRED)
list(APPEND incs ${EIGEN3_INCLUDE_DIR})
## fmt
find_package(fmt REQUIRED)
list(APPEND incs ${fmt_INCLUDE_DIRS})
list(APPEND libs fmt::fmt)

set(PROTO_LIST
    game_event
    grSim_Commands
    grSimMessage
    grSim_Packet
    grSim_Replacement
    messages_robocup_ssl_detection
    messages_robocup_ssl_geometry_legacy
    messages_robocup_ssl_geometry
    messages_robocup_ssl_refbox_log
    messages_robocup_ssl_wrapper_legacy
    messages_robocup_ssl_wrapper
    ssl_referee
    ssl_game_event
    ssl_game_event_2019
    ssl_game_controller_common
    vision_detection
    zss_cmd
    zss_debug
    zss_geometry
)
get_pb_file(PROTO_H PROTO_CPP ${PROTO_LIST})

set(PROJECT_INCLUDE_DIRS
    ${incs}
    ${CMAKE_BINARY_DIR}/proto
    ${CMAKE_SOURCE_DIR}/share
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Algorithm
    ${CMAKE_CURRENT_SOURCE_DIR}/src/LuaModule
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Main
    ${CMAKE_CURRENT_SOURCE_DIR}/src/MotionControl
    ${CMAKE_CURRENT_SOURCE_DIR}/src/OtherLibs
    ${CMAKE_CURRENT_SOURCE_DIR}/src/PathPlan
    ${CMAKE_CURRENT_SOURCE_DIR}/src/RefereeBox
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Simulator
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Strategy
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Utils
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Vision
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Wireless
    ${CMAKE_CURRENT_SOURCE_DIR}/src/WorldModel
    ${CMAKE_CURRENT_SOURCE_DIR}/src/OtherLibs/cmu
    ${CMAKE_CURRENT_SOURCE_DIR}/src/OtherLibs/cornell
    ${CMAKE_CURRENT_SOURCE_DIR}/src/OtherLibs/nlopt
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Strategy/defence
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Strategy/rolematch
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Strategy/skill
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Vision/mediator/net
)
file(GLOB PROJECT_SOURCE
    "${CMAKE_SOURCE_DIR}/share/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/Algorithm/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/LuaModule/LuaModule.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/Main/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/MotionControl/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/OtherLibs/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/PathPlan/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/RefereeBox/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/Simulator/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/Strategy/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/Utils/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/Vision/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/Wireless/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/WorldModel/*.cpp"

    "${CMAKE_CURRENT_SOURCE_DIR}/src/OtherLibs/cmu/*.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/src/OtherLibs/cornell/*.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/src/OtherLibs/nlopt/*.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/src/Strategy/defence/*.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/src/Strategy/rolematch/*.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/src/Strategy/skill/*.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/src/Vision/mediator/net/*.cpp"
    ${PROTO_H}
    ${PROTO_CPP}
    "${CMAKE_CURRENT_BINARY_DIR}/tolua_zeus.cpp"
)

subdir_list(TACTIC_PACKS ${CMAKE_CURRENT_SOURCE_DIR})

# remove ignore directory from TACTIC_PACKS
list(REMOVE_ITEM TACTIC_PACKS 
    "src"
)

foreach(pack ${TACTIC_PACKS})
    message(STATUS "rocos - tactics_package: '${pack}'")
    file(GLOB TACTIC_SOURCE 
        "${CMAKE_CURRENT_SOURCE_DIR}/${pack}/skill/*.cpp"
    )
    list(APPEND PROJECT_INCLUDE_DIRS
        ${CMAKE_CURRENT_SOURCE_DIR}/${pack}/skill
    )
    message(TACTIC_SOURCE ${TACTIC_SOURCE})
    list(APPEND PROJECT_SOURCE ${TACTIC_SOURCE})
endforeach()

include_directories(${PROJECT_INCLUDE_DIRS})
add_executable(${TARGET_NAME} ${PROJECT_SOURCE})
target_link_libraries(${TARGET_NAME} PRIVATE ${libs})
