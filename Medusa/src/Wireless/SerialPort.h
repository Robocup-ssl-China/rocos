/// FileName : 		SeriaPort.h
/// 				declaration file
/// Description :	serial handling
///	Keywords :		com send
/// Organization : 	ZJUNlict@Small Size League
/// Author : 		cliffyin
/// E-mail : 		cliffyin@zju.edu.cn
///					cliffyin007@gmail.com
/// Create Date : 	2011-07-25
/// Modified Date :	2011-07-25 
/// History :

#ifndef __SERIAL_PORT_H__
#define __SERIAL_PORT_H__
//#include "atltime.h"
//#include "atlstr.h"
//#include <Windows.h>

#define FC_DTRDSR       0x01
#define FC_RTSCTS       0x02
#define FC_XONXOFF      0x04

// ascii definitions
#define ASCII_BEL       0x07
#define ASCII_BS        0x08
#define ASCII_LF        0x0A
#define ASCII_CR        0x0D
#define ASCII_XON       0x11
#define ASCII_XOFF      0x13

#define BUFLEN 24
#define PACKSIZE 9

class CCommControl;

/// <summary>	Serial port.  </summary>
///
/// <remarks>	ZjuNlict, 2011-7-26. </remarks>

class CSerialPort
{
public:

	/// <summary>	Default constructor. </summary>
	///
	/// <remarks>	ZjuNlict, 2011-7-26. </remarks>

	CSerialPort(void);

	/// <summary>	Finaliser. </summary>
	///
	/// <remarks>	Zju Nlict, 2011-7-26. </remarks>

	~CSerialPort(void);

	/// <summary>	Opens a port. </summary>
	///
	/// <remarks>	Zju Nlict, 2011-7-26. </remarks>
	///
	/// <param name="port">	   	The port num. </param>
	/// <param name="rate">	   	The baudrate. </param>
	/// <param name="bytesize">	The bytesize. </param>
	/// <param name="stop">	   	The stop bit. </param>
	/// <param name="parity">  	The parity flag. </param>
	///
	/// <returns>	true if it succeeds, false if it fails. </returns>
	bool OpenPort(int port,DWORD rate,BYTE bytesize,BYTE stop,BYTE parity);

	/// <summary>	Closes the port. </summary>
	///
	/// <remarks>	Zju Nlict, 2011-7-26. </remarks>
	///
	/// <returns>	true if it succeeds, false if it fails. </returns>

	bool ClosePort();

	/// <summary>	Writes a block. </summary>
	///
	/// <remarks>	Zju Nlict, 2011-7-26. </remarks>
	///
	/// <param name="lpByte">		 	[in,out] If non-null, the pointer to a byte. </param>
	/// <param name="dwBytesToWrite">	The dw bytes to write. </param>
	///
	/// <returns>	true if it succeeds, false if it fails. </returns>

	bool WriteBlock( BYTE *lpByte , DWORD dwBytesToWrite);

	/// <summary>	Serial port watch. </summary>
	///
	/// <remarks>	Zju Nlict, 2011-7-26. </remarks>
	///
	/// <param name="pVoid">	[in,out] If non-null, the void. </param>
	///
	/// <returns>	. </returns>

	friend unsigned __stdcall SerialPortWatch(void *pVoid);
	//friend CThreadCreator::CallBackReturnType THREAD_CALLBACK SerialPortWatch(CThreadCreator::CallBackParamType pVoid);
	friend class CCommControl;

private:

	void WaitEventProcess(DWORD dwEvtMask);
	void GetBYTE(BYTE buf);

	bool bInited;
	DCB dcb;
	COMMTIMEOUTS  CommTimeOuts ;
	OVERLAPPED ovRead;
	OVERLAPPED ovWrite;
	OVERLAPPED ovWait;

	HANDLE hComm;

	unsigned _threadID;
	HANDLE hThread;
	HANDLE hThreadStop;

	bool bReceiveFlag;
	BYTE ReceiveBuff[BUFLEN];
	int ReceiveBuffOffset;

	CCommControl *pControl;
};

unsigned __stdcall SerialPortWatch(void *pVoid);
//CThreadCreator::CallBackReturnType THREAD_CALLBACK SerialPortWatch(CThreadCreator::CallBackParamType pVoid);

#endif // ~__SERIAL_PORT_H__
