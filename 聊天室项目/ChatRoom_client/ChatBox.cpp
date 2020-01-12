// ChatBox.cpp: 实现文件
//

#include "stdafx.h"
#include "ChatRoom_client.h"
#include "ChatBox.h"
#include "afxdialogex.h"
#include "data.h"
#include "ChatRoom_clientDlg.h"

// ChatBox 对话框

IMPLEMENT_DYNAMIC(ChatBox, CDialogEx)

ChatBox::ChatBox(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DIALOG2, pParent)
	, WriteCstring(_T(""))
	, ShowCstring(_T(""))
	, m_chatto1(_T(""))
	, m_Friend1(_T(""))
	, m_GroupNumber(_T(""))
{
}

ChatBox::~ChatBox()
{
}

void ChatBox::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT2, WriteChatMessage);
	DDX_Control(pDX, IDC_EDIT1, ShowChatMeaage);
	DDX_Text(pDX, IDC_EDIT2, WriteCstring);
	DDX_Text(pDX, IDC_EDIT1, ShowCstring);
	DDX_Control(pDX, IDC_LIST2, m_userlist);
	DDX_Text(pDX, IDC_EDIT4, m_chatto1);
	DDX_Control(pDX, IDC_LIST1, m_HistoryMsg);
	DDX_Text(pDX, IDC_EDIT6, m_Friend1);
	DDX_Control(pDX, IDC_LIST3, m_Show);
	DDX_Text(pDX, IDC_EDIT7, m_GroupNumber);
}


BEGIN_MESSAGE_MAP(ChatBox, CDialogEx)
	ON_BN_CLICKED(IDC_BUTTON1, &ChatBox::SendChatMessage)
	ON_MESSAGE(WM_CHATMESSAGEtoChatBox, &ChatBox::OnChatmessagetochatbox)
	ON_BN_CLICKED(IDC_BUTTON2, &ChatBox::Show_User)
	ON_MESSAGE(WM_FINDUSERtoChatBox, &ChatBox::OnFindusertochatbox)
	ON_BN_CLICKED(IDC_BUTTON3, &ChatBox::ChatToOne)
	ON_BN_CLICKED(IDC_BUTTON4, &ChatBox::GetHistoryMessage)
	ON_MESSAGE(WM_HISTORYMSGtoChatBox, &ChatBox::OnHistorymsgtochatbox)
	ON_MESSAGE(WM_CHATTOONEtoChatBox, &ChatBox::OnChattoonetochatbox)

	ON_BN_CLICKED(IDC_BUTTON5, &ChatBox::MakeFriendsWith)
	ON_BN_CLICKED(IDC_BUTTON6, &ChatBox::ShowFriend)
	ON_MESSAGE(WM_SHOWFRIEND, &ChatBox::OnShowfriend)
END_MESSAGE_MAP()

// ChatBox 消息处理程序
//发送聊天数据
void ChatBox::SendChatMessage()
{
	//if(m_GroupNumber)
	//保存聊天窗口的句柄
	g_ChatHandle = this->m_hWnd;

	CChatRoomclientDlg *pwd = (CChatRoomclientDlg *)AfxGetMainWnd();
	CString name = pwd->m_login_name;
	
	UpdateData(true);
	//将发送该数据的用户名拼接到发送的字符串中
	WriteCstring =  name + L"说:"+WriteCstring;
	
	//正文数据放在buff中
	wchar_t buff[1024];
	memcpy(buff, WriteCstring.GetBuffer(), (WriteCstring.GetLength() + 1) * sizeof(TCHAR));

	//将正文数据加密
	wchar_t buff2[1024];
	for (int i = 0; i < 1024; i++)
	{
		buff2[i] = buff[i] ^ 233;
	}

	MESSAGECONTENT msg;
	msg.Msg_Type = ChatMessage;//消息类型为注册消息
	msg.hwnd = this->m_hWnd;//窗口句柄为当前句柄
	msg.dwMsgLen = (WriteCstring.GetLength()) * sizeof(TCHAR);//正文聊天数据的长度
	//将加密的数据打包
	memcpy(&(msg.m_content),buff2,(WriteCstring.GetLength() + 1) * sizeof(TCHAR));

	//UpdateData(false);

	//将当前窗口句柄以及数据包分别作为wparam和lparam传递给主窗口 注意这里是将数据包的指针传到了主窗口
	::SendMessage(g_LogHandle, WM_CHATMESSAGE, (WPARAM)this->m_hWnd, (LPARAM)&msg);
}

