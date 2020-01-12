// ChatRoom_client.h: PROJECT_NAME 应用程序的主头文件
//
#pragma once
#ifndef __AFXWIN_H__
	#error "在包含此文件之前包含“stdafx.h”以生成 PCH 文件"
#endif

#include "resource.h"		// 主符号

// CChatRoomclientApp:
// 有关此类的实现，请参阅 ChatRoom_client.cpp
//

class CChatRoomclientApp : public CWinApp
{
public:
	CChatRoomclientApp();

// 重写
public:
	virtual BOOL InitInstance();

// 实现

	DECLARE_MESSAGE_MAP()
};

//声明全局对象，该对象的类是自己定义的
extern CChatRoomclientApp theApp;
