#include <iostream>
#include <fstream>

#include <WorldModel.h>
#include <ServerInterface.h>
#include "DecisionModule.h"
#include <VisionReceiver.h>

#include <GDebugEngine.h>
#include "Global.h"
#include <mutex>
#include "CommandInterface.h"
#include "parammanager.h"
#include "zss_debug.pb.h"
#include "Semaphore.h"
#include "RefereeBoxIf.h"
#include "ActionModule.h"
#include <QCoreApplication>

/*! \mainpage Zeus - Run for number one
*
* \section Introduction
*
*	ZJUNlict is robot soccer team of Robocup in Small Size League.
*
* \section Strategy
*	Frame : GameState -> Play -> Agent -> Skill -> Motion Control -> Wireless
*
* \subsection step1: GameState
* \subsection step2: Play
* \subsection step3: Agent
* \subsection step4: Skill
* \subsection step5: Motion Control : PathPlan and Trajectory Generation
*
* etc...
*/

extern Semaphore visionEvent;
extern std::mutex decisionMutex;
/// <summary> For GPU. </summary>
std::mutex* _best_visiondata_copy_mutex = nullptr;
std::mutex* _value_getter_mutex = nullptr;

using PARAM::Latency::TOTAL_LATED_FRAME;

bool VERBOSE_MODE = true;
bool IS_SIMULATION = false;
bool record_run_pos_on = false;
namespace {
COptionModule *option;
CDecisionModule *decision;
CActionModule *action;
CServerInterface::VisualInfo visionInfo;
}

int runLoop() {
    ZSS::ZParamManager::instance()->loadParam(IS_SIMULATION, "Alert/IsSimulation", false);
    initializeSingleton();
    option = new COptionModule();
    CCommandInterface::instance(option);
    vision->registerOption(option);
    vision->startReceiveThread();
    ballModel->registerVision(vision);
    skillapi->registerVision(vision);
    decision = new CDecisionModule(vision);
    action = new CActionModule(vision, decision);
    WORLD_MODEL->registerVision(vision);
    _best_visiondata_copy_mutex = new std::mutex();
    _value_getter_mutex = new std::mutex();
    RefereeBoxInterface::Instance();
    while (true) {
        vision->setNewVision();
        decision->DoDecision();
        action->sendAction();
        GDebugEngine::Instance()->send(option->MyColor() == PARAM::BLUE); //Show two teams debug messages
    }
}
int main(int argc, char* argv[]) {
    QCoreApplication a(argc, argv);
    std::thread t(runLoop);
    t.detach();
    return a.exec();
}
