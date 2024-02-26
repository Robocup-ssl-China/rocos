import QtQuick 2.10
import QtQuick.Controls 2.3
import QtQuick.Controls.Styles 1.4
import QtQuick.Dialogs 1.2
import QtQuick.Layouts 1.3
import ZSS 1.0 as ZSS
Page{
    id:control;
    property bool socketConnect : false;
    property bool radioConnect : false;
    property bool medusaConnect : false;
    property bool medusaConnect2 : false;
    property bool simConnect : false;
    property bool crazyConnect : false;
    property bool ifEdgeTest : false;
    property bool isRecoring: true;
    property bool monitorConnect : false;
    property bool isLogPlay: false;
    property bool isRecFile: false;
    property bool isReplaying: false;
    property bool isCutting: true;
    property int playspeed: 1

    Timer{
        id:oneSecond;
        interval:800;
        running:true;
        repeat:true;
        onTriggered: {
            interaction.updateInterfaces();
            interfaces4vision.updateModel();
            radioComboBox.updateModel();
//            interfaces4BlueSender.updateModel();
//            interfaces4BlueReceiver.updateModel();
//            interfaces4YellowSender.updateModel();
//            interfaces4YellowReceiver.updateModel();
        }
    }

    ZSS.LogSlider{
        id: log
    }
    ZSS.LogEventLabel{
        id: eventlabel
    }

    ZSS.Interaction{
        id:interaction;
    }
    ZSS.Interaction4Field{
        id:interaction4field
    }

    property variant shortCutString:["`","1","2","3","4","5","6","7","8","9","0","-","=","i","o","p","[",
        "Ctrl+`","Ctrl+1","Ctrl+2","Ctrl+3","Ctrl+4","Ctrl+5","Ctrl+6","Ctrl+7","Ctrl+8","Ctrl+9","Ctrl+0","Ctrl+-","Ctrl+=","Ctrl+i","Ctrl+o","Ctrl+p","Ctrl+["];
    property variant recplayerShortCut: ["m", "Left", "Right", "Up", "Down"];
    function robotControl(index){

        var team = (index > 16 ? 1 : 0);
        var id = index%17;
        interaction.robotControl(id,team)
    }
    header:TabBar {
        id: bar
        width:parent.width;
        contentHeight:50;
        height:contentHeight;
        position: TabBar.Header;
        TabButton {
            icon.source:"/source/camera.png";
        }
        TabButton {
            icon.source:"/source/referee.png";
        }
//        TabButton {
//            icon.source:"/source/viewer.png";
//        }
       TabButton {
           icon.source:"/source/joy3.png";
       }
        TabButton {
            icon.source:"/source/settings.png";
        }
    }

    StackLayout {
        id:controlLayout;
        width: parent.width;
        height:parent.height; //fix display bug at low resolution.2019.3.29 wz
        currentIndex: bar.currentIndex;
        Grid{
            padding: 5;
            topPadding: 15;
            width:controlLayout.width;
            //height:controlLayout.height;
            columns: 1;
            horizontalItemAlignment: Grid.AlignHCenter;
            verticalItemAlignment: Grid.AlignVCenter;
            id:vision
            columnSpacing: 0;
            rowSpacing: 0;
            property int itemWidth : width - padding*2;
            ZGroupBox{
                title: qsTr("Vision")
                Column{
                    id:visionControls;
                    width:parent.width;
                    height:parent.height;
                    spacing: 0;
                    padding:0;
                    property int itemWidth : width - 2*padding;
                    property bool ifConnected : false;
                    ZComboBox{
                        id:interfaces4vision;
                        model:interaction.getInterfaces();
                        onActivated: interaction.changeVisionInterface(currentIndex);
                        function updateModel(){
                            model = interaction.getInterfaces();
                            if(currentIndex >= 0)
                                interaction.changeVisionInterface(currentIndex);
                        }
                        Component.onCompleted: {
                            interaction.getInterfaces();
                        }
                    }
//                    ZComboBox{
//                        id:grsimInterface;
//                        model:interaction.getGrsimInterfaces();
//                        onActivated: interaction.changeGrsimInterface(currentIndex);
//                        function updateModel(){
//                            model = interaction.getGrsimInterfaces();
//                            if(currentIndex >= 0)
//                                interaction.changeGrsimInterface(currentIndex);
//                        }
//                        Component.onCompleted: {
//                            interaction.getGrsimInterfaces();
//                        }
//                    }
                    ZSwitch{
                        id:simulation;
                        width:parent.itemWidth;
                        leftText:qsTr("Simulation");
                        rightText:qsTr("Real");
                    }
                    Grid{
                        id:cameraControls;
                        width:parent.itemWidth - 10;
                        columns:8;
                        columnSpacing: 0;
                        rowSpacing: 0;
                        horizontalItemAlignment: Grid.AlignHCenter;
                        verticalItemAlignment: Grid.AlignVCenter;
                        anchors.horizontalCenter: parent.horizontalCenter;
                        property int itemWidth : (width - (columns-1) * columnSpacing - 2*padding)/columns;
                        Repeater{
                            model:interaction.getCameraNumber();
                            CheckBox{
                                property int itemIndex : index;
                                checked: itemIndex == 0;
                                width:cameraControls.itemWidth;
                                height:40;
                                //anchors.fill: parent;
                                onCheckStateChanged: {
                                    interaction.controlCamera(itemIndex,checked);
                                }
                            }
                        }
                    }
                    Button{
                        width:parent.itemWidth;
                        icon.source:visionControls.ifConnected ? "/source/connect.png" : "/source/disconnect.png";
                        onClicked: {
                            visionControls.ifConnected = !visionControls.ifConnected;
                            interaction.setVision(visionControls.ifConnected,simulation.checked);
                        }
                    }
                }
            }
            ZGroupBox{
                title: qsTr("Radio")
                Grid{
                    width:parent.width;
                    verticalItemAlignment: Grid.AlignVCenter;
                    horizontalItemAlignment: Grid.AlignHCenter;
                    spacing: 0;
                    columns:1;
                    property int itemWidth : width - 2*padding;
                    SpinBox{
                        width:parent.itemWidth;
                        from:0;to:15;
                        wrap:true;
                        value:interaction.getFrequency();
                        onValueModified: {
                            if(!interaction.changeSerialFrequency(value))
                                value:interaction.getFrequency();
                        }
                    }
                    ComboBox{
                        id:radioComboBox;
                        enabled: !control.radioConnect;
                        model:interaction.getSerialPortsList();
                        onActivated: interaction.changeSerialPort(currentIndex);
                        width:parent.itemWidth;
                        function updateModel(){
                            model = interaction.getSerialPortsList();
                            if(currentIndex >= 0)
                                interaction.changeSerialPort(currentIndex);
                        }
                        Component.onCompleted: updateModel();
                    }
                    Button{
                        width:parent.itemWidth;
                        icon.source:control.radioConnect ? "/source/connect.png" : "/source/disconnect.png";
                        onClicked: {
                            control.radioConnect = !control.radioConnect;
                            if(!interaction.connectSerialPort(control.radioConnect)){
                                control.radioConnect = !control.radioConnect;
                            }
                        }
                    }
                }
            }
            ZGroupBox{
                title: qsTr("Core")
                Grid{
                    width:parent.width;
                    verticalItemAlignment: Grid.AlignVCenter;
                    horizontalItemAlignment: Grid.AlignHCenter;
                    spacing: 0;
                    rowSpacing: 0;
                    columns:1;
                    property int itemWidth : (width - (columns-1) * columnSpacing - 2*padding)/columns;
                    ZButton{
                        icon.source:control.monitorConnect ? "/source/stop.png" : "/source/start.png";
                        onClicked: {
                            control.monitorConnect = !control.monitorConnect;
                            interaction.controlMonitor(control.monitorConnect)
                        }
                    }
                    Grid{
                        width:parent.width;
                        verticalItemAlignment: Grid.AlignVCenter;
                        horizontalItemAlignment: Grid.AlignHCenter;
                        spacing: 0;
                        rowSpacing: 5;
                        columns:2;
                        property int itemWidth : (width - (columns-1) * columnSpacing - 2*padding)/columns;
                        ZSwitch{
                            id:medusaSide;
                            width:parent.itemWidth * 2;
                            leftText:qsTr("Left");
                            rightText:qsTr("Right");
                            checked: false;
                            onCheckedChanged: {
                                interaction.changeMedusaSettings(false,medusaSide.checked)
                            }
                        }
                    }
                    // Grid{
                    //     width:parent.width;
                    //     verticalItemAlignment: Grid.AlignVCenter;
                    //     horizontalItemAlignment: Grid.AlignHCenter;
                    //     spacing: 0;
                    //     rowSpacing: 0;
                    //     columns:4;
                    //     property int itemWidth : (width - (columns-1) * columnSpacing - 2*padding)/columns;
                    //     CheckBox{
                    //         width:
                    //         text: "TEST"
                    //     }
                    //     ZButton{
                    //         text: "test button"
                    //     }
                    // }
                    Grid{
                        width:parent.width;
                        verticalItemAlignment: Grid.AlignVCenter;
                        horizontalItemAlignment: Grid.AlignHCenter;
                        spacing: 0;
                        rowSpacing: 0;
                        columns:2;
                        property int itemWidth : (width - (columns-1) * columnSpacing - 2*padding)/columns;
                        // SpinBox{
                        //     id:medusaFrq1;
                        //     width:parent.itemWidth;
                        //     from:0;to:15;
                        //     wrap:true;
                        //     value:8
                        // }
                        // SpinBox{
                        //     id:medusaFrq2;
                        //     width:parent.itemWidth;
                        //     from:0;to:15;
                        //     wrap:true;
                        //     value:6
                        // }
                        // ZComboBox{
                        //     enabled: !control.autoIMUBlue;
                        //     id:buleAdderss;
                        //     model:interaction.getAllAddress();
                        //     contentItem: Text {
                        //                   id:blueText
                        //                   text: interaction.getRealAddress(0)
                        //                   color: enabled ? "#ffffff" : "#888"
                        //                   font: buleAdderss.font
                        //                   elide: Text.ElideNone
                        //                   wrapMode: Text.WordWrap
                        //                   verticalAlignment: Text.AlignVCenter
                        //                   horizontalAlignment: Text.AlignHCenter
                        //               }
                        //     onActivated:{
                        //         interaction.changeAddress(0,currentIndex);
                        //         blueText.text = interaction.getRealAddress(0);
                        //     }
                        //     function updateModel(){
                        //         model = interaction.getAllAddress();
                        //         if(currentIndex >= 0){
                        //             interaction.changeAddress(0,currentIndex);
                        //             blueText.text = interaction.getRealAddress(0);
                        //         }
                        //     }
                        //     Component.onCompleted: {
                        //         interaction.getAllAddress();
                        //         blueText.text = interaction.getRealAddress(0);
                        //     }
                        // }
                        // ZComboBox{
                        //     enabled: !control.autoIMUYellow;
                        //     id:yellowAdderss;
                        //     model:interaction.getAllAddress();
                        //     contentItem: Text {
                        //                   id:yellowText
                        //                   text: interaction.getRealAddress(1)
                        //                   color: enabled ? "#ffffff" : "#888"
                        //                   font: yellowAdderss.font
                        //                   elide: Text.ElideNone
                        //                   wrapMode: Text.WordWrap
                        //                   verticalAlignment: Text.AlignVCenter
                        //                   horizontalAlignment: Text.AlignHCenter
                        //               }
                        //     onActivated:{
                        //         interaction.changeAddress(1,currentIndex);
                        //         yellowText.text = interaction.getRealAddress(1);
                        //     }
                        //     function updateModel(){
                        //         model = interaction.getAllAddress();
                        //         if(currentIndex >= 0){
                        //             interaction.changeAddress(1,currentIndex);
                        //             yellowText.text = interaction.getRealAddress(1);
                        //         }
                        //     }
                        //     Component.onCompleted: {
                        //         model = interaction.getAllAddress();
                        //         yellowText.text = interaction.getRealAddress(1);
                        //     }
                        // }
                        ZButton{
                            text: "<font color='#2976ca'>" + (medusaSide.checked ? "Right → Left" : "Left → Right") + "</font>";
                            icon.source:control.medusaConnect ? "/source/stop.png" : "/source/start.png";
                            icon.color: "#2976ca";
                            onClicked: {
                                var res = false;
                                control.medusaConnect = !control.medusaConnect;
                                interaction.changeMedusaSettings(false,medusaSide.checked)
                                res = interaction.changeTestSettings(false,test_script_mode_blue.checked,test_script_blue.currentIndex)
                                if (res == false){
                                    test_script_mode_blue.checked = false;
                                }
                                res = interaction.changeRefConfigSettings(false,refconfig_mode_blue.checked,refconfig_blue.currentIndex)
                                if (res == false){
                                    refconfig_mode_blue.checked = false;
                                }
                                if(!simulation.checked){
                                    interaction.connectSim(control.medusaConnect,0,false);
                                }else{
//                                    interaction.connectRadio(control.medusaConnect,0,medusaFrq1.value);
                                }
                                interaction.controlMedusa(control.medusaConnect)
                            }
                        }
                        ZButton{
                            text: "<font color='#ebdb7c'>" + (!medusaSide.checked ? "Right → Left" : "Left → Right") + "</font>";
                            icon.source:control.medusaConnect2 ? "/source/stop.png" : "/source/start.png";
                            icon.color: "#ebdb7c";
                            onClicked: {
                                var res = false;
                                control.medusaConnect2 = !control.medusaConnect2;
                                interaction.changeMedusaSettings(true,!medusaSide.checked)
                                res = interaction.changeTestSettings(true,test_script_mode_yellow.checked,test_script_yellow.currentIndex)
                                if (res == false){
                                    test_script_mode_yellow.checked = false;
                                }
                                res = interaction.changeRefConfigSettings(true,refconfig_mode_yellow.checked,refconfig_yellow.currentIndex)
                                if (res == false){
                                    refconfig_mode_yellow.checked = false;
                                }
                                if(!simulation.checked){
                                    interaction.connectSim(control.medusaConnect2,1,true);
                                }else{
//                                    interaction.connectRadio(control.medusaConnect2,1,medusaFrq2.value);
                                }
                                interaction.controlMedusa2(control.medusaConnect2)
                            }
                        }
                    }
                    Grid{
                        width:parent.width;
                        height:44;
                        verticalItemAlignment: Grid.AlignVCenter;
                        horizontalItemAlignment: Grid.AlignHCenter;
                        spacing: 0;
                        rowSpacing: 0;
                        rows:1;
                        // property int itemWidth : (width - (columns-1) * columnSpacing - 2*padding)/columns;
                        CheckBox{
                            id:test_script_mode_blue;
                            height:parent.height;
                            text: "TEST"
                        }
                        ZComboBox{
                            id:test_script_blue;
                            width:parent.width/2 - test_script_mode_blue.width;
                            height:parent.height;
                        }
                        CheckBox{
                            id:test_script_mode_yellow;
                            height:parent.height;
                            text: "TEST"
                        }
                        ZComboBox{
                            id:test_script_yellow;
                            width:parent.width/2 - test_script_mode_yellow.width - test_script_refresh.width;
                            height:parent.height;
                        }
                        Button{
                            id:test_script_refresh;
                            height:parent.height;
                            icon.source: "/source/refresh.png";
                            function trigger(){
                                interaction.updateTestScriptList();
                                test_script_blue.model = interaction.getTestScriptList();
                                test_script_yellow.model = interaction.getTestScriptList();
                            }
                            onClicked: {
                                trigger();
                            }
                        }
                        Component.onCompleted: {
                            test_script_refresh.trigger();
                            test_script_mode_blue.checked = interaction.getTestSettings(false);
                            test_script_mode_yellow.checked = interaction.getTestSettings(true);
                            test_script_blue.currentIndex = interaction.getTestScriptIndex(false);
                            test_script_yellow.currentIndex = interaction.getTestScriptIndex(true);
                        }
                    }
                    Grid{
                        width:parent.width;
                        height:44;
                        verticalItemAlignment: Grid.AlignVCenter;
                        horizontalItemAlignment: Grid.AlignHCenter;
                        spacing: 0;
                        rowSpacing: 0;
                        rows:1;
                        // property int itemWidth : (width - (columns-1) * columnSpacing - 2*padding)/columns;
                        CheckBox{
                            id:refconfig_mode_blue;
                            height:parent.height;
                            text: "USE "
                        }
                        ZComboBox{
                            id:refconfig_blue;
                            width:parent.width/2 - refconfig_mode_blue.width;
                            height:parent.height;
                        }
                        CheckBox{
                            id:refconfig_mode_yellow;
                            height:parent.height;
                            text: "USE "
                        }
                        ZComboBox{
                            id:refconfig_yellow;
                            width:parent.width/2 - refconfig_mode_yellow.width - refconfig_refresh.width;
                            height:parent.height;
                        }
                        Button{
                            id:refconfig_refresh;
                            height:parent.height;
                            icon.source: "/source/refresh.png";
                            function trigger(){
                                interaction.updateRefConfigList();
                                refconfig_blue.model = interaction.getRefConfigList();
                                refconfig_yellow.model = interaction.getRefConfigList();
                            }
                            onClicked: {
                                trigger();
                            }
                        }
                        Component.onCompleted: {
                            refconfig_refresh.trigger();
                            refconfig_mode_blue.checked = interaction.getRefConfigSetting(false);
                            refconfig_mode_yellow.checked = interaction.getRefConfigSetting(true);
                            refconfig_blue.currentIndex = interaction.getRefConfigIndex(false);
                            refconfig_yellow.currentIndex = interaction.getRefConfigIndex(true);
                        }
                    }
                }
            }

//            ZGroupBox{
//                title: qsTr("ZSimulator")
//                Grid{
//                    width:parent.width;
//                    verticalItemAlignment: Grid.AlignVCenter;
//                    horizontalItemAlignment: Grid.AlignHCenter;
//                    spacing: 0;
//                    rowSpacing: 5;
//                    columns:1;
//                    property int itemWidth : width - 2*padding;
//                    ZSwitch{
//                        id:simGui;
//                        width:parent.itemWidth;
//                        leftText:qsTr("Hide");
//                        rightText:qsTr("Show");
//                        checked: false;
//                    }
//                    ZButton{
//                        icon.source:control.simConnect ? "/source/stop.png" : "/source/start.png";
//                        onClicked: {
//                            control.simConnect = !control.simConnect;
//                            interaction.controlSim(control.simConnect,simGui.checked);
//                        }
//                    }
//                }
//            }
            ZGroupBox{
                title: qsTr("Kill")
                Grid{
                    width:parent.width;
                    verticalItemAlignment: Grid.AlignVCenter;
                    horizontalItemAlignment: Grid.AlignHCenter;
                    spacing: 0;
                    rowSpacing: 5;
                    columns:1;
                    property int itemWidth : width - 2*padding;
                    ZButton {
                        icon.source:"/source/stop.png";
                        onClicked: {
                            interaction.kill();
                        }
                    }
                }
            }
            ZSS.Display{
                type:1;
                width:parent.width - 2*parent.padding;
                height:300;
                onWidthChanged: {
                    resetSize(width,height);
                }
                onHeightChanged: {
                    resetSize(width,height);
                }
            }
        }
        RefereeBox{
        }
//        Viewer{}
        /*****************************************/
        /*                  UDP                  */
        /*****************************************/
       Grid {
           id: radio;
           width: parent.itemWidth;
           padding:10;
           verticalItemAlignment: Grid.AlignVCenter;
           horizontalItemAlignment: Grid.AlignHCenter;
           spacing: 5;
           columns:1;
           property int itemWidth : width - 2*padding;
        //    ZGroupBox{
        //        title: qsTr("Crazy")
        //        Grid{
        //            width:parent.width;
        //            verticalItemAlignment: Grid.AlignVCenter;
        //            horizontalItemAlignment: Grid.AlignHCenter;
        //            spacing: 0;
        //            rowSpacing: 5;
        //            columns:1;
        //            property int itemWidth : width - 2*padding;
        //            Button{
        //                width:parent.itemWidth;
        //                icon.source:control.crazyConnect ? "/source/stop.png" : "/source/start.png";
        //                onClicked: {
        //                    control.crazyConnect = !control.crazyConnect;
        //                    interaction.controlCrazy(control.crazyConnect)
        //                }
        //            }
        //        }
        //    }
        //    ZGroupBox{
        //        title: qsTr("Camera Edge Test")
        //        Grid{
        //            width:parent.width;
        //            verticalItemAlignment: Grid.AlignVCenter;
        //            horizontalItemAlignment: Grid.AlignHCenter;
        //            spacing: 0;
        //            rowSpacing: 5;
        //            columns:1;
        //            property int itemWidth : width - 2*padding;
        //            Button{
        //                width:parent.itemWidth;
        //                icon.source:control.ifEdgeTest ? "/source/stop.png" : "/source/start.png";
        //                onClicked: {
        //                    control.ifEdgeTest = !control.ifEdgeTest;
        //                    interaction.setIfEdgeTest(control.ifEdgeTest);
        //                }
        //            }
        //        }
        //    }
           ZGroupBox{
               title: qsTr("Rec")
               Grid{
                   width:parent.width;
                   property int itemWidth : width - 2*padding;
                   Button{
                       width:parent.itemWidth;
                       icon.source: isRecoring ?  "/source/stop.png" : "/source/start.png";
                       onClicked: {
                           control.isRecoring = !control.isRecoring;
                           interaction4field.setRecorder(control.isRecoring);
                       }
                   }
               }
           }
           ZGroupBox{
               title: qsTr("RecReplay")
               Column{
                   width:parent.width;
                   height:parent.height;
                   spacing: 0;
                   padding:0;
                   Grid{
                       width:parent.width;
                       verticalItemAlignment: Grid.AlignVCenter;
                       horizontalItemAlignment: Grid.AlignHCenter;
                       spacing: 5;
                       rowSpacing: 5;
                       columns: 7;
                       ZButton {
                               id:openBtnr
                               width: 35;
                               icon.source:"/source/openfile.png";
                               anchors.leftMargin: 10
                               onClicked: {
                                   fdrs.open();
                                   interaction4field.setRecorder(false);
                               }
                           }
                           Label {
                               id: labelsr
                               text: qsTr("")
                               height: 25
                               anchors.left:openBtnr.right
                               anchors.leftMargin: 10
                           }

                           FileDialog {
                               id:fdrs
                               title: "Please select"
                               selectExisting: true
                               selectFolder: false
                               selectMultiple: false
                               nameFilters: ["Rec files (*.log)"]
                               onAccepted: {
                                   if (control.isReplaying) ZSS.RecSlider.toggleStopped();
                                   control.isReplaying = false;
                                   rectimer.running = false;
                                   console.log("You chose: " + fdrs.fileUrl);
                                   ZSS.RecSlider.loadFile(fdrs.fileUrl);
                                   recslider.to = ZSS.RecSlider.maximumValue;
                                   recslider.stepSize = ZSS.RecSlider.stepSize;
                                   recslider.value = 0;
                                   control.isRecFile = true;
                                   control.isRecoring = false;
                               }
                               onRejected: {
                                   labelsr.text = "";
                                   console.log("Canceled");
                               }
                           }
                       ZButton{
                           id: recPlayerPlay;
                           width: 35;
                           enabled: control.isRecFile;
                           icon.source:control.isReplaying ? "/source/pause.png" : "/source/start.png";
                           onClicked: {
                               control.isReplaying  = !control.isReplaying;
                               rectimer.running = control.isReplaying;
                               ZSS.RecSlider.toggleStopped();
                           }
                       }
                       ZButton{
                           width: 35;
                           icon.source:"/source/stop.png";
                           enabled: control.isRecFile;
                           onClicked: {
                               control.isReplaying = false;
                               rectimer.running = false;
                               control.isRecFile = false;
                               ZSS.RecSlider.setStopped(true);
                               recslider.value = 0;
                           }
                       }
                       ZButton{
                           id: recPlayerForward
                           width: 35;
                           enabled: control.isRecFile /*&& !control.isReplaying*/;
                           icon.source:"/source/last.png";
                           onClicked: {
                               recslider.value = recslider.value - recPlaySpinBox.value * recslider.stepSize;
                           }
                       }
                       ZButton{
                           id: recPlayerBack
                           width: 35;
                           enabled: control.isRecFile /*&& !control.isReplaying*/;
                           icon.source:"/source/next.png";
                           onClicked: {
                               recslider.value = recslider.value + recPlaySpinBox.value * recslider.stepSize;
                           }
                       }
                       Text{
                           text: "Steps:";
                           color: "white";
                       }
//                        TextField {
//                            id:input;
//                            height: 46
//                            width:36
//                            text: control.playspeed
//                            color:"white";
//                            font.pixelSize: 16;
//                            font.family:"Arial";
//                            leftPadding: 12;
////                            verticalAlignment: Text.AlignVCenter
//                            selectByMouse: true;
//                            onAccepted:{
//                                control.playspeed = text;
//                                focus = false;
//                                width= text.length*8 + 24
//                                console.log(text.length)
//                            }
//                        }
                       SpinBox {
                           id:recPlaySpinBox;
                           height: 46
                           width: 120
                           from: 1
                           to:10000
                           font.pixelSize: 16
                       }
                   }
                   Grid{
                       width:parent.width;
                       verticalItemAlignment: Grid.AlignVCenter;
                       horizontalItemAlignment: Grid.AlignHCenter;
                       spacing: 5;
                       rowSpacing: 5;
                       columns: 4;

                       Slider{
                           id: recslider
                           width: parent.width - timeTextr.width;
//                            value: ZSS.RecSlider.currentFrame;
                           onValueChanged: {
                               if (recslider.value != ZSS.RecSlider.currentFrame)
                                   ZSS.RecSlider.seekFrame(recslider.value);
                           }

//                            MouseArea {
//                                property int pos
//                                anchors.fill: parent
//                                onClicked: {
//                                    pos = ZSS.RecSlider.maximumValue * mouse.x/parent.width;
//                                    recslider.value = pos;
//                                }
//                                onPressAndHold:  {
//                                    pos = ZSS.RecSlider.maximumValue * mouse.x/parent.width;
//                                    recslider.value = pos;
//                                }
//                            }
                       }
                       Text{
                           id: timeTextr;
                           text: ZSS.RecSlider.durTime + " / " + ZSS.RecSlider.maxTime;
                           color: "white";
                       }
                       Timer{
                           id: rectimer;
                           interval: 1;
                           repeat: true;
                           running: false;
                           onTriggered: {
                               recslider.value = ZSS.RecSlider.currentFrame;
                           }
                       }
                   }
                   Grid{
                       width:parent.width;
                       verticalItemAlignment: Grid.AlignVCenter;
                       horizontalItemAlignment: Grid.AlignHCenter;
                       spacing: 5;
                       rowSpacing: 5;
                       columns: 6;
                       Text{
                           id:recname
                           width: parent.width - 180;
                           text: ZSS.RecSlider.recName;
                           color: "white";
                       }
                   }
               }
           }

        //    ZGroupBox{
        //        title: qsTr("Log")
        //        Column{
        //            width:parent.width;
        //            height:parent.height;
        //            spacing: 0;
        //            padding:0;
        //            Grid{
        //                width:parent.width;
        //                verticalItemAlignment: Grid.AlignVCenter;
        //                horizontalItemAlignment: Grid.AlignHCenter;
        //                spacing: 5;
        //                rowSpacing: 5;
        //                columns: 6;
        //                ZButton {
        //                        id:openBtn
        //                        width: 35;
        //                        icon.source:"/source/openfile.png";
        //                        anchors.leftMargin: 10
        //                        onClicked: {
        //                            fds.open();
        //                        }
        //                    }
        //                    Label {
        //                        id: labels
        //                        text: qsTr("")
        //                        height: 25
        //                        anchors.left:openBtn.right
        //                        anchors.leftMargin: 10
        //                    }

        //                    FileDialog {
        //                        id:fds
        //                        title: "Please select"
        //                        folder: shortcuts.desktop
        //                        selectExisting: true
        //                        selectFolder: false
        //                        selectMultiple: false
        //                        nameFilters: ["Log files (*.log *.log.gz)"]
        //                        onAccepted: {
        //                            console.log("You chose: " + fds.fileUrl + " "+fds.folder);
        //                            log.loadFile(fds.fileUrl);
        //                            logslider.to = log.maximumValue;
        //                            logslider.stepSize = log.stepSize;
        //                            logslider.value = 0;
        //                        }
        //                        onRejected: {
        //                            labels.text = "";
        //                            console.log("Canceled");
        //                        }
        //                    }
        //                ZButton{
        //                    width: 35;
        //                    icon.source:control.isLogPlay ? "/source/pause.png" : "/source/start.png";
        //                    onClicked: {
        //                        control.isLogPlay = !control.isLogPlay;
        //                        logtimer.running = !logtimer.running;
        //                        log.toggleStopped();
        //                    }
        //                }
        //                ZButton{
        //                    width: 35;
        //                    icon.source:"/source/last.png";
        //                    onClicked: {
        //                        logslider.value = logslider.value - 3 * logslider.stepSize;
        //                    }
        //                }
        //                ZButton{
        //                    width: 35;
        //                    icon.source:"/source/next.png";
        //                    onClicked: {
        //                        logslider.value = logslider.value + 3 * logslider.stepSize;
        //                    }
        //                }
        //                ZButton{
        //                    width: 35;
        //                    icon.source:control.isCutting ? "/source/scissors1.png" : "/source/scissors2.png";
        //                    onClicked: {
        //                        if (control.isCutting) {
        //                            log.setStartPoint(fds.fileUrl, logslider.value);
        //                        } else {
        //                            log.setStopPoint(fds.fileUrl, logslider.value);
        //                        }
        //                        control.isCutting = !control.isCutting;
        //                    }
        //                }
        //            }
        //            Grid{
        //                width:parent.width;
        //                verticalItemAlignment: Grid.AlignVCenter;
        //                horizontalItemAlignment: Grid.AlignHCenter;
        //                spacing: 5;
        //                rowSpacing: 5;
        //                columns: 4;

        //                Slider{
        //                    id: logslider
        //                    width: parent.width - timeText.width;
        //                    onValueChanged: log.seekFrame(logslider.value);
        //                }
        //                Text{
        //                    id: timeText;
        //                    text: log.durTime + " / " + log.maxTime;
        //                    color: "white";
        //                }
        //                Timer{
        //                    id: logtimer;
        //                    repeat: true;
        //                    running: false;
        //                    onTriggered: {
        //                        logslider.value = logslider.value + logslider.stepSize;
        //                    }
        //                }
        //            }
        //            Grid{
        //                width:parent.width;
        //                verticalItemAlignment: Grid.AlignVCenter;
        //                horizontalItemAlignment: Grid.AlignHCenter;
        //                spacing: 5;
        //                rowSpacing: 5;
        //                columns: 6;
        //                Text{
        //                    id:logname
        //                    width: parent.width - 180;
        //                    text: log.logName;
        //                    color: "white";
        //                }
        //            }
        //        }
        //    }
//            ZGroupBox{
//                title: qsTr("Techinal Challenge2")
//                Column{
//                    width:parent.width;
//                    height:parent.height;
//                    spacing: 0;
//                    padding:0;
//                    Grid{
//                        width:parent.width;
//                        verticalItemAlignment: Grid.AlignVCenter;
//                        horizontalItemAlignment: Grid.AlignHCenter;
//                        spacing: 5;
//                        rowSpacing: 5;
//                        columns: 6;
//                        ZButton {
//                                id:openLabelBtn
//                                width: 35;
//                                icon.source:"/source/openfile.png";
//                                anchors.leftMargin: 10
//                                onClicked: {
//                                    labelfilestart.open();
//                                }
//                            }
// //                            Label {
// //                                id: labelstart
// //                                text: qsTr("")
// //                                height: 25
// //                                anchors.left:openBtn.right
// //                                anchors.leftMargin: 10
// //                            }

//                            FileDialog {
//                                id:labelfilestart
//                                title: "Please select"
//                                folder: shortcuts.desktop
//                                selectExisting: true
//                                selectFolder: false
//                                selectMultiple: false
//                                nameFilters: ["SSL Label files (*.labeler)"]
//                                onAccepted: {
//                                    console.log("You chose: " + labelfilestart.fileUrl + " "+labelfilestart.folder);
//                                    eventlabel.loadFile(labelfilestart.fileUrl);
//                                    logslider.to = log.maximumValue;
//                                    logslider.stepSize = log.stepSize;
//                                    logslider.value = 0;
//                                }
//                                onRejected: {
//                                    labels.text = "";
//                                    console.log("Canceled");
//                                }
//                            }
//                        ZButton{
//                            width: 35;
//                            icon.source:control.isLogPlay ? "/source/pause.png" : "/source/start.png";
//                            onClicked: {
//                                control.isLogPlay = !control.isLogPlay;
//                                logtimer.running = !logtimer.running;
//                                eventlabel.processLabel();
//                            }
//                        }
//                    }
//                    Grid{
//                        width:parent.width;
//                        verticalItemAlignment: Grid.AlignVCenter;
//                        horizontalItemAlignment: Grid.AlignHCenter;
//                        spacing: 5;
//                        rowSpacing: 5;
//                        columns: 6;
//                        Text{
//                            id:labelname
//                            width: parent.width - 180;
//                            text: eventlabel.logName;
//                            color: "white";
//                        }
//                    }
//                }
//            }
       }

        Settings{
        }

    }
    Shortcut{
        property int index : 0;
        sequence:shortCutString[index];
        onActivated: {
            control.robotControl(index)
        }
    }
    Shortcut{
        property int index : 1;
        sequence:shortCutString[index];
        onActivated: {
            control.robotControl(index)
        }
    }
    Shortcut{
        property int index : 2;
        sequence:shortCutString[index];
        onActivated: {
            control.robotControl(index)
        }
    }
    Shortcut{
        property int index : 3;
        sequence:shortCutString[index];
        onActivated: {
            control.robotControl(index)
        }
    }
    Shortcut{
        property int index : 4;
        sequence:shortCutString[index];
        onActivated: {
            control.robotControl(index)
        }
    }
    Shortcut{
        property int index : 5;
        sequence:shortCutString[index];
        onActivated: {
            control.robotControl(index)
        }
    }
    Shortcut{
        property int index : 6;
        sequence:shortCutString[index];
        onActivated: {
            control.robotControl(index)
        }
    }
    Shortcut{
        property int index : 7;
        sequence:shortCutString[index];
        onActivated: {
            control.robotControl(index)
        }
    }
    Shortcut{
        property int index : 8;
        sequence:shortCutString[index];
        onActivated: {
            control.robotControl(index)
        }
    }
    Shortcut{
        property int index : 9;
        sequence:shortCutString[index];
        onActivated: {
            control.robotControl(index)
        }
    }
    Shortcut{
        property int index : 10;
        sequence:shortCutString[index];
        onActivated: {
            control.robotControl(index)
        }
    }
    Shortcut{
        property int index : 11;
        sequence:shortCutString[index];
        onActivated: {
            control.robotControl(index)
        }
    }
    Shortcut{
        property int index : 12;
        sequence:shortCutString[index];
        onActivated: {
            control.robotControl(index)
        }
    }
    Shortcut{
        property int index : 13;
        sequence:shortCutString[index];
        onActivated: {
            control.robotControl(index)
        }
    }
    Shortcut{
        property int index : 14;
        sequence:shortCutString[index];
        onActivated: {
            control.robotControl(index)
        }
    }
    Shortcut{
        property int index : 15;
        sequence:shortCutString[index];
        onActivated: {
            control.robotControl(index)
        }
    }
    Shortcut{
        property int index : 16;
        sequence:shortCutString[index];
        onActivated: {
            control.robotControl(index)
        }
    }
    Shortcut{
        property int index : 17;
        sequence:shortCutString[index];
        onActivated: {
            control.robotControl(index)
        }
    }
    Shortcut{
        property int index : 18;
        sequence:shortCutString[index];
        onActivated: {
            control.robotControl(index)
        }
    }
    Shortcut{
        property int index : 19;
        sequence:shortCutString[index];
        onActivated: {
            control.robotControl(index)
        }
    }
    Shortcut{
        property int index : 20;
        sequence:shortCutString[index];
        onActivated: {
            control.robotControl(index)
        }
    }
    Shortcut{
        property int index : 21;
        sequence:shortCutString[index];
        onActivated: {
            control.robotControl(index)
        }
    }
    Shortcut{
        property int index : 22;
        sequence:shortCutString[index];
        onActivated: {
            control.robotControl(index)
        }
    }
    Shortcut{
        property int index : 23;
        sequence:shortCutString[index];
        onActivated: {
            control.robotControl(index)
        }
    }
    Shortcut{
        property int index : 24;
        sequence:shortCutString[index];
        onActivated: {
            control.robotControl(index)
        }
    }
    Shortcut{
        property int index : 25;
        sequence:shortCutString[index];
        onActivated: {
            control.robotControl(index)
        }
    }
    Shortcut{
        property int index : 26;
        sequence:shortCutString[index];
        onActivated: {
            control.robotControl(index)
        }
    }
    Shortcut{
        property int index : 27;
        sequence:shortCutString[index];
        onActivated: {
            control.robotControl(index)
        }
    }
    Shortcut{
        property int index : 28;
        sequence:shortCutString[index];
        onActivated: {
            control.robotControl(index)
        }
    }
    Shortcut{
        property int index : 29;
        sequence:shortCutString[index];
        onActivated: {
            control.robotControl(index)
        }
    }
    Shortcut{
        property int index : 30;
        sequence:shortCutString[index];
        onActivated: {
            control.robotControl(index)
        }
    }
    Shortcut{
        property int index : 31;
        sequence:shortCutString[index];
        onActivated: {
            control.robotControl(index)
        }
    }
    Shortcut{
        property int index : 32;
        sequence:shortCutString[index];
        onActivated: {
            control.robotControl(index)
        }
    }
    Shortcut{
        property int index : 33;
        sequence:shortCutString[index];
        onActivated: {
            control.robotControl(index)
        }
    }
    Shortcut{
        sequence: recplayerShortCut[0];
        onActivated: {
            recPlayerPlay.clicked()
        }
    }
    Shortcut{
        sequence: recplayerShortCut[1];
        onActivated: {
            recPlayerForward.clicked()
        }
    }
    Shortcut{
        sequence: recplayerShortCut[2];
        onActivated: {
            recPlayerBack.clicked()
        }
    }
    Shortcut{
        sequence: recplayerShortCut[3];
        onActivated: {
            recPlaySpinBox.value = recPlaySpinBox.value + 1;
        }
    }
    Shortcut{
        sequence: recplayerShortCut[4];
        onActivated: {
            recPlaySpinBox.value = recPlaySpinBox.value - 1;
        }
    }
}
