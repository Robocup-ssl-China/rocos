#ifndef _OPTION_MODULE_H_
#define _OPTION_MODULE_H_
/**
* COptionModule.
* 一些初始化参数
*/
class COptionModule{
public:
    COptionModule();
	~COptionModule();
	int MySide() const { return _side; }
	int MyNumber() const { return _number; }
	int MyType() const { return _type; }
	int MyColor() const { return _color; }
protected:
	void ReadOption(int argc, char* argv[]);
private:
	int _side; // 比赛中所在的边
	int _number; // 机器人号码，启动一个队员的时候用到
	int _type; // 机器人类型
	int _color; // 我方颜色,RefereeBox用到
};
#endif
