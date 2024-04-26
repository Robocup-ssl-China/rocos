#include "globaldata.h"
#include "parammanager.h"
#include "staticparams.h"
namespace  {
auto vpm = ZSS::VParamManager::instance();
auto zpm = ZSS::ZParamManager::instance();
}
CGlobalData::CGlobalData(): ctrlC(false)
    , maintain(ZSS::Athena::Vision::MAINTAIN_STORE_BUFFER)
    , processBall(ZSS::Athena::Vision::BALL_STORE_BUFFER)
    , processRobot(ZSS::Athena::Vision::ROBOT_STORE_BUFFER) {
    for(int i = 0; i < PARAM::CAMERA; i++) {
        cameraMatrix[i].id = i;
        cameraControl[i] = true;
    }
}
CGeoPoint CGlobalData::saoConvert(CGeoPoint originPoint) {
    CGeoPoint result;
    switch (saoAction) {
    case 0:
        result.setX(originPoint.x());
        result.setY(originPoint.y());
        break;
    case 1:
        result.setX(originPoint.y() + 3000);
        result.setY(-originPoint.x());
        break;
    case 2:
        result.setX(originPoint.x() * 3 / 2);
        result.setY(originPoint.y() * 3 / 2);
        break;
    default:
        result.setX(originPoint.x());
        result.setY(originPoint.y());
        break;
    }
    return result;
}

void CGlobalData::saoConvertEdge() {
    switch (saoAction) {
    case 0:
        for (int i = 0; i < PARAM::CAMERA; i++) {
            vpm->loadParam(cameraMatrix[i].leftedge.min,    "Camera" + QString::number(i) + "Leftmin",	cameraMatrix[i].campos.x());
            vpm->loadParam(cameraMatrix[i].leftedge.max,    "Camera" + QString::number(i) + "Leftmax",	cameraMatrix[i].campos.x());
            vpm->loadParam(cameraMatrix[i].rightedge.min,   "Camera" + QString::number(i) + "Rightmin",	cameraMatrix[i].campos.x());
            vpm->loadParam(cameraMatrix[i].rightedge.max,   "Camera" + QString::number(i) + "Rightmax",	cameraMatrix[i].campos.x());
            vpm->loadParam(cameraMatrix[i].upedge.min,      "Camera" + QString::number(i) + "Upmin",	cameraMatrix[i].campos.y());
            vpm->loadParam(cameraMatrix[i].upedge.max,      "Camera" + QString::number(i) + "Upmax",	cameraMatrix[i].campos.y());
            vpm->loadParam(cameraMatrix[i].downedge.min,    "Camera" + QString::number(i) + "Downmin",	cameraMatrix[i].campos.y());
            vpm->loadParam(cameraMatrix[i].downedge.max,    "Camera" + QString::number(i) + "Downmax",	cameraMatrix[i].campos.y());
        }
        break;
    case 1:
        for (int i = 0; i < PARAM::CAMERA; i++) {
            CameraEdge leftedge, rightedge, upedge, downedge;
            vpm->loadParam(leftedge.min,    "Camera" + QString::number(i) + "Leftmin",	cameraMatrix[i].campos.x());
            vpm->loadParam(leftedge.max,    "Camera" + QString::number(i) + "Leftmax",	cameraMatrix[i].campos.x());
            vpm->loadParam(rightedge.min,   "Camera" + QString::number(i) + "Rightmin",	cameraMatrix[i].campos.x());
            vpm->loadParam(rightedge.max,   "Camera" + QString::number(i) + "Rightmax",	cameraMatrix[i].campos.x());
            vpm->loadParam(upedge.min,      "Camera" + QString::number(i) + "Upmin",	cameraMatrix[i].campos.y());
            vpm->loadParam(upedge.max,      "Camera" + QString::number(i) + "Upmax",	cameraMatrix[i].campos.y());
            vpm->loadParam(downedge.min,    "Camera" + QString::number(i) + "Downmin",	cameraMatrix[i].campos.y());
            vpm->loadParam(downedge.max,    "Camera" + QString::number(i) + "Downmax",	cameraMatrix[i].campos.y());
            cameraMatrix[i].leftedge.min	=	downedge.min    + 3000;
            cameraMatrix[i].leftedge.max	=	downedge.max    + 3000;
            cameraMatrix[i].rightedge.min	=	upedge.min      + 3000;
            cameraMatrix[i].rightedge.max	=	upedge.max      + 3000;
            cameraMatrix[i].upedge.min   	=	-leftedge.min        ;
            cameraMatrix[i].upedge.max   	=	-leftedge.max        ;
            cameraMatrix[i].downedge.min	=	-rightedge.min       ;
            cameraMatrix[i].downedge.max	=	-rightedge.max       ;
        }
        break;
    case 2:
        for (int i = 0; i < PARAM::CAMERA; i++) {
            vpm->loadParam(cameraMatrix[i].leftedge.min,    "Camera" + QString::number(i) + "Leftmin",	cameraMatrix[i].campos.x());
            vpm->loadParam(cameraMatrix[i].leftedge.max,    "Camera" + QString::number(i) + "Leftmax",	cameraMatrix[i].campos.x());
            vpm->loadParam(cameraMatrix[i].rightedge.min,   "Camera" + QString::number(i) + "Rightmin",	cameraMatrix[i].campos.x());
            vpm->loadParam(cameraMatrix[i].rightedge.max,   "Camera" + QString::number(i) + "Rightmax",	cameraMatrix[i].campos.x());
            vpm->loadParam(cameraMatrix[i].upedge.min,      "Camera" + QString::number(i) + "Upmin",	cameraMatrix[i].campos.y());
            vpm->loadParam(cameraMatrix[i].upedge.max,      "Camera" + QString::number(i) + "Upmax",	cameraMatrix[i].campos.y());
            vpm->loadParam(cameraMatrix[i].downedge.min,    "Camera" + QString::number(i) + "Downmin",	cameraMatrix[i].campos.y());
            vpm->loadParam(cameraMatrix[i].downedge.max,    "Camera" + QString::number(i) + "Downmax",	cameraMatrix[i].campos.y());
            cameraMatrix[i].leftedge.min	=	cameraMatrix[i].leftedge.min  * 3 / 2;
            cameraMatrix[i].leftedge.max	=	cameraMatrix[i].leftedge.max  * 3 / 2;
            cameraMatrix[i].rightedge.min	=	cameraMatrix[i].rightedge.min * 3 / 2;
            cameraMatrix[i].rightedge.max	=	cameraMatrix[i].rightedge.max * 3 / 2;
            cameraMatrix[i].upedge.min   	=	cameraMatrix[i].upedge.min    * 3 / 2;
            cameraMatrix[i].upedge.max   	=	cameraMatrix[i].upedge.max    * 3 / 2;
            cameraMatrix[i].downedge.min	=	cameraMatrix[i].downedge.min  * 3 / 2;
            cameraMatrix[i].downedge.max	=	cameraMatrix[i].downedge.max  * 3 / 2;
        }
        break;
    }
}

