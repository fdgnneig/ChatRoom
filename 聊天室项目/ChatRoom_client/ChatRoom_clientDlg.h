
// ChatRoom_clientDlg.h: 头文件
//

#pragma once
//#include <afxsock.h>
#include "RegisterBox.h"
#include "CMySocket.h"
#include "data.h"
#include "ChatBox.h"


// CChatRoomclientDlg 对话框
class CChatRoomclientDlg : public CDialogEx
{
// 构造
public:
	CChatRoomclientDlg(CWnd* pParent = nullptr);	// 标准构造函数

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_CHATROOM_CLIENT_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持

// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	//登陆框用户名
	CString m_login_name;
	//登陆框密码
	CString m_login_pass;
	afx_msg void RegisterButton();
	//当socket接收到网络数据包的时候，会将数据包作为参数调用该函数，
	//该函数内部将收到的数据包分发给不同的程序窗口，实现来自服务器的消息的分发
	void OnReceive(wchar_t*szText);
	//用于建立连接的socket
	CMySocket aSocket;

protected:
	afx_msg LRESULT OnRegister(WPARAM wParam, LPARAM lParam);
public:
	afx_msg void LoginButton();
protected:
	afx_msg LRESULT OnChatmessage(WPARAM wParam, LPARAM lParam);
	//用于响应显示当前所有用户的信息
	afx_msg LRESULT OnFinduser(WPARAM wParam, LPARAM lParam);
	//用于响应当前显示历史聊天
	afx_msg LRESULT OnHistorymsg(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnChattoone(WPARAM wParam, LPARAM lParam);

	afx_msg LRESULT OnFriend(WPARAM wParam, LPARAM lParam);
};
