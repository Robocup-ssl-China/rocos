TEMPLATE = app

CONFIG += c++11

QT += qml quick serialport

TARGET = CrazyNaN

RESOURCES += qml.qrc

LIBS +=\
-lprotobuf

HEADERS += \
    $$PWD/src/radiopacket.h \
    $$PWD/src/serialobject.h \
    $$PWD/src/singleton.hpp \
    $$PWD/src/lib/crc/crc.h \
    $$PWD/src/translation.hpp \
    $$PWD/src/settings.hpp \
    $$PWD/src/udp24l01object.h \
    $$PWD/src/lib/udp24l01object.h \
    $$PWD/src/lib/share/cpp/zss_cmd.pb.h \
    $$PWD/src/lib/share/cpp/zss_cmd_type.pb.h \
    $$PWD/src/udpwifiobject.h

SOURCES += \
    $$PWD/src/main.cpp \
    $$PWD/src/radiopacket.cpp \
    $$PWD/src/serialobject.cpp \
    $$PWD/src/lib/crc/crc.cpp \
    $$PWD/src/udp24l01object.cpp \
    $$PWD/src/lib/share/cpp/zss_cmd.pb.cc \
    $$PWD/src/lib/share/cpp/zss_cmd_type.pb.cc \
    $$PWD/src/udpwifiobject.cpp

TRANSLATIONS = t1_zh.ts

defineTest(copyToDestdir) {
    files = $$1

    for(FILE, files) {
        macx {
            DDIR = $${DESTDIR}/$${TARGET}.app/Contents/MacOS
        }
        win32 {
            DDIR = $$DESTDIR
        }

        # Replace slashes in paths with backslashes for Windows
        win32:FILE ~= s,/,\\,g
        win32:DDIR ~= s,/,\\,g

        QMAKE_POST_LINK += $$QMAKE_COPY $$quote($$FILE) $$quote($$DDIR) $$escape_expand(\\n\\t)
    }

    export(QMAKE_POST_LINK)
}

win32 {
    RC_ICONS = logo.ico
    system(lrelease t1_zh.ts)
    copyToDestdir($$PWD/t1_zh.qm)
}

macx {
    system(lrelease t1_zh.ts)
    copyToDestdir($$PWD/t1_zh.qm)
}

DISTFILES += \
    src/lib/share/auto.sh \
    src/lib/share/zss_cmd.proto \
    src/lib/share/zss_cmd_type.proto
