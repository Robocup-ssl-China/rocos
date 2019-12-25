///// FileName : 		CommControl.h
///// 				declaration file
///// Description :	It supports command send interface for ZJUNlict,
/////	Keywords :		fitting, send, interface
///// Organization : 	ZJUNlict@Small Size League
///// Author : 		cliffyin
///// E-mail : 		cliffyin@zju.edu.cn
/////					cliffyin007@gmail.com
///// Create Date : 	2011-07-25
///// Modified Date :	2011-07-25
///// History :

//#ifndef __COMM_CONTROL_H__
//#define __COMM_CONTROL_H__

//#include <MultiThread.h>
//#include "./RobotCommand.h"
//#include "staticparams.h"

///// <summary>	Robot control.  </summary>
/////
///// <remarks>	ZjuNlict, 2011-7-26. </remarks>

//class CRobotControl
//{
//public:
//	CRobotControl(void)
//	{
//		nSpeed1	= 0;
//		nSpeed2	= 0;
//		nSpeed3	= 0;
//		nSpeed4	= 0;
//		nKick	= 0;
//		nControlBall = 0;
//		fX = 0;
//		fY = 0;
//		posX = 0;
//		posY = 0;
//		posW = 0;
//		fRotate	= 0;
//		gyro = false;
//		stop=false;
//		angle = 0;
//		radius = 0;
//	}

//    ~CRobotControl(void){}

//	// 车号
//	int nRobotNum;

//	// 工作模式
//	int workingMode;

//	// send信息: 发下去的四个轮子的速度
//	// 上层指令进行速度分解后的浮点值
//	float fSpeed1,fSpeed2,fSpeed3,fSpeed4;
//	// 发送到底层时必须转化成整型值
//	short nSpeed1,nSpeed2,nSpeed3,nSpeed4;

//	// send信息:整车的指定x方向,y方向速度,以及转速
//	float fX, fY, fRotate;

//	// send信息：小车的目标位置和方向
//	float posX, posY, posW;

//	// send信息:射门力量
//	unsigned int nKick;

//	// send信息:控球度
//	char nControlBall;

//	// send信息：陀螺仪
//	bool gyro;
//	float angle;
//	unsigned int radius;
//	bool stop;

//	// send: PID Param
//	unsigned int prop[4];
//	unsigned int intg[4];
//	unsigned int diff[4];

//	// receive信息: 收上来的四个轮子速度
//	// 双向通讯收到的只能是整型数据
//	short nCurSpeed1,nCurSpeed2,nCurSpeed3,nCurSpeed4;
//	// 上层取用的浮点数据
//	float fCurSpeed1,fCurSpeed2,fCurSpeed3,fCurSpeed4;

//	// receive信息: 红外检测信号
//	bool bInfraredInfo;
	
//	// receive信息: 吸球情况
//	bool bControledInfo;

//	// receive信息：上传改变次数
//	int changeNum;

//	// receive信息：上传本次改变已发了几帧
//	int changeCountNum;

//	//receive信息：电池电压
//	int battery;

//	//receive信息：电容电池
//	int capacity;

//	//receive信息: x,y方向加速度,角速度，角度
//	float accX, accY, vW, angel;

//	//receive信息: 1,2,3,4号占空比
//	float duty1, duty2, duty3, duty4;

//	// receive信息: 击球检测（预留，尚未用到）
//	short kickInfo; // 0-没击球, 1-平射, 2-挑射

//	unsigned char kickId, kickCount;
	

//	void fton()
//	{
//		nSpeed1=(int)fSpeed1;
//		nSpeed2=(int)fSpeed2;
//		nSpeed3=(int)fSpeed3;
//		nSpeed4=(int)fSpeed4;
//    }

//	void ntof()
//	{
//		fCurSpeed1=(float)nCurSpeed1;
//		fCurSpeed2=(float)nCurSpeed2;
//		fCurSpeed3=(float)nCurSpeed3;
//		fCurSpeed4=(float)nCurSpeed4;
//    }
//};

///// <summary>	Robotinfo.  </summary>
///// 用于上层向下层索要小车状态数据时的结构信息
///// <remarks>	ZjuNlict, 2011-7-26. </remarks>

//struct ROBOTINFO
//{
//	int nRobotNum;

//	float fCurSpeed1;
//	float fCurSpeed2;
//	float fCurSpeed3;
//	float fCurSpeed4;

//	bool bInfraredInfo;
//	bool bControledInfo;
//	short nKickInfo;
//	unsigned char kickId;
//	unsigned char kickCount;
//	int changeNum;
//	int changeCountNum;
//	int battery;
//	int capacity;
//};

///// <summary>	Communications control, abstract interface.  </summary>
/////
///// <remarks>	Zju Nlict, 2011-7-26. </remarks>
//class AbstractRadioPackets;

///// <summary>	Communications control, main module.  </summary>
/////
///// <remarks>	ZjuNlict, 2011-7-26. </remarks>

//class CCommControl
//{
//public:

//	/// <summary>	Default constructor. </summary>
//	///
//	/// <remarks>	ZjuNlict, 2011-7-26. </remarks>

//	CCommControl(void);

