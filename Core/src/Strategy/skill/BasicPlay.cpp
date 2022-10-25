/************************************************************************/
/* Copyright (c) CSC-RL, Zhejiang University							*/
/* Team:			SSL-ZJUNlict										*/
/* HomePage: http://www.nlict.zju.edu.cn/ssl/WelcomePage.html			*/
/************************************************************************/
/* File:	BasicPlay.cpp												*/
/* Brief:	C++ Implementation: Base class for play						*/
/* Func:	Provide an base class for stated play						*/
/* Author:	cliffyin, 2012, 08											*/
/* Refer:	NONE														*/
/* E-mail:	cliffyin007@gmail.com										*/
/************************************************************************/

#include "BasicPlay.h"
#include <TaskMediator.h>

void CBasicPlay::setTask(const int num, CPlayerTask* pTask, const int priority)
{
	TaskMediator::Instance()->setPlayerTask(num, pTask, priority);
}