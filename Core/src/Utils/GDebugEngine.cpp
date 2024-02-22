#include "GDebugEngine.h"
#include "staticparams.h"
#include <cstring>
#include "zss_debug.pb.h"
#include "staticparams.h"
#include "WorldModel.h"
#include "parammanager.h"
namespace{
    ZSS::Protocol::Debug_Msgs guiDebugMsgs;
}
CGDebugEngine::CGDebugEngine(){
    ZSS::ZParamManager::instance()->loadParam(remote_debugger,"Alert/z_remoteADebugger",false);
    ZSS::ZParamManager::instance()->loadParam(remote_address,"Alert/z_remoteDAddr","127.0.0.1");
    ZSS::ZParamManager::instance()->loadParam(remote_port,"Alert/z_remotePort",22001);
}
CGDebugEngine::~CGDebugEngine(){
}

void CGDebugEngine::gui_debug_x(const CGeoPoint& p, int debug_color,int RGB_value){
	CGeoPoint basePos = p;
    static const int x_line_length = 30;
    const CGeoPoint p1 = basePos + CVector( x_line_length, x_line_length);
    const CGeoPoint p2 = basePos + CVector(-x_line_length,-x_line_length);
    gui_debug_line(p1, p2, debug_color,RGB_value);

    const CGeoPoint p3 = basePos + CVector( x_line_length,-x_line_length);
    const CGeoPoint p4 = basePos + CVector(-x_line_length, x_line_length);
    gui_debug_line(p3,p4, debug_color,RGB_value);
}
void CGDebugEngine::gui_debug_points(const std::vector<CGeoPoint> points, int debug_color,int RGB_value){
    debugMutex.lock();
    ZSS::Protocol::Debug_Msg* msg = guiDebugMsgs.add_msgs();
    msg->set_type(ZSS::Protocol::Debug_Msg::Debug_Type::Debug_Msg_Debug_Type_POINTS);
    msg->set_color(ZSS::Protocol::Debug_Msg_Color(debug_color));
    if(debug_color==COLOR_USE_RGB)  msg->set_rgb_value(RGB_value);
    ZSS::Protocol::Debug_Points* debugPoints = msg->mutable_points();
    for (int i=0; i < points.size(); i++) {
        ZSS::Protocol::Point* point = debugPoints->add_point();
        if ( WorldModel::Instance()->option()->MySide() == PARAM::Field::POS_SIDE_RIGHT ){
            point->set_x(-points[i].x());
            point->set_y(-points[i].y());
        }else{
            point->set_x(points[i].x());
            point->set_y(points[i].y());
        }
    }
//    qDebug() << "point size: " << points.size();
    debugMutex.unlock();
}
void CGDebugEngine::gui_debug_line(const CGeoPoint& p1, const CGeoPoint& p2, int debug_color,int RGB_value){
    debugMutex.lock();
    ZSS::Protocol::Debug_Msg* msg = guiDebugMsgs.add_msgs();
    msg->set_type(ZSS::Protocol::Debug_Msg::Debug_Type::Debug_Msg_Debug_Type_LINE);
    msg->set_color(ZSS::Protocol::Debug_Msg_Color(debug_color));
    if(debug_color==COLOR_USE_RGB)  msg->set_rgb_value(RGB_value);
    ZSS::Protocol::Debug_Line* line = msg->mutable_line();
    ZSS::Protocol::Point
		*pos1 = line->mutable_start(),
		*pos2 = line->mutable_end();
	if ( WorldModel::Instance()->option()->MySide() == PARAM::Field::POS_SIDE_RIGHT ){
		pos1->set_x(-p1.x());
        pos1->set_y(-p1.y());
		pos2->set_x(-p2.x());
        pos2->set_y(-p2.y());
	}else{
		pos1->set_x(p1.x());
        pos1->set_y(p1.y());
		pos2->set_x(p2.x());
        pos2->set_y(p2.y());
	}
	line->set_back(false);
    line->set_forward(false);
    debugMutex.unlock();
}
void CGDebugEngine::gui_debug_arc(const CGeoPoint& p, double r, double start_angle, double span_angle, int debug_color,int RGB_value){
    debugMutex.lock();
    ZSS::Protocol::Debug_Msg* msg = guiDebugMsgs.add_msgs();
    msg->set_type(ZSS::Protocol::Debug_Msg::Debug_Type::Debug_Msg_Debug_Type_ARC);
    msg->set_color(ZSS::Protocol::Debug_Msg_Color(debug_color));
    if(debug_color == COLOR_USE_RGB) {
           msg->set_rgb_value(RGB_value);
       }
    ZSS::Protocol::Debug_Arc* arc = msg->mutable_arc();
	arc->set_start(start_angle);
    arc->set_span(span_angle);
    ZSS::Protocol::Rectangle* rec = arc->mutable_rect();
    ZSS::Protocol::Point
		*p1 = rec->mutable_point1(),
		*p2 = rec->mutable_point2();
	PosT center;
	if ( WorldModel::Instance()->option()->MySide() == PARAM::Field::POS_SIDE_RIGHT ){
		center.x = -p.x();
        center.y = -p.y();
	}
	else{
		center.x = p.x();
        center.y = p.y();
	}
	p1->set_x(center.x - r);
    p1->set_y((center.y - r));
	p2->set_x(center.x + r);
    p2->set_y((center.y + r));
    debugMutex.unlock();
}
void CGDebugEngine::gui_debug_triangle(const CGeoPoint& p1, const CGeoPoint& p2, const CGeoPoint& p3, int debug_color,int RGB_value){
    debugMutex.lock();
    ZSS::Protocol::Debug_Msg* msg = guiDebugMsgs.add_msgs();
    msg->set_type(ZSS::Protocol::Debug_Msg::Debug_Type::Debug_Msg_Debug_Type_POLYGON);
    msg->set_color(ZSS::Protocol::Debug_Msg_Color(debug_color));
    if(debug_color == COLOR_USE_RGB) {
           msg->set_rgb_value(RGB_value);
       }
    ZSS::Protocol::Debug_Polygon* tri = msg->mutable_polygon();
    ZSS::Protocol::Point* pos1 = tri->add_vertex();
    ZSS::Protocol::Point* pos2 = tri->add_vertex();
    ZSS::Protocol::Point* pos3 = tri->add_vertex();
	if ( WorldModel::Instance()->option()->MySide() == PARAM::Field::POS_SIDE_RIGHT ){
		pos1->set_x(-p1.x());
        pos1->set_y(-p1.y());
		pos2->set_x(-p2.x());
        pos2->set_y(-p2.y());
		pos3->set_x(-p3.x());
        pos3->set_y(-p3.y());
	}
	else{
		pos1->set_x(p1.x());
        pos1->set_y(p1.y());
		pos2->set_x(p2.x());
        pos2->set_y(p2.y());
		pos3->set_x(p3.x());
        pos3->set_y(p3.y());
    }
    debugMutex.unlock();
}
void CGDebugEngine::gui_debug_robot(const CGeoPoint& p, double robot_dir, int debug_color,int RGB_value){
    debugMutex.lock();
    ZSS::Protocol::Debug_Msg* msg = guiDebugMsgs.add_msgs();
    msg->set_type(ZSS::Protocol::Debug_Msg::Debug_Type::Debug_Msg_Debug_Type_ROBOT);
    msg->set_color(ZSS::Protocol::Debug_Msg_Color(debug_color));
    if(debug_color == COLOR_USE_RGB) {
           msg->set_rgb_value(RGB_value);
       }
    ZSS::Protocol::Debug_Robot* robot = msg->mutable_robot();

	float rdir;
	if ( WorldModel::Instance()->option()->MySide() == PARAM::Field::POS_SIDE_RIGHT ){
		robot_dir = Utils::Normalize(robot_dir + PARAM::Math::PI);
	}
    rdir = robot_dir*180/PARAM::Math::PI; // 转成角度
	robot->set_dir(rdir);

    ZSS::Protocol::Point* robot_pos = robot->mutable_pos();

	if ( WorldModel::Instance()->option()->MySide() == PARAM::Field::POS_SIDE_RIGHT ){
		robot_pos->set_x(-p.x());
        robot_pos->set_y(-p.y());
	}
	else{
		robot_pos->set_x(p.x());
        robot_pos->set_y(p.y());
    }
    debugMutex.unlock();
}
void CGDebugEngine::gui_debug_msg(const CGeoPoint& p, const std::string& msgstr, int debug_color,int RGB_value, const double size, const int weight){
    gui_debug_msg(p,msgstr.c_str(),debug_color,RGB_value,size,weight);
}
void CGDebugEngine::gui_debug_msg(const CGeoPoint& p, const char* msgstr, int debug_color,int RGB_value, const double size, const int weight){
    gui_debug_msg_fix(p*(WorldModel::Instance()->option()->MySide() == PARAM::Field::POS_SIDE_RIGHT?-1:1),msgstr,debug_color,RGB_value,size,weight);
}
void CGDebugEngine::gui_debug_msg_fix(const CGeoPoint& p, const std::string& msgstr, int debug_color,int RGB_value, const double size, const int weight){
    gui_debug_msg_fix(p,msgstr.c_str(),debug_color,RGB_value,size,weight);
}
void CGDebugEngine::gui_debug_msg_fix(const CGeoPoint& p, const char* msgstr, int debug_color,int RGB_value, const double size, const int weight)
{
    debugMutex.lock();
    ZSS::Protocol::Debug_Msg* msg = guiDebugMsgs.add_msgs();
    msg->set_type(ZSS::Protocol::Debug_Msg::Debug_Type::Debug_Msg_Debug_Type_TEXT);
    msg->set_color(ZSS::Protocol::Debug_Msg_Color(debug_color));
    if(debug_color == COLOR_USE_RGB) {
           msg->set_rgb_value(RGB_value);
       }
    ZSS::Protocol::Debug_Text* text = msg->mutable_text();
    text->set_size(size);
    text->set_weight(weight);
    ZSS::Protocol::Point* center = text->mutable_pos();
    center->set_x(p.x());
    center->set_y(p.y());
    text->set_text(msgstr);
    debugMutex.unlock();
}
void CGDebugEngine::send(bool teamIsBlue){
    static QByteArray data;
    debugMutex.lock();
    int size = guiDebugMsgs.ByteSizeLong();
    data.resize(size);
    guiDebugMsgs.SerializeToArray(data.data(),size);
    int sent_size = 0;
    int port = ZSS::Medusa::DEBUG_MSG_SEND[teamIsBlue?0:1];
    sent_size = sendSocket.writeDatagram(data,data.size(),QHostAddress(ZSS::LOCAL_ADDRESS),port);
    if(remote_debugger){
        sendSocket.writeDatagram(data,data.size(),QHostAddress(remote_address),teamIsBlue?remote_port:remote_port+1);
    }
//    std::cout << "size: " << data.size() << ' ' << sent_size << std::endl;
    guiDebugMsgs.clear_msgs();
    debugMutex.unlock();
}
