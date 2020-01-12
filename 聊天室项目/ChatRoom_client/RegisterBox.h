#pragma once

#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <winsock2.h>
#include <windows.h>
#include <stdio.h>
#pragma comment(lib,"ws2_32.lib")
#include "ChatRoom_clientDlg.h"
#include "data.h"

// RegisterBox 对话框

class RegisterBox : public  CDialogEx
{
	DECLARE_DYNAMIC(RegisterBox)

public:
	RegisterBox(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~RegisterBox();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG1 };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:

	//注册属性为男或女
	BOOL m_MaleorFemale;
	//注册用户名
	CString m_login_name;
	//注册密码
	CString m_login_pass;
	//注册密码检验
	CString m_login_pass_check;

	afx_msg void Login_Button();
};
