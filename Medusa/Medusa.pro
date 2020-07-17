TEMPLATE = app

CONFIG += c++11

QT += core qml quick network

CONFIG += c++11 console

DEFINES += QT_DEPRECATED_WARNINGS

# python module for windows and linux
# if you change this switch, you may need to clear the cache
#DEFINES += USE_PYTHON_MODULE
#DEFINES += USE_CUDA_MODULE
#DEFINES += USE_OPENMP

ZSS_LIBS =
ZSS_INCLUDES =

CONFIG(debug, debug|release) {
    #CONFIG += console
    TARGET = MedusaD
    DESTDIR = $$PWD/../ZBin
    MOC_DIR = ./temp
    OBJECTS_DIR = ./temp
}
CONFIG(release, debug|release) {
    TARGET = Medusa
    DESTDIR = $$PWD/../ZBin
    MOC_DIR = ./temp
    OBJECTS_DIR = ./temp
}

win32 {
    if(contains(DEFINES,USE_OPENMP)){
        QMAKE_CXXFLAGS += -openmp
    }
    win32-msvc*:QMAKE_CXXFLAGS += /wd"4819"
    # Third party library dir
    THIRD_PARTY_DIR = $$PWD/../ZBin/3rdParty
    contains(QMAKE_TARGET.arch, x86_64){
        message("64-bit")
        CONFIG(release,debug|release){
            ZSS_LIBS += $${THIRD_PARTY_DIR}/protobuf/lib/x64/libprotobuf.lib \
                        $${THIRD_PARTY_DIR}/tolua++/lib/x64/tolua++.lib \
                        $${THIRD_PARTY_DIR}/lua/lib/x64/lua5.1.lib
        }
        CONFIG(debug,debug|release){
            ZSS_LIBS += $${THIRD_PARTY_DIR}/protobuf/lib/x64/libprotobufd.lib \
                        $${THIRD_PARTY_DIR}/tolua++/lib/x64/tolua++D.lib \
                        $${THIRD_PARTY_DIR}/lua/lib/x64/lua5.1.lib
        }
    } else {
        message("32-bit")
        CONFIG(release,debug|release){
            ZSS_LIBS += $${THIRD_PARTY_DIR}/protobuf/lib/x86/libprotobuf.lib \
                        $${THIRD_PARTY_DIR}/tolua++/lib/x86/tolua++.lib \
                        $${THIRD_PARTY_DIR}/lua/lib/x86/lua5.1.lib
        }
        CONFIG(debug,debug|release){
            ZSS_LIBS += $${THIRD_PARTY_DIR}/protobuf/lib/x86/libprotobufd.lib \
                        $${THIRD_PARTY_DIR}/tolua++/lib/x86/tolua++.lib \
                        $${THIRD_PARTY_DIR}/lua/lib/x86/lua5.1.lib
        }
    }

    ZSS_INCLUDES += \
        $${THIRD_PARTY_DIR}/protobuf/include \
        $${THIRD_PARTY_DIR}/Eigen \

}

unix:!macx{
    ZSS_INCLUDES += \
        /usr/include/eigen3 \
        /usr/local/include
    ZSS_LIBS += \
        -llua5.1 \
        -ldl \
        -ltolua++5.1 \
        -lprotobuf
}

INCLUDEPATH += \
    $$ZSS_INCLUDES

LIBS += \
    $$ZSS_LIBS

INCLUDEPATH += \
    share \
    share/proto/cpp \
    src \
    src/Algorithm \
    src/LuaModule \
    src/Main \
    src/MotionControl \
    src/OtherLibs \
    src/OtherLibs/cmu \
    src/OtherLibs/cornell \
    src/OtherLibs/nlopt \
    src/PathPlan \
    src/RefereeBox \
    src/Simulator \
    src/Strategy \
    src/Strategy/rolematch \
    src/Strategy/skill \
    src/Utils \
    src/Vision \
    src/Vision/mediator/net \
    src/Wireless \
    src/WorldModel