void CGlobalData::setCameraMatrix(bool real) {
    zpm->loadParam(saoAction, "Alert/SaoAction", 0);
    if (real) {
        for (int i = 0; i < PARAM::CAMERA; i++)
        {
            cameraMatrix[i].fillCenter((CGeoPoint(0.0, 0.0)));
            cameraMatrix[i].leftedge.min = 0;
            cameraMatrix[i].leftedge.max = 0;
            cameraMatrix[i].rightedge.min = 0;
            cameraMatrix[i].rightedge.max = 0;
            cameraMatrix[i].downedge.min = 0;
            cameraMatrix[i].downedge.max = 0;
            cameraMatrix[i].upedge.min = 0;
            cameraMatrix[i].upedge.max = 0;
        }
        CameraInit();
    } else {            //for Sim
        cameraMatrix[0].fillCenter(saoConvert(CGeoPoint(2410, 1579))); //(-4500,2250);
        cameraMatrix[1].fillCenter(saoConvert(CGeoPoint(2162, -1529))); //(-1500,2250);
        cameraMatrix[2].fillCenter(saoConvert(CGeoPoint(-2195, -1404))); //(1500,2250);
        cameraMatrix[3].fillCenter(saoConvert(CGeoPoint(-2310, 1477))); //(4500,2250);
        /*
        cameraMatrix[0].fillCenter(saoConvert(CGeoPoint(-3000,2250)));
        cameraMatrix[1].fillCenter(saoConvert(CGeoPoint(3000,2250)));
        cameraMatrix[2].fillCenter(saoConvert(CGeoPoint(-3000,-2250)));
        cameraMatrix[3].fillCenter(saoConvert(CGeoPoint(3000,-2250)));
        */
//        cameraMatrix[4].fillCenter(saoConvert(CGeoPoint(-4500,-2250)));
//        cameraMatrix[5].fillCenter(saoConvert(CGeoPoint(-1500,-2250)));
//        cameraMatrix[6].fillCenter(saoConvert(CGeoPoint(1500,-2250)));
//        cameraMatrix[7].fillCenter(saoConvert(CGeoPoint(4500,-2250)));
        for (int i = 0; i < PARAM::CAMERA; i++) {
            cameraMatrix[i].leftedge.min = cameraMatrix[i].campos.x() - 3300;
            cameraMatrix[i].leftedge.max = cameraMatrix[i].campos.x() - 3000;
            cameraMatrix[i].rightedge.min = cameraMatrix[i].campos.x() + 3300;
            cameraMatrix[i].rightedge.max = cameraMatrix[i].campos.x() + 3000;
            cameraMatrix[i].downedge.min = cameraMatrix[i].campos.y() - 2550;
            cameraMatrix[i].downedge.max = cameraMatrix[i].campos.y() - 2250;
            cameraMatrix[i].upedge.min = cameraMatrix[i].campos.y() + 2550;
            cameraMatrix[i].upedge.max = cameraMatrix[i].campos.y() + 2250;
        }
    }
}

void CGlobalData::CameraInit()
{
    for (int i = 0; i < PARAM::CAMERA; i++)
    {
        cameraMatrix[i].leftedge.min = 0;
        cameraMatrix[i].leftedge.max = 0;
        cameraMatrix[i].rightedge.min = 0;
        cameraMatrix[i].rightedge.max = 0;
        cameraMatrix[i].downedge.min = 0;
        cameraMatrix[i].downedge.max = 0;
        cameraMatrix[i].upedge.min = 0;
        cameraMatrix[i].upedge.max = 0;
    }
}