//该函数用于接收从主窗口发送过来的聊天消息
afx_msg LRESULT ChatBox::OnChatmessagetochatbox(WPARAM wParam, LPARAM lParam)
{
	static CString before= L"";//用于保存上一次输出的内容

	//保存从主窗口返回的信息
	MESSAGECONTENT receive;
	memcpy(&receive, (void*)lParam, sizeof(MESSAGECONTENT));
	
	//获取到正文数据包
	wchar_t note[1024];
	memcpy(note, &receive.m_content, sizeof(wchar_t) * 1024);


	//解密正文数据包
	wchar_t note2[1024];
	for (int i = 0; i < 1024; i++)
	{
		note2[i] = note[i] ^ 233;
	}

	UpdateData(true);
	//显示解密后的内容
	ShowCstring = note2;
	ShowCstring = before+ShowCstring + L"\r\n";
	UpdateData(false);
	
	
	before = ShowCstring;

	//清空输入框
	WriteChatMessage.SetSel(0, -1);
	WriteChatMessage.ReplaceSel(_T(""));
	return 0;
}

void ChatBox::Show_User()
{
	g_ChatHandle = this->m_hWnd;
	// TODO: 在此添加控件通知处理程序代码
	m_userlist.InsertColumn(0, L"在线用户", 0, 80);
	m_userlist.DeleteAllItems();
	UpdateData(false);

	//或取主窗口句柄，并给主窗口发送消息
	//HWND pWnd = FindWindowA(NULL, "登陆");
	//if (pWnd == NULL)
	//	MessageBox(L"Error!");
	//将当前窗口句柄以及数据包分别作为wparam和lparam传递给主窗口 注意这里是将数据包的指针传到了主窗口

	MESSAGECONTENT msg;
	msg.Msg_Type = FindUser;

	::SendMessage(g_LogHandle, WM_FINDUSER, (WPARAM)this->m_hWnd, (LPARAM)&msg);
}


afx_msg LRESULT ChatBox::OnFindusertochatbox(WPARAM wParam, LPARAM lParam)
{
	//保存从主窗口返回的信息
	MESSAGECONTENT receive;
	memcpy(&receive, (void*)lParam, sizeof(MESSAGECONTENT));

	//获取到正文数据包
	wchar_t note[1024];
	memcpy(note, &receive.m_content, sizeof(wchar_t) * 1024);
	m_userlist.InsertItem(0, note);
	
	return 0;
}

//与人私聊
void ChatBox::ChatToOne()
{
	g_ChatHandle = this->m_hWnd;

	// TODO: 在此添加控件通知处理程序代码
	CChatRoomclientDlg *pwd = (CChatRoomclientDlg *)AfxGetMainWnd();
	CString name2 = pwd->m_login_name;

	UpdateData(true);
	WriteCstring = name2 + L"说:" + WriteCstring;

	//正文数据放在buff中
	wchar_t buff[1024];
	memcpy(buff, WriteCstring.GetBuffer(), (WriteCstring.GetLength() + 1) * sizeof(TCHAR));

	//将正文数据加密
	wchar_t buff2[1024];
	for (int i = 0; i < 1024; i++)
	{
		buff2[i] = buff[i] ^ 233;
	}

	//将私聊对手的用户名保存起来
	wchar_t name[10];
	memcpy(name, m_chatto1.GetBuffer(), (m_chatto1.GetLength() + 1) * sizeof(wchar_t));
	//memcpy(name, m_chatto1.GetBuffer(), (m_chatto1.GetLength()) * sizeof(TCHAR));

	TOONE msg;
	msg.Msg_Type = ChatOne;
	msg.dwMsgLen = (WriteCstring.GetLength()) * sizeof(TCHAR);//私聊聊天数据的长度
	memcpy(&(msg.m_content), buff2, (WriteCstring.GetLength() + 1) * sizeof(TCHAR));//将私聊信息放入结构体
	memcpy(&(msg.anotherPerson), name, (m_chatto1.GetLength() + 1) * sizeof(wchar_t));//将私聊对手的信息保存起来
	//memcpy(&(msg.anotherPerson), name, 10 * sizeof(wchar_t));//将私聊对手的信息保存起来

	//将当前窗口句柄以及数据包分别作为wparam和lparam传递给主窗口 注意这里是将数据包的指针传到了主窗口

	::SendMessage(g_LogHandle,WM_CHATTOONE, (WPARAM)this->m_hWnd, (LPARAM)&msg);
}

void ChatBox::GetHistoryMessage()
{
	// TODO: 在此添加控件通知处理程序代码
		// TODO: 在此添加控件通知处理程序代码
	g_ChatHandle = this->m_hWnd;

	m_HistoryMsg.InsertColumn(0, L"历史消息", 0, 300);
	m_HistoryMsg.DeleteAllItems();
	UpdateData(false);

	//获取主窗口句柄，并给主窗口发送消息
	//HWND pWnd = FindWindowA(NULL, "登陆");
	//if (pWnd == NULL)
	//	MessageBox(L"Error!");
	//将当前窗口句柄以及数据包分别作为wparam和lparam传递给主窗口 注意这里是将数据包的指针传到了主窗口

	MESSAGECONTENT msg;
	msg.Msg_Type = GetHistoryMsg;

	::SendMessage(g_LogHandle, WM_HISTORYMSG, (WPARAM)this->m_hWnd, (LPARAM)&msg);
}

//用于显示历史记录
afx_msg LRESULT ChatBox::OnHistorymsgtochatbox(WPARAM wParam, LPARAM lParam)
{
	//保存从主窗口返回的信息
	MESSAGECONTENT receive;
	memcpy(&receive, (void*)lParam, sizeof(MESSAGECONTENT));

	//获取到正文数据包

	wchar_t note[1024];
	memcpy(note, &receive.m_content, sizeof(wchar_t) * 1024);

	//将历史消息解密
	for (int i = 0; i < 1024; i++)
	{
		note[i] = note[i] ^ 233;
	}

	m_HistoryMsg.InsertItem(0, note);
	//UpdateData(false);
	return 0;
}

//用于显示私聊消息
afx_msg LRESULT ChatBox::OnChattoonetochatbox(WPARAM wParam, LPARAM lParam)
{
	//保存从主窗口返回的信息
	TOONE receive;
	memcpy(&receive, (void*)lParam, sizeof(TOONE));

	//获取到正文数据包
	wchar_t note[1024];
	memcpy(note, &receive.m_content, sizeof(wchar_t) * 1024);

	//解密正文数据包
	wchar_t note2[1024];
	for (int i = 0; i < 1024; i++)
	{
		note2[i] = note[i] ^ 233;
	}
	//显示解密后的内容
	ShowCstring = note2;
	ShowCstring = ShowCstring + L"\n";

	UpdateData(false);

	//清空输入框
	WriteChatMessage.SetSel(0, -1);
	WriteChatMessage.ReplaceSel(_T(""));
	return 0;
}

void ChatBox::MakeFriendsWith()
{
	//要被加的用户
	//CString m_Friend1;
	//获得当前窗口用户名
	CChatRoomclientDlg *pwd = (CChatRoomclientDlg *)AfxGetMainWnd();
	CString name1 = pwd->m_login_name;
	UpdateData(true);

	/////////////////////////////////////////////////////////////////////////////////////
	FRIEND friendinfo;
	//将用户名输入注册信息结构体
	memcpy(friendinfo.user_name1, name1.GetBuffer(), (name1.GetLength() + 1) * sizeof(TCHAR));
	//将密码输入注册信息结构体
	memcpy(friendinfo.user_name2, m_Friend1.GetBuffer(), (m_Friend1.GetLength() + 1) * sizeof(TCHAR));
	//用户名长度，用于之后再服务端解包
	friendinfo.namelen1 = wcslen(friendinfo.user_name1);
	friendinfo.namelen2 = wcslen(friendinfo.user_name2);

	//定义需要发送的数据包
	MESSAGECONTENT msg;
	msg.Msg_Type = Friend;//消息类型为注册消息
	msg.hwnd = this->m_hWnd;//窗口句柄为当前句柄
	msg.dwMsgLen = sizeof(FRIEND);
	//这一步将使用一个结构体，对一个联合体进行赋值
	memcpy(&(msg.m_content), &friendinfo, sizeof(FRIEND));

	::SendMessage(g_LogHandle, WM_FRIEND, (WPARAM)this->m_hWnd, (LPARAM)&msg);

}

//响应显示账号好友的按钮
void ChatBox::ShowFriend()
{
	// TODO: 在此添加控件通知处理程序代码

	g_ChatHandle = this->m_hWnd;
	
	m_Show.InsertColumn(0, L"好友信息", 0, 300);
	m_Show.DeleteAllItems();
	//UpdateData(false);

	MESSAGECONTENT msg;
	msg.Msg_Type = ShowFriendinfo;


	//使用历史消息的接口向服务器发送消息
	::SendMessage(g_LogHandle, WM_HISTORYMSG, (WPARAM)this->m_hWnd, (LPARAM)&msg);
}

//用于显示传回的好友信息
afx_msg LRESULT ChatBox::OnShowfriend(WPARAM wParam, LPARAM lParam)
{
	//CListCtrl m_Show;
	//保存从主窗口返回的信息

	MESSAGECONTENT receive;
	memcpy(&receive, (void*)lParam, sizeof(MESSAGECONTENT));

	//获取到正文数据包
	wchar_t note[1024];

	memcpy(note, &receive.m_content, sizeof(wchar_t) * 1024);

	m_Show.InsertItem(0, note);

	return 0;
}
