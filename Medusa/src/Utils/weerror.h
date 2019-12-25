#ifndef _WE_ERROR_H_
#define _WE_ERROR_H_

#include <iostream>
class WEError { 
public:
	WEError(char* strFile,int nLine,char* strMsg) : _strFile(strFile),_nLine(nLine),_strMsg(strMsg) { } 
	void print() { std::cerr << _strFile << ':' << _nLine << ':' << _strMsg << std::endl; } // 返回错误信息
private:
	char* _strFile; // 出错的源文件名
	int	  _nLine;	// 出错的行号
	char* _strMsg ; // 保存错误信息
};

#endif // _WE_ERROR_H_
