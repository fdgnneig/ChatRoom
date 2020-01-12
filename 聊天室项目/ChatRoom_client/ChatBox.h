#pragma once


// ChatBox 对话框

class ChatBox : public CDialogEx
{
	DECLARE_DYNAMIC(ChatBox)

public:
	ChatBox(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~ChatBox();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG2 };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void SendChatMessage();
	CEdit WriteChatMessage;
	CEdit ShowChatMeaage;
	CString WriteCstring;
	CString ShowCstring;

protected:
	afx_msg LRESULT OnChatmessagetochatbox(WPARAM wParam, LPARAM lParam);
public:
	CListCtrl m_userlist;
	afx_msg void Show_User();
protected:
	afx_msg LRESULT OnFindusertochatbox(WPARAM wParam, LPARAM lParam);
public:
	//指定私聊对手的变量
	CString m_chatto1;
	//用于发送私聊消息的按钮
	afx_msg void ChatToOne();
	//用于显示聊天记录的list
	CListCtrl m_HistoryMsg;
	//用于得到聊天记录的按钮
	afx_msg void GetHistoryMessage();
protected:
	afx_msg LRESULT OnHistorymsgtochatbox(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnChattoonetochatbox(WPARAM wParam, LPARAM lParam);
public:
	//加好友的两个用户名
	CString m_Friend1;
	afx_msg void MakeFriendsWith();
	//用于显示好友列表的控件
	CListCtrl m_Show;
	afx_msg void ShowFriend();
protected:
	afx_msg LRESULT OnShowfriend(WPARAM wParam, LPARAM lParam);
public:
	//用于标识进行群聊的群号
	CString m_GroupNumber;
};

