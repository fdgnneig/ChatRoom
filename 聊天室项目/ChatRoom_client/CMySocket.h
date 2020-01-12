//消息收发类，主要用于在主窗口对服务端进行网络连接，本程序其他窗口与服务端的数据交互通过主窗口交互

#pragma once
//mfc中如果要使用socket需要添加该头文件
#include<afxsock.h>

#include "data.h"

class CMySocket :public CSocket
{
public:
	CMySocket();
	virtual ~CMySocket();
	//virtual void OnReceive() override;
	void InitSocket();
	virtual void OnReceive(int nErrorCode);
};