//	/// <summary>	Finaliser. </summary>
//	///
//	/// <remarks>	ZjuNlict, 2011-7-26. </remarks>

//	~CCommControl(void);

//	/// <summary>	Initialize interface, creates multi-threads. </summary>
//	///
//	/// <remarks>	ZjuNlict, 2011-7-26. </remarks>
//	void Init();

//	/// <summary>	Destroys interface, close threads. </summary>
//	///
//	/// <remarks>	ZjuNlict, 2011-7-26. </remarks>

//	void Destroy();

//	/// <summary>	Request robot information. </summary>
//	/// 返回值表示双向通讯数据的时间序号
//	/// <remarks>	ZjuNlict, 2011-7-26. </remarks>
//	///
//	/// <param name="n">   	The specified vehilce num. </param>
//	/// <param name="info">	[in,out] If non-null, the information. </param>
//	///
//	/// <returns>	. </returns>

//	int RequestSpeedInfo(int n,ROBOTINFO *info);

//	/// <summary>	Creates new robot command. </summary>
//	///
//	/// <remarks>	ZjuNlict, 2011-7-26. </remarks>
//	///
//	/// <param name="n">	  	The specified vehilce num. </param>
//	/// <param name="command">	The corresponding command. </param>

//	void NewRobotCommand(int n, const ROBOTCOMMAND* command);

//	// added by cliffyin [6/11/2012 ZJUNlict]
//	void ClearRobotCmd(int robotNum);

//	/// <summary> The serial port </summary>
//	friend class CSerialPort;

//	/// <summary>	Control thread, main loop thread. </summary>
//	///
//	/// <remarks>	ZjuNlict, 2011-7-26. </remarks>
//	///
//	/// <param name="p">	[in,out] If non-null, the. </param>
//	///
//	/// <returns>	. </returns>

////	friend unsigned __stdcall ControlThread(void *p);
////	friend unsigned __stdcall ReceiveThread(void *p);
//	//friend CThreadCreator::CallBackReturnType THREAD_CALLBACK ControlThread(CThreadCreator::CallBackParamType p);

//private:
//	void CloseAll();
//	void Error();

//	void EncodePacket(int pkgNum);
//	void DecodePacket();
//	int recv_packet_len() const { return _nRecvPacketLen; }

//	bool bStart;                    // 是否用于开始发送
//	bool bCrcRight;					// 双向通讯，用于接收端CRC校验，控制接收数据的有效性

////	HANDLE hThread;
//	unsigned _threadID;

////	HANDLE hEventControlTimer;      // 将速度等指令以一定时间触发并发送出去的event
////	HANDLE hEventControlReceive;    // 收到小车数据的event
////	HANDLE hEventControlCheck;
////	HANDLE hEventRequestInfo;       // 上层向下层索要下车数据的event
////	HANDLE hEventRequestWait;
////	HANDLE hEventNewCommand;        // 等待获取上层速度控球等指令数据，并赋值到CCommControl对象的相应变量中去的event（注意与hEventControlTimer的区别）
////	HANDLE hEventNewCommandWait;
////	HANDLE hEventWatchDog;          // 看门狗事件(超时没有数据更新则复位)

////	UINT uTimerID;                  // 发送指令的时间触发的计时器
////	UINT watchDogID;

////	BYTE TXBuff[24];                // 数据发送的缓冲地址,用于发送x,y速度,转速,控球,踢球等命令
////	BYTE RXBuff[24];                // 数据接收的缓冲地址,用于收到小车相应数据

//    // 下层->上层
//	ROBOTINFO *pTempInfo;           // 上层向下层索要小车状态数据，而下层将数据存放在pTempInfo中
//	int nTempNum;                   // 上层索要数据时的车号
//	int _nRecvPacketLen;

//	// 上层->下层
//	int nTempNumSend;               // 上层向下层写入待发送指令时的车号
//	ROBOTCOMMAND CmdBuff;           // 上层命令传下来的临时空间(将在hEventNewCommand事件中，即WAIT_OBJECT_0+3处，被复制到RobotPool[]相应小车处)

//    std::mutex m_mutex;
//    std::mutex m_mutex2_list[PARAM::Field::MAX_PLAYER];
////	CSerialPort Com;
//	bool bInit;

//    // 存储机器人
//	double RobotCmdLostTime[PARAM::Field::MAX_PLAYER];		// 上层运动指令消失时间
//	CRobotControl RobotPool[PARAM::Field::MAX_PLAYER];		// 存放上层向下层发送指令时的数据区
//	CRobotControl RevRobotPool[PARAM::Field::MAX_PLAYER];	// 存放双向通讯收到数据时,存放的数据区
//	int _updateValue[PARAM::Field::MAX_PLAYER];              // 表示最新双向通讯数据返回的时刻

//	AbstractRadioPackets* pIRadioPacket;		// 解码编码的抽象接口
//};

////unsigned __stdcall ControlThread(void *p);
////unsigned __stdcall ReceiveThread(void *p);
////CThreadCreator::CallBackReturnType THREAD_CALLBACK ControlThread(CThreadCreator::CallBackParamType p);

//#define WM_ROBOTFEEDBACK WM_USER+0x0100

//#endif // ~__COMM_CONTROL_H__
