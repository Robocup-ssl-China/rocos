/****************************************************************************
**
** Copyright (C) 2017 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the examples of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:BSD$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** BSD License Usage
** Alternatively, you may use this file under the terms of the BSD license
** as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of The Qt Company Ltd nor the names of its
**     contributors may be used to endorse or promote products derived
**     from this software without specific prior written permission.
**
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
**
** $QT_END_LICENSE$
**
****************************************************************************/

pragma Singleton
import QtQuick 2.12
import QtQuick.Controls.Universal 2.12
QtObject {
    readonly property bool use_dark : true
    readonly property color c_black:         Qt.hsla(0.000, 0.00, 0.04, 1.00)
    readonly property color c_black_bis:     Qt.hsla(0.000, 0.00, 0.12, 1.00)
    readonly property color c_black_ter:     Qt.hsla(0.000, 0.00, 0.16, 1.00)
    readonly property color c_grey_darker:   Qt.hsla(0.000, 0.00, 0.21, 1.00)
    readonly property color c_grey_dark:     Qt.hsla(0.000, 0.00, 0.29, 1.00)
    readonly property color c_grey:          Qt.hsla(0.000, 0.00, 0.40, 1.00)
    readonly property color c_grey_light:    Qt.hsla(0.000, 0.00, 0.71, 1.00)
    readonly property color c_grey_lighter:  Qt.hsla(0.000, 0.00, 0.86, 1.00)
    readonly property color c_grey_lightest: Qt.hsla(0.000, 0.00, 0.93, 1.00)
    readonly property color c_white_ter:     Qt.hsla(0.000, 0.00, 0.96, 1.00)
    readonly property color c_white_bis:     Qt.hsla(0.000, 0.00, 0.98, 1.00)
    readonly property color c_white:         Qt.hsla(0.000, 0.00, 1.00, 1.00)
    readonly property color c_orange:        Qt.hsla(0.039, 0.80, 0.53, 1.00)
    readonly property color c_yellow:        Qt.hsla(0.122, 0.80, 0.73, 1.00)
    readonly property color c_green:         Qt.hsla(0.425, 0.53, 0.53, 1.00)
    readonly property color c_turquoise:     Qt.hsla(0.475, 0.80, 0.41, 1.00)
    readonly property color c_cyan:          Qt.hsla(0.575, 0.61, 0.53, 1.00)
    readonly property color c_blue:          Qt.hsla(0.636, 0.53, 0.53, 1.00)
    readonly property color c_purple:        Qt.hsla(0.753, 0.80, 0.71, 1.00)
    readonly property color c_red:           Qt.hsla(0.967, 0.86, 0.61, 1.00)

    readonly property color c_primary: c_turquoise
    readonly property color c_info: c_cyan
    readonly property color c_link: c_blue
    readonly property color c_success: c_green
    readonly property color c_warning: c_yellow
    readonly property color c_danger: c_red
    readonly property color c_background: use_dark ? c_black_ter : c_grey_lighter

    readonly property color background: c_background
    readonly property color foreground: "#fff"
    readonly property color accent: "orange"//"#00C39C"
    readonly property color background_light:Qt.lighter(background,1.2)
    readonly property color background_lighter:Qt.lighter(background,1.5)
    readonly property color background_dark:Qt.darker(background,1.2)
    readonly property color background_darker:Qt.darker(background,1.5)

    readonly property int i_main:0
    readonly property int i_main_font:1
    readonly property int i_hover:2
    readonly property int i_hover_font:3
    readonly property int i_active:4
    readonly property int i_active_font:5
    readonly property int i_disable:6
    readonly property int i_disable_font:7

    function colorLuminance(c){
        function h(v){
            if(v<0.03928)
                return v/12.92
            else
                return Math.pow((v+0.055)/1.055,2)
        }
        return h(c.r)*0.2126+h(c.g)*0.7152+h(c.b)*0.0722
    }
    function findColorInvert(c){
        if(colorLuminance(c)>0.45)
            return Qt.rgba(0,0,0,0.7)
        return Qt.rgba(1,1,1,0.8)
    }
    function findLightColor(c,ratio=1.1){
        return Qt.lighter(c,ratio)
    }
    function findDarkColor(c,ratio=1.1){
        return Qt.darker(c,ratio)
    }
    function findAlphaColor(c,ratio=1){
        return Qt.rgba(c.r,c.g,c.b,c.a*ratio);
    }
    function getAllColors(c){
        return [c,findColorInvert(c),
                findLightColor(c),findColorInvert(findLightColor(c)),
                findDarkColor(c),findColorInvert(findDarkColor(c)),
                findAlphaColor(c,0.5),findAlphaColor(findColorInvert(c),0.6)]
    }
    readonly property variant __c: {
        "main"    : getAllColors(background),
        "main2"   : getAllColors(background_lighter),
        "darker"  : getAllColors(background_dark),
        "accent"  : getAllColors(accent),
        "primary" : getAllColors(c_primary),
        "link"    : getAllColors(c_link),
        "info"    : getAllColors(c_info),
        "success" : getAllColors(c_success),
        "warning" : getAllColors(c_warning),
        "danger"  : getAllColors(c_danger),
        "transparent" : getAllColors(Qt.hsla(0.000, 0.00, 0.00, 0.00))
    }

    property int baseSize: 10
    readonly property int smallSize: 12
    readonly property int normalSize: 15
    readonly property int mediumSize: 20
    readonly property int largeSize: 24

    property font font
//    Component.onCompleted: {
//        console.log(__c["info"][i_main],__c["warning"][i_main]);
//    }
}
