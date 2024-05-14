#ifndef BALLSTATE_H
#define BALLSTATE_H

enum Result {fail, success, undefined};
enum State {_unknown, _touched, _shoot, _struggle, _pass, _dribble, _stop, _outoffied, _control, _undefine, _kicked, _chip_pass, _flat_pass};

class Ballstate {
  public:
    Ballstate();
    State ballState = State::_unknown;
};

#endif // BALLSTATE_H