SOURCES += \
    share/geometry.cpp \
    share/parammanager.cpp \
    share/proto/cpp/grSim_Commands.pb.cc \
    share/proto/cpp/grSimMessage.pb.cc \
    share/proto/cpp/grSim_Packet.pb.cc \
    share/proto/cpp/grSim_Replacement.pb.cc \
    share/proto/cpp/log_labeler_data.pb.cc \
    share/proto/cpp/log_labels.pb.cc \
    share/proto/cpp/messages_robocup_ssl_detection.pb.cc \
    share/proto/cpp/messages_robocup_ssl_geometry_legacy.pb.cc \
    share/proto/cpp/messages_robocup_ssl_geometry.pb.cc \
    share/proto/cpp/messages_robocup_ssl_refbox_log.pb.cc \
    share/proto/cpp/messages_robocup_ssl_wrapper_legacy.pb.cc \
    share/proto/cpp/messages_robocup_ssl_wrapper.pb.cc \
    share/proto/cpp/ssl_game_controller_auto_ref.pb.cc \
    share/proto/cpp/ssl_game_controller_common.pb.cc \
    share/proto/cpp/ssl_game_controller_team.pb.cc \
    share/proto/cpp/ssl_game_event_2019.pb.cc \
    share/proto/cpp/ssl_game_event.pb.cc \
    share/proto/cpp/ssl_referee.pb.cc \
    share/proto/cpp/vision_detection.pb.cc \
    share/proto/cpp/zss_cmd.pb.cc \
    share/proto/cpp/zss_debug.pb.cc \
    src/LuaModule/LuaModule.cpp \
    src/LuaModule/lua_zeus.cpp \
    src/Main/ActionModule.cpp \
    src/Main/DecisionModule.cpp \
    src/Main/Global.cpp \
    src/Main/OptionModule.cpp \
    src/Main/zeus_main.cpp \
    src/MotionControl/CMmotion.cpp \
    src/MotionControl/ControlModel.cpp \
    src/MotionControl/CubicEquation.cpp \
    src/MotionControl/DynamicsSafetySearch.cpp \
    src/MotionControl/noneTrapzodalVelTrajectory.cpp \
    src/MotionControl/QuadraticEquation.cpp \
    src/MotionControl/QuarticEquation.cpp \
    src/MotionControl/TrapezoidalVelTrajectory.cpp \
    src/OtherLibs/cmu/obstacle.cpp \
    src/OtherLibs/cmu/path_planner.cpp \
    src/OtherLibs/cornell/Trajectory.cpp \
    src/OtherLibs/cornell/TrajectoryStructs.cpp \
    src/OtherLibs/cornell/TrajectorySupport.cpp \
    src/PathPlan/KDTreeNew.cpp \
    src/PathPlan/ObstacleNew.cpp \
    src/PathPlan/PathPlanner.cpp \
    src/PathPlan/PredictTrajectory.cpp \
    src/PathPlan/RRTPathPlanner.cpp \
    src/PathPlan/RRTPathPlannerNew.cpp \
    src/RefereeBox/RefereeBoxIf.cpp \
    src/Simulator/CommandInterface.cpp \
    src/Strategy/rolematch/matrix.cpp \
    src/Strategy/rolematch/munkres.cpp \
    src/Strategy/rolematch/munkresTacticPositionMatcher.cpp \
    src/Strategy/skill/BasicPlay.cpp \
    src/Strategy/skill/Factory.cpp \
    src/Strategy/skill/Goalie.cpp \
    src/Strategy/skill/GotoPosition.cpp \
    src/Strategy/skill/OpenSpeed.cpp \
    src/Strategy/skill/PlayerTask.cpp \
    src/Strategy/skill/SmartGotoPosition.cpp \
    src/Strategy/skill/Speed.cpp \
    src/Strategy/skill/StopRobot.cpp \
    src/Strategy/skill/Touch.cpp \
    src/Utils/BufferCounter.cpp \
    src/Utils/GDebugEngine.cpp \
    src/Utils/utils.cpp \
    src/Vision/BallPredictor.cpp \
    src/Vision/CollisionSimulator.cpp \
    src/Vision/RobotPredictError.cpp \
    src/Vision/RobotPredictor.cpp \
    src/Vision/RobotsCollision.cpp \
    src/Vision/VisionModule.cpp \
    src/Vision/VisionReceiver.cpp \
    src/Wireless/PlayerCommandV2.cpp \
    src/Wireless/RobotSensor.cpp \
    src/WorldModel/PlayInterface.cpp \
    src/WorldModel/RobotCapability.cpp \
    src/WorldModel/WorldModel_basic.cpp \
    src/Algorithm/ballmodel.cpp \
    src/Algorithm/BallSpeedModel.cpp \
    src/Algorithm/BallStatus.cpp \
    src/Algorithm/CollisionDetect.cpp \
    src/Algorithm/ContactChecker.cpp \
    src/Algorithm/runtimepredictor.cpp


