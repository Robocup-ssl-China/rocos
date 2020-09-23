import QtQuick 2.6
import QtQuick.Controls 1.4
import QtQuick.Controls.Styles 1.4
import QtQuick.Dialogs 1.2
import Client.Component 1.0 as Client
ApplicationWindow{
    visible:true;
    width:580;
    height:410;
    /*
    //固定窗口大小
    minimumHeight: height;
    minimumWidth: width;
    maximumHeight: height;
    maximumWidth: width;
    */
    color:"lightgrey";
    id:window;
    Client.Serial { id : serial; }
    Client.Translator{ id : translator; }
    Timer{
        id:timer;
        interval:15;//15ms启动一次
        running:false;
        repeat:true;
        onTriggered: {
            crazyShow.updateCommand();//调用serial.updateCommandParams()
            serial.sendCommand();//把数据发出去
        }
    }
    Rectangle{
        id : radio;
        anchors.fill: parent;
        Rectangle{
            width:parent.width;
            anchors.top: parent.top;
            anchors.bottom: parent.bottom;
            color : "lightgrey";
            id:radioRectangle;
            //最上面一条Box
            GroupBox{
                id : crazyListRectangle;
                width: parent.width - 15;
                anchors.horizontalCenter: parent.horizontalCenter;
                anchors.top: parent.top;
                anchors.margins: 10;
                title :qsTr("Sender Setting") + translator.emptyString;
                Grid{
                    height:parent.height;
                    id : crazyListView;
                    verticalItemAlignment: Grid.AlignVCenter;
                    horizontalItemAlignment: Grid.AlignLeft;
                    anchors.horizontalCenter: parent.horizontalCenter;
                    columnSpacing: 40;
                    rowSpacing: 5;
                    columns:4;
                    enabled: !crazyConnect.ifConnected;
                    property int itemWidth : 90;
                    //端口相关
                    Text{
                        text: qsTr("Ports")+ translator.emptyString;
                        width:parent.itemWidth;
                    }
                    ComboBox{
                        model:serial.getCrazySetting(0);
                        currentIndex : serial.getDefaultIndex(0);
                        onActivated: serial.sendCrazySetting(0,index);
                        width:parent.itemWidth;
                    }
                    //频率相关
                    Text{
                        text: qsTr("Frequency")+ translator.emptyString;
                        width:parent.itemWidth;
                    }
                    ComboBox{
                        model:serial.getCrazySetting(1);
                        currentIndex : serial.getDefaultIndex(1);
                        onActivated: serial.sendCrazySetting(1,index);
                        width:parent.itemWidth;
                    }
                }
            }
            //连接按钮
            Button{
                id : crazyConnect;
                text : (ifConnected ? qsTr("Disconnect") : qsTr("Connect")) + translator.emptyString;
                property bool ifConnected:false;
                anchors.top: crazyListRectangle.bottom;
                anchors.right: parent.right;
                anchors.rightMargin: 20;
                anchors.topMargin: 10;
                onClicked: clickEvent();
                function clickEvent(){
                    if(ifConnected){
                        timer.stop();
                        if(crazyStart.ifStarted) crazyStart.handleClickEvent();
                        serial.closeSerialPort();
                    }else{
                        serial.openSerialPort();
                        serial.sendStartPacket();
                    }
                    ifConnected = !ifConnected;
                }
            }
            //下面大的Box
            GroupBox{
                title : qsTr("Manual Control") + translator.emptyString;
                width:parent.width - 15;
                anchors.top:crazyConnect.bottom;
                anchors.horizontalCenter: parent.horizontalCenter;
                id : groupBox2;
                Grid{
                    id : crazyShow;
                    columns: 6;//6列
                    verticalItemAlignment: Grid.AlignVCenter;
                    horizontalItemAlignment: Grid.AlignLeft;
                    anchors.horizontalCenter: parent.horizontalCenter;
                    columnSpacing: 10;
                    rowSpacing: 5;
                    property int robotID : 1;//Robot
                    property int velX : 0;//Vx
                    property int velY : 0;//Vy
                    property int velR : 0;//Vr
                    property bool shoot : false;//Shoot
                    property bool dribble : false;//Dribb
                    property bool rush : false;//Rush

                    property int velXStep : 20;//VxStep
                    property int velYStep : 20;//VyStep
                    property int velRStep : 20;//VrStep
                    property bool mode : false;//KickMode
                    property int dribbleLevel : 2;//DribLevel
                    property int rushSpeed : 20;//RushSpeed

                    property int m_VEL : 255//MaxVel
                    property int velocityRMax : 511;//MaxVelR
                    property int power : 20;//KickPower

                    property int m_VELR : 1023
                    property int velocityMax : 511;//最大速度
                    property int dribbleMaxLevel : 3;//吸球最大等级
                    property int kickPowerMax: 127;//最大踢球力量
                    property int itemWidth : 70;

                    Text{ text:qsTr("Robot") + translator.emptyString }
                    //最多12辆车
                    SpinBox{ minimumValue:1; maximumValue:12; value:parent.robotID; width:parent.itemWidth
                        onEditingFinished:{parent.robotID = value}}
                    Text{ text:"Stop" }
                    //有用吗？
                    Button{ text:qsTr("[Space]") + translator.emptyString;width:parent.itemWidth
                    }
                    Text{ text:" " }
                    Text{ text:" " }
                    Text{ text:qsTr("Vx [W/S]") + translator.emptyString }
                    //Vx:(-m_VEL, m_VEL)
                    SpinBox{ minimumValue:-crazyShow.m_VEL; maximumValue:crazyShow.m_VEL; value:parent.velX;width:parent.itemWidth
                        onEditingFinished:{parent.velX = value;}}
                    Text{ text:qsTr("VxStep") + translator.emptyString }
                    //VxStep:(1, m_VEL)
                    SpinBox{ minimumValue:1; maximumValue:crazyShow.m_VEL; value:parent.velXStep;width:parent.itemWidth;
                        onEditingFinished:{parent.velXStep = value;}}
                    Text{ text:qsTr("MaxVel") + translator.emptyString }
                    //MaxVel:(1, velocityMax)
                    SpinBox{ minimumValue:1; maximumValue:crazyShow.velocityMax; value:parent.m_VEL;width:parent.itemWidth
                        onEditingFinished:{parent.m_VEL = value;}}
                    Text{ text:qsTr("Vy [A/D]") + translator.emptyString}
                    //Vy:(-m_VEL, m_VEL)
                    SpinBox{ minimumValue:-crazyShow.m_VEL; maximumValue:crazyShow.m_VEL; value:parent.velY;width:parent.itemWidth
                        onEditingFinished:{parent.velY = value;}}
                    Text{ text:qsTr("VyStep") + translator.emptyString }
                    //VyStep:(1, m_VEL)
                    SpinBox{ minimumValue:1; maximumValue:crazyShow.m_VEL; value:parent.velYStep;width:parent.itemWidth
                        onEditingFinished:{parent.velYStep = value;}}
                    Text{ text:" " }
                    Text{ text:" " }
                    Text{ text:qsTr("Vr [Left/Right]")  + translator.emptyString}
                    //Vr:(-m_VEL, m_VEL)
                    SpinBox{ minimumValue:-crazyShow.m_VELR; maximumValue:crazyShow.m_VELR; value:parent.velR;width:parent.itemWidth
                        onEditingFinished:{parent.velR = value;}}
                    Text{ text:qsTr("VrStep") + translator.emptyString }
                    //VrStep:(1, m_VELR)
                    SpinBox{ minimumValue:1; maximumValue:crazyShow.m_VELR; value:parent.velRStep;width:parent.itemWidth
                        onEditingFinished:{parent.velRStep = value;}}
                    Text{ text:qsTr("MaxVelR") + translator.emptyString }
                    //MaxVelR:(1, velocityRMax)
                    SpinBox{ minimumValue:1; maximumValue:crazyShow.velocityRMax; value:parent.m_VELR;width:parent.itemWidth
                        onEditingFinished:{parent.m_VELR = value;}}
                    Text{ text:qsTr("Shoot [E]") + translator.emptyString}
                    Button{ text:(parent.shoot? qsTr("true") : qsTr("false")) + translator.emptyString;width:parent.itemWidth
                        onClicked: {
                            parent.shoot = !parent.shoot;
                        }
                    }

                    Text{ text:qsTr("KickMode [Up]")  + translator.emptyString}
                    Button{ text:(parent.mode?qsTr("chip"):qsTr("flat")) + translator.emptyString;width:parent.itemWidth
                        onClicked: {
                            parent.mode = !parent.mode
                        }
                    }
                    Text{ text:qsTr("KickPower") + translator.emptyString }
                    //KickPower:(1, kickPowerMax)
                    SpinBox{ minimumValue:0; maximumValue:parent.kickPowerMax; value:parent.power;width:parent.itemWidth
                        onEditingFinished:{parent.power = value;}}
                    Text{ text:qsTr("Dribb [Q]") + translator.emptyString }
                    Button{ text:(parent.dribble ? qsTr("true") : qsTr("false")) +translator.emptyString;width:parent.itemWidth
                        onClicked: {
                            parent.dribble = !parent.dribble;
                        }
                    }
                    Text{ text:qsTr("DribLevel")  + translator.emptyString}
                    //DribLevel:(0, dribbleMaxLevel)
                    SpinBox{ minimumValue:0; maximumValue:crazyShow.dribbleMaxLevel; value:parent.dribbleLevel;width:parent.itemWidth
                        onEditingFinished:{parent.dribbleLevel = value;}}
                    Text{ text:" " }
                    Text{ text:" " }
                    Text{ text:qsTr("Rush [G]") + translator.emptyString }
                    Button{ text:(parent.rush ? qsTr("true") : qsTr("false")) +translator.emptyString;width:parent.itemWidth;
                        onClicked: {
                            parent.rush = !parent.rush;
                            crazyShow.updateRush();
                        }
                    }
                    Text{ text:qsTr("RushSpeed")  + translator.emptyString}
                    //RushSpeed:(0, m_VEL)
                    SpinBox{ minimumValue:0; maximumValue:crazyShow.m_VEL; value:parent.rushSpeed;width:parent.itemWidth
                        onEditingFinished:{parent.rushSpeed = value;}}
                    Rectangle{
                        width:parent.itemWidth; height:20; color:parent.shoot ? "red" : "lightgrey";
                    }

                    //键盘响应实现
                    Keys.onPressed:getFocus(event);
                    function getFocus(event){
                        switch(event.key){
                        case Qt.Key_Enter:
                        case Qt.Key_Return:
                        case Qt.Key_Escape:
                            crazyShow.focus = true;
                            break;
                        default:
                            event.accepted = false;
                            return false;
                        }
                        event.accepted = true;
                    }
                    function updateStop(){
                        crazyShow.velX = 0;
                        crazyShow.velY = 0;
                        crazyShow.velR = 0;
                        crazyShow.shoot = false;
                        crazyShow.dribble = false;
                        crazyShow.rush = false;
                    }
                    function updateRush(){
                        if(crazyShow.rush){
                            crazyShow.velX = crazyShow.rushSpeed;
                            crazyShow.velY = 0;
                            crazyShow.velR = 0;
                            crazyShow.shoot = true;
                            crazyShow.dribble = false;
                        }else{
                            crazyShow.updateStop();
                        }
                    }
                    function handleKeyboardEvent(e){
                        switch(e){
                        case 'U':{crazyShow.mode = !crazyShow.mode;break;}
                        case 'a':{crazyShow.velY = crazyShow.limitVel(crazyShow.velY-crazyShow.velYStep,-crazyShow.m_VEL,crazyShow.m_VEL);
                            break;}
                        case 'd':{crazyShow.velY = crazyShow.limitVel(crazyShow.velY+crazyShow.velYStep,-crazyShow.m_VEL,crazyShow.m_VEL);
                            break;}
                        case 'w':{crazyShow.velX = crazyShow.limitVel(crazyShow.velX+crazyShow.velXStep,-crazyShow.m_VEL,crazyShow.m_VEL);
                            break;}
                        case 's':{crazyShow.velX = crazyShow.limitVel(crazyShow.velX-crazyShow.velXStep,-crazyShow.m_VEL,crazyShow.m_VEL);
                            break;}
                        case 'q':{crazyShow.dribble = !crazyShow.dribble;
                            break;}
                        case 'e':{crazyShow.shoot = !crazyShow.shoot;
                            break;}
                        case 'L':{crazyShow.velR = crazyShow.limitVel(crazyShow.velR-crazyShow.velRStep,-crazyShow.m_VELR,crazyShow.m_VELR);
                            break;}
                        case 'R':{crazyShow.velR = crazyShow.limitVel(crazyShow.velR+crazyShow.velRStep,-crazyShow.m_VELR,crazyShow.m_VELR);
                            break;}
                        case 'S':{crazyShow.updateStop();
                            break;}
                        case 'g':{crazyShow.rush = !crazyShow.rush;
                            updateRush();
                            break;}
                        default:
                            return false;
                        }
                        updateCommand();
                    }
                    //serial.updateCommandParams在C++中实现
                    function updateCommand(){
                        serial.updateCommandParams(crazyShow.robotID,crazyShow.velX,crazyShow.velY,crazyShow.velR,crazyShow.dribble,crazyShow.dribbleLevel,crazyShow.mode,crazyShow.shoot,crazyShow.power);
                    }
                    function limitVel(vel,minValue,maxValue){
                        if(vel>maxValue) return maxValue;
                        if(vel<minValue) return minValue;
                        return vel;
                    }
                    Shortcut{
                        sequence:"G";
                        onActivated:crazyShow.handleKeyboardEvent('g');
                    }
                    Shortcut{
                        sequence:"A";
                        onActivated:crazyShow.handleKeyboardEvent('a');
                    }
                    Shortcut{
                        sequence:"Up";
                        onActivated:crazyShow.handleKeyboardEvent('U');
                    }
                    Shortcut{
                        sequence:"D"
                        onActivated:crazyShow.handleKeyboardEvent('d');
                    }
                    Shortcut{
                        sequence:"W"
                        onActivated:crazyShow.handleKeyboardEvent('w');
                    }
                    Shortcut{
                        sequence:"S"
                        onActivated:crazyShow.handleKeyboardEvent('s');
                    }
                    Shortcut{
                        sequence:"Q"
                        onActivated:crazyShow.handleKeyboardEvent('q');
                    }
                    Shortcut{
                        sequence:"E"
                        onActivated:crazyShow.handleKeyboardEvent('e');
                    }
                    Shortcut{
                        sequence:"Left"
                        onActivated:crazyShow.handleKeyboardEvent('L');
                    }
                    Shortcut{
                        sequence:"Right"
                        onActivated:crazyShow.handleKeyboardEvent('R');
                    }
                    Shortcut{
                        sequence:"Space"
                        onActivated:crazyShow.handleKeyboardEvent('S');
                    }
                }
            }
            //最下面的Start按钮
            Button{
                id:crazyStart;
                text:qsTr("Start") + translator.emptyString;
                property bool ifStarted:false;
                anchors.right:parent.right;
                anchors.rightMargin: 20;
                anchors.top:groupBox2.bottom;
                anchors.topMargin: 10;
                enabled : crazyConnect.ifConnected;//如果连接成功按钮才有效
                onClicked:{
                    handleClickEvent();
                }
                function handleClickEvent(){
                    if(ifStarted){//若开始，定时器关闭
                        timer.stop();
                    }else{//若未开始，定时器打开
                        timer.start();
                    }
                    ifStarted = !ifStarted;
                    text = (ifStarted ? qsTr("Stop") : qsTr("Start")) + translator.emptyString;
                }
            }
        }
    }
    Component.onCompleted:{
        //translator.selectLanguage("zh");
    }
}


