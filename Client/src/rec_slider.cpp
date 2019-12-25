#include "rec_slider.h"
#include "field.h"

namespace  {
int fps = 1000/75;


bool _stopped = true;
}
rec_slider::rec_slider(QObject* parent)
    : QObject(parent) {
    connect(&_player, SIGNAL(positionChanged(int)), SLOT(updateTime(int)));
}

void rec_slider::loadFile(QString filename) {
    currentFrame = 0;
#ifdef WIN32
    filename.remove(QString("file:///"));
#else
    filename.remove(QString("file://"));
#endif

    if (_player.loadRec(filename, _maxFrame)) {

        setmaxTime(QString("%1:%2.%3")
                   .arg((int) ((_maxFrame-1) * fps / 1E3) / 60)
                   .arg((int) ((_maxFrame-1) * fps / 1E3) % 60, 2, 10, QChar('0'))
                   .arg((int) ((_maxFrame-1) * fps) % 1000, 3, 10, QChar('0')));
        setmaximumValue(_maxFrame-1);
        setstepSize(1);

        setrecName(filename);
    }
}

void rec_slider::toggleStopped() {
    setStopped(!_stopped);
//    qDebug() << "toggleStopped worked" << endl;
}

void rec_slider::setStopped(bool p) {
//    _stopped = p | !(currentFrame < _maxFrame);
////    _stopped = p;
////    qDebug() << "_stopped is " << _stopped;
//    Field::pauseRecPlayer(_stopped, currentFrame);
    _stopped = p | !_player.good();

    if (_stopped) {
        _player.stop();
    } else {
        _player.start(currentFrame);
    }
}

void rec_slider::seekFrame(int value) {
//    currentFrame = value;
//    Field::pauseRecPlayer(_stopped, currentFrame);
    currentFrame = value;

    if (!_stopped) {
        _player.stop();
        _player.start(currentFrame);
    } else {
        _player.slowly(value);
    }
}

void rec_slider::updateTime(int frame) {
    currentFrame = frame;
    setdurTime(QString("%1:%2.%3")
               .arg((int) (currentFrame* fps / 1E3) / 60)
               .arg((int) (currentFrame* fps / 1E3) % 60, 2, 10, QChar('0'))
               .arg((int) (currentFrame* fps) % 1000, 3, 10, QChar('0')));
}