HEADERS += \
    share/dataqueue.hpp \
    share/geometry.h \
    share/parammanager.h \
    share/proto/cpp/grSim_Commands.pb.h \
    share/proto/cpp/grSimMessage.pb.h \
    share/proto/cpp/grSim_Packet.pb.h \
    share/proto/cpp/grSim_Replacement.pb.h \
    share/proto/cpp/log_labeler_data.pb.h \
    share/proto/cpp/log_labels.pb.h \
    share/proto/cpp/messages_robocup_ssl_detection.pb.h \
    share/proto/cpp/messages_robocup_ssl_geometry_legacy.pb.h \
    share/proto/cpp/messages_robocup_ssl_geometry.pb.h \
    share/proto/cpp/messages_robocup_ssl_refbox_log.pb.h \
    share/proto/cpp/messages_robocup_ssl_wrapper_legacy.pb.h \
    share/proto/cpp/messages_robocup_ssl_wrapper.pb.h \
    share/proto/cpp/ssl_game_controller_auto_ref.pb.h \
    share/proto/cpp/ssl_game_controller_common.pb.h \
    share/proto/cpp/ssl_game_controller_team.pb.h \
    share/proto/cpp/ssl_game_event_2019.pb.h \
    share/proto/cpp/ssl_game_event.pb.h \
    share/proto/cpp/ssl_referee.pb.h \
    share/proto/cpp/vision_detection.pb.h \
    share/proto/cpp/zss_cmd.pb.h \
    share/proto/cpp/zss_debug.pb.h \
    share/singleton.hpp \
    share/staticparams.h \
    src/LuaModule/lauxlib.h \
    src/LuaModule/luaconf.h \
    src/LuaModule/lua.h \
    src/LuaModule/lualib.h \
    src/LuaModule/LuaModule.h \
    src/LuaModule/tolua++.h \
    src/Main/ActionModule.h \
    src/Main/DecisionModule.h \
    src/Main/Global.h \
    src/Main/OptionModule.h \
    src/Main/TaskMediator.h \
    src/MotionControl/CMmotion.h \
    src/MotionControl/ControlModel.h \
    src/MotionControl/CubicEquation.h \
    src/MotionControl/DynamicsSafetySearch.h \
    src/MotionControl/noneTrapzodalVelTrajectory.h \
    src/MotionControl/QuadraticEquation.h \
    src/MotionControl/QuarticEquation.h \
    src/MotionControl/TrapezoidalVelTrajectory.h \
    src/OtherLibs/cmu/constants.h \
    src/OtherLibs/cmu/fast_alloc.h \
    src/OtherLibs/cmu/kdtree.h \
    src/OtherLibs/cmu/obstacle.h \
    src/OtherLibs/cmu/path_planner.h \
    src/OtherLibs/cmu/util.h \
    src/OtherLibs/cmu/vector.h \
    src/OtherLibs/cornell/Trajectory.h \
    src/OtherLibs/cornell/TrajectoryStructs.h \
    src/OtherLibs/cornell/TrajectorySupport.h \
    src/OtherLibs/nlopt/nlopt.h \
    src/OtherLibs/nlopt/nlopt.hpp \
    src/PathPlan/FastAllocator.h \
    src/PathPlan/KDTreeNew.h \
    src/PathPlan/ObstacleNew.h \
    src/PathPlan/PathPlanner.h \
    src/PathPlan/PredictTrajectory.h \
    src/PathPlan/RRTPathPlanner.h \
    src/PathPlan/RRTPathPlannerNew.h \
    src/PathPlan/Stack.h \
    src/RefereeBox/game_state.h \
    src/RefereeBox/playmode.h \
    src/RefereeBox/RefereeBoxIf.h \
    src/RefereeBox/referee_commands.h \
    src/Simulator/CommandInterface.h \
    src/Simulator/server.h \
    src/Simulator/ServerInterface.h \
    src/Strategy/rolematch/matrix.h \
    src/Strategy/rolematch/munkres.h \
    src/Strategy/rolematch/munkresTacticPositionMatcher.h \
    src/Strategy/skill/BasicPlay.h \
    src/Strategy/skill/Factory.h \
    src/Strategy/skill/Goalie.h \
    src/Strategy/skill/GotoPosition.h \
    src/Strategy/skill/OpenSpeed.h \
    src/Strategy/skill/PlayerTask.h \
    src/Strategy/skill/SmartGotoPosition.h \
    src/Strategy/skill/Speed.h \
    src/Strategy/skill/StopRobot.h \
    src/Strategy/skill/Touch.h \
    src/Utils/BufferCounter.h \
    src/Utils/ClassFactory.h \
    src/Utils/DataQueue.hpp \
    src/Utils/GDebugEngine.h \
    src/Utils/misc_types.h \
    src/Utils/MultiThread.h \
    src/Utils/Semaphore.h \
    src/Utils/singleton.h \
    src/Utils/utils.h \
    src/Utils/ValueRange.h \
    src/Utils/weerror.h \
    src/Vision/BallPredictor.h \
    src/Vision/CollisionSimulator.h \
    src/Vision/mediator/net/message.h \
    src/Vision/RobotPredictData.h \
    src/Vision/RobotPredictError.h \
    src/Vision/RobotPredictor.h \
    src/Vision/RobotsCollision.h \
    src/Vision/VisionModule.h \
    src/Vision/VisionReceiver.h \
    src/Wireless/CommandFactory.h \
    src/Wireless/CommControl.h \
    src/Wireless/PlayerCommand.h \
    src/Wireless/PlayerCommandV2.h \
    src/Wireless/RobotCommand.h \
    src/Wireless/RobotSensor.h \
    src/Wireless/SerialPort.h \
    src/WorldModel/DribbleStatus.h \
    src/WorldModel/KickStatus.h \
    src/WorldModel/PlayInterface.h \
    src/WorldModel/RobotCapability.h \
    src/WorldModel/WorldDefine.h \
    src/WorldModel/WorldModel.h \
    src/Algorithm/ballmodel.h \
    src/Algorithm/BallSpeedModel.h \
    src/Algorithm/BallStatus.h \
    src/Algorithm/CollisionDetect.h \
    src/Algorithm/ContactChecker.h \
    src/Algorithm/runtimepredictor.h



win32-msvc*: QMAKE_LFLAGS += /FORCE:MULTIPLE

QMAKE_CXXFLAGS += -utf-8
unix:!macx{
    QMAKE_CXXFLAGS += -Wno-comment -Wno-reorder -Wno-conversion-null
}

#message($$INCLUDEPATH)

#LD_LIBRARY_PATH=dir：$LD_LIBRARY_PATH
#export LD_LIBRARY_PATH
