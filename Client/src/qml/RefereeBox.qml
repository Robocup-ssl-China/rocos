import QtQuick 2.10
import QtQuick.Controls 2.3
import QtQuick.Controls.Styles 1.4
import ZSS 1.0 as Client
ScrollView {
    Client.RefereeBox { id : refereeBox; }
    Timer {
        id:refBoxTimer;
        interval:16;
        running:false;
        repeat:true;
        onTriggered: {
            refereeBox.multicastCommand();
        }
    }
    id : root;
    property var color : ["#ddd","#79a6ea","#fbeb9c"];
    property int state : Client.GameState.HALTED;
    property bool haltGame : false;
    property bool stopGame : true;
    property bool forceStart : false;
    property bool normalStart : false;
    property bool teamControl : false;
    property int next_command : Client.RefBoxCommand.HALT;
    function getButtonsCommand(index){
        switch(index){
        case Client.RefBoxCommand.HALT:
            state = Client.GameState.HALTED;break;
        case Client.RefBoxCommand.STOP_GAME:
        case Client.RefBoxCommand.BALL_PLACEMENT_YELLOW:
        case Client.RefBoxCommand.BALL_PLACEMENT_BLUE:
            state = Client.GameState.STOPPED;break;
        case Client.RefBoxCommand.FORCE_START:
        case Client.RefBoxCommand.NORMAL_START:
        case Client.RefBoxCommand.DIRECT_KICK_YELLOW:
        case Client.RefBoxCommand.INDIRECT_KICK_YELLOW:
        case Client.RefBoxCommand.DIRECT_KICK_BLUE:
        case Client.RefBoxCommand.INDIRECT_KICK_BLUE:
            state = Client.GameState.RUNNING;break;
        case Client.RefBoxCommand.TIMEOUT_YELLOW:
            state = Client.GameState.TIMEOUT_YELLOW;break;
        case Client.RefBoxCommand.TIMEOUT_BLUE:
            state = Client.GameState.TIMEOUT_BLUE;break;
        case Client.RefBoxCommand.KICKOFF_YELLOW:
        case Client.RefBoxCommand.KICKOFF_BLUE:
            state = Client.GameState.PREPARE_KICKOFF;break;
        case Client.RefBoxCommand.PENALTY_YELLOW:
        case Client.RefBoxCommand.PENALTY_BLUE:
            state = Client.GameState.PREPARE_PENALTY;break;
        default:
            console.log("RefBox Command ERROR!!!!!!");
            return;
        }
        controlButtons(state);
        refereeBox.changeCommand(index);
    }
    function controlButtons(state){
        function unify(onOff){ haltGame = stopGame = forceStart = normalStart = teamControl = onOff; }
        switch(state){
        case Client.GameState.HALTED:
            unify(false);
            stopGame = true;
            break;
        case Client.GameState.PREPARE_KICKOFF:
        case Client.GameState.PREPARE_PENALTY:
            unify(false);
            normalStart = true;
            break;
        case Client.GameState.RUNNING:
        case Client.GameState.TIMEOUT_BLUE:
        case Client.GameState.TIMEOUT_YELLOW:
            unify(false);
            haltGame = stopGame = true;
            break;
        case Client.GameState.STOPPED:
            unify(true);
            stopGame = normalStart = false;
            break;
        }
    }
    Grid{
        id : refereeSetting;
        width:parent.width;
        columnSpacing: 0;
        rowSpacing: 14;
        padding: 5;
        columns: 1;
        topPadding: 15;
        horizontalItemAlignment: Grid.AlignHCenter;
        verticalItemAlignment: Grid.AlignVCenter;
        anchors.horizontalCenter: parent.horizontalCenter;
        property int itemWidth : (width - (columns-1) * columnSpacing - 2*padding)/columns;
        ZGroupBox{
            title:qsTr("Referee Setting");
            property bool visionGetter : false;
            Grid{
                columns: 1;
                columnSpacing: 0;
                rowSpacing: 0;
                verticalItemAlignment: Grid.AlignVCenter;
                horizontalItemAlignment: Grid.AlignLeft;
                width:parent.width;
                padding: 5;
                property int itemWidth : width - 2*padding;
                Button{
                    id:refBoxSwitch;
                    text:(refBoxSwitch.refereeSwitch ? qsTr("Stop") : qsTr("Send"));
                    width:parent.itemWidth;
                    property bool refereeSwitch: false;
                    onClicked: changeState();
                    function changeState(){
                        refBoxSwitch.refereeSwitch = !refBoxSwitch.refereeSwitch;
                        run();
                    }
                    function run(){
                        if(refBoxSwitch.refereeSwitch){
                            refBoxTimer.start();
                        }else{
                            refBoxTimer.stop();
                        }
                    }
                    Component.onCompleted: run();
                }
            }
        }
    }
    Grid {
        id : control;
        anchors.top: refereeSetting.bottom;
        width:parent.width;
        columnSpacing: 0;
        rowSpacing: 14;
        columns: 1;
        topPadding: 15;
        padding: 5;
        horizontalItemAlignment: Grid.AlignHCenter;
        verticalItemAlignment: Grid.AlignVCenter;
        anchors.horizontalCenter: parent.horizontalCenter;
        property int itemWidth : width - 2*padding;
        enabled: refBoxSwitch.refereeSwitch;
        ZGroupBox{
            title:qsTr("Control Command");
            Grid{
                width:parent.width;
                columns:2;
                property int buttonWidth : (width - (columns-1) * columnSpacing - 2*padding)/columns;
                rowSpacing: 0;
                columnSpacing: 10;
                spacing: 0;
                padding: 0;
                ZRefButton{
                    property int index : Client.RefBoxCommand.HALT;
                    text:qsTr("Halt");
                    onClicked:getButtonsCommand(index);
                    enabled: haltGame;
                }
                ZRefButton{
                    property int index : Client.RefBoxCommand.STOP_GAME;
                    text:qsTr("Stop Game");
                    onClicked:getButtonsCommand(index);
                    enabled: stopGame;
                }
                ZRefButton{
                    property int index : Client.RefBoxCommand.FORCE_START;
                    text:qsTr("Force Start");
                    onClicked:getButtonsCommand(index);
                    enabled: forceStart;
                }
                ZRefButton{
                    property int index : Client.RefBoxCommand.NORMAL_START;
                    text:qsTr("Normal Start");
                    onClicked:getButtonsCommand(index);
                    enabled: normalStart;
                }
            }
        }
        ZGroupBox{
            title:qsTr("Yellow Team Control");
            enabled: teamControl;
            Grid{
                width:parent.width;
                rows:4;
                rowSpacing: 0;
                columns:1;
                property int buttonWidth : (width - (columns-1) * columnSpacing - 2*padding)/columns;
                Grid{
                    width:parent.width;
                    columns:2;
                    columnSpacing: 10;
                    property int buttonWidth : (width - (columns-1) * columnSpacing - 2*padding)/columns;
                    Repeater{
                        model:[
                            {cmd_type:2,cmd_num:Client.RefBoxCommand.KICKOFF_YELLOW,cmd_str:"Kick off"},
                            {cmd_type:2,cmd_num:Client.RefBoxCommand.PENALTY_YELLOW,cmd_str:"Penalty"},
                            {cmd_type:2,cmd_num:Client.RefBoxCommand.DIRECT_KICK_YELLOW,cmd_str:"Direct Kick"},
                            {cmd_type:2,cmd_num:Client.RefBoxCommand.INDIRECT_KICK_YELLOW,cmd_str:"Indirect Kick"},
                            {cmd_type:2,cmd_num:Client.RefBoxCommand.TIMEOUT_YELLOW,cmd_str:"Timeout"},
                            {cmd_type:2,cmd_num:Client.RefBoxCommand.BALL_PLACEMENT_YELLOW,cmd_str:"Ball Placement"},
                        ]
                        ZRefButton{
                            property int index : modelData.cmd_num;
                            text:qsTr(modelData.cmd_str);
                            onClicked:getButtonsCommand(index);
                            Component.onCompleted: {
                                color = root.color[modelData.cmd_type];
                            }
                        }
                    }
                }
            }
        }
        ZGroupBox{
            title:qsTr("Blue Team Control");
            enabled: teamControl;
            Grid{
                width:parent.width;
                rows:4;
                columns:1;
                rowSpacing: 0;
                property int buttonWidth : (width - (columns-1) * columnSpacing - 2*padding)/columns;
                Grid{
                    width:parent.width;
                    columns:2;
                    columnSpacing: 10;
                    property int buttonWidth : (width - (columns-1) * columnSpacing - 2*padding)/columns;
                    Repeater{
                        model:[
                            {cmd_type:1,cmd_num:Client.RefBoxCommand.KICKOFF_BLUE,cmd_str:"Kick off"},
                            {cmd_type:1,cmd_num:Client.RefBoxCommand.PENALTY_BLUE,cmd_str:"Penalty"},
                            {cmd_type:1,cmd_num:Client.RefBoxCommand.DIRECT_KICK_BLUE,cmd_str:"Direct Kick"},
                            {cmd_type:1,cmd_num:Client.RefBoxCommand.INDIRECT_KICK_BLUE,cmd_str:"Indirect Kick"},
                            {cmd_type:1,cmd_num:Client.RefBoxCommand.TIMEOUT_BLUE,cmd_str:"Timeout"},
                            {cmd_type:1,cmd_num:Client.RefBoxCommand.BALL_PLACEMENT_BLUE,cmd_str:"Ball Placement"},
                        ]
                        ZRefButton{
                            property int index : modelData.cmd_num;
                            text:qsTr(modelData.cmd_str);
                            onClicked:getButtonsCommand(index);
                            Component.onCompleted: {
                                color = root.color[modelData.cmd_type];
                            }
                        }
                    }
                }
            }
        }
        ZGroupBox{
            title:qsTr("Next Command");
            Grid{
                width:parent.width;
                columns:2;
                property int buttonWidth : (width - (columns-1) * columnSpacing - 2*padding)/columns;
                rowSpacing: 0;
                columnSpacing: 10;
                spacing: 0;
                padding: 0;
                Repeater{
                    model: [
                        {cmd_type:0,cmd_str:"Stop Game",cmd_num:Client.RefBoxCommand.STOP_GAME},
                        {cmd_type:0,cmd_str:"Force Start",cmd_num:Client.RefBoxCommand.FORCE_START},
                        {cmd_type:2,cmd_str:"Direct Yellow",cmd_num:Client.RefBoxCommand.DIRECT_KICK_YELLOW},
                        {cmd_type:1,cmd_str:"Direct Blue",cmd_num:Client.RefBoxCommand.DIRECT_KICK_BLUE},
                        {cmd_type:2,cmd_str:"Indirect Yellow",cmd_num:Client.RefBoxCommand.INDIRECT_KICK_YELLOW},
                        {cmd_type:1,cmd_str:"Indirect Blue",cmd_num:Client.RefBoxCommand.INDIRECT_KICK_BLUE},
                    ]
                    ZRefButton{
                        property int index : modelData.cmd_num;
                        text:qsTr(modelData.cmd_str);
                        highlighted:next_command == index;
                        onClicked: {
                            root.next_command = index;
                            refereeBox.setNextCommand(index);
                        }
                        Component.onCompleted: {
                            color = root.color[modelData.cmd_type];
                        }
                    }
                }
            }
        }
    }
}
