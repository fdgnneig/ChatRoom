
// ChatRoom_clientDlg.cpp: 实现文件
//

#include "stdafx.h"
#include "ChatRoom_client.h"
#include "ChatRoom_clientDlg.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif

bool _trace(TCHAR *format, ...) //变参函数
{
	TCHAR buffer[1000];
	va_list argptr;
	va_start(argptr, format);
	//将格式化信息写入指定的缓冲区
	wvsprintf(buffer, format, argptr);
	va_end(argptr);
	//将缓冲区信息输出
	OutputDebugString(buffer);
	return true;
}

// 用于应用程序“关于”菜单项的 CAboutDlg 对话框
class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
protected:
	DECLARE_MESSAGE_MAP()
public:
//	afx_msg void SendChatMessage();
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)

END_MESSAGE_MAP()


// CChatRoomclientDlg 对话框
CChatRoomclientDlg::CChatRoomclientDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_CHATROOM_CLIENT_DIALOG, pParent)
	, m_login_name(_T(""))
	, m_login_pass(_T(""))
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}


void CChatRoomclientDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT2, m_login_name);
	DDX_Text(pDX, IDC_EDIT1, m_login_pass);
}

BEGIN_MESSAGE_MAP(CChatRoomclientDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON1, &CChatRoomclientDlg::RegisterButton)
	ON_MESSAGE(WM_REGISTER, &CChatRoomclientDlg::OnRegister)
	ON_BN_CLICKED(IDC_BUTTON2, &CChatRoomclientDlg::LoginButton)
	ON_MESSAGE(WM_CHATMESSAGE, &CChatRoomclientDlg::OnChatmessage)

	ON_MESSAGE(WM_FINDUSER, &CChatRoomclientDlg::OnFinduser)
	ON_MESSAGE(WM_HISTORYMSG, &CChatRoomclientDlg::OnHistorymsg)
	ON_MESSAGE(WM_CHATTOONE, &CChatRoomclientDlg::OnChattoone)
	ON_MESSAGE(WM_FRIEND, &CChatRoomclientDlg::OnFriend)
END_MESSAGE_MAP()


// CChatRoomclientDlg 消息处理程序

BOOL CChatRoomclientDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	// 将“关于...”菜单项添加到系统菜单中。
	// IDM_ABOUTBOX 必须在系统命令范围内。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != nullptr)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}
	// 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码
	//这里要将socket的派生类初始化，并于服务器进行连接

	aSocket.InitSocket();

	//将登陆窗口保存
	g_LogHandle = this->m_hWnd;

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CChatRoomclientDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。  对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CChatRoomclientDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}


//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CChatRoomclientDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

//注册界面
void CChatRoomclientDlg::RegisterButton()
{
	// TODO: 在此添加控件通知处理程序代码
    //将句柄作为全局变量保存，而可以实现直接向某个窗口发消息的功能，暂时不用
	//Dlglog_hWnd=this->m_hWnd;
	RegisterBox dlg;
	INT_PTR nResponse = dlg.DoModal();
}	

//该函数用于将从服务器接收到的消息转发给程序其他函数
void CChatRoomclientDlg::OnReceive(wchar_t*szTex)
{
	//这里需要对从服务器接收到的数据进行判断处理

	MESSAGECONTENT receive;
	memcpy(&receive,szTex,sizeof(MESSAGECONTENT));
	
	wchar_t buffer[1024];
	memcpy(buffer, &receive.m_content, sizeof(buffer));

	switch (receive.Msg_Type)
	{
	case RegisterMessage:
	case Friend:
		MessageBox(buffer, 0);
		break;
	case LoginMessage:
	{	//如果登陆成功
		if (receive.dwMsgLen == 1)
		{
			ShowWindow(SW_HIDE);
			MessageBox(buffer, 0);
			//创建窗口
			ChatBox dlg;
			INT_PTR nResponse = dlg.DoModal();	

			PostQuitMessage(0);
		}
		else//如果登陆失败
		{
			MessageBox(buffer, 0);
		}
		break;
	}
	case ChatMessage:
	{
		////将消息分发到聊天窗口进行显示
		//HWND pWnd = FindWindowA(NULL, "聊天");
		//if (pWnd == NULL)
		//	MessageBox(L"Error!");
		////将主窗口句柄和接收到的消息结构体的指针传给聊天窗口
		//::SendMessage(pWnd, WM_CHATMESSAGEtoChatBox, (WPARAM)this->m_hWnd, (LPARAM)&receive);
		::SendMessage(g_ChatHandle, WM_CHATMESSAGEtoChatBox, (WPARAM)this->m_hWnd, (LPARAM)&receive);

		break;
	}
	case FindUser:
	{
		////将消息分发到聊天窗口进行显示
		//HWND pWnd = FindWindowA(NULL, "聊天");
		//if (pWnd == NULL)
		//	MessageBox(L"Error!");
		////将主窗口句柄和接收到的消息结构体的指针传给聊天窗口
		::SendMessage(g_ChatHandle, WM_FINDUSERtoChatBox, (WPARAM)this->m_hWnd, (LPARAM)&receive);
		break;
	}
	case GetHistoryMsg:
	{
		::SendMessage(g_ChatHandle, WM_HISTORYMSGtoChatBox, (WPARAM)this->m_hWnd, (LPARAM)&receive);
		break;
	}
	case ChatOne:
	{
		::SendMessage(g_ChatHandle, WM_CHATTOONEtoChatBox, (WPARAM)this->m_hWnd, (LPARAM)&receive);
		break;
	}
	case ShowFriendinfo:
	{
		::SendMessage(g_ChatHandle, WM_SHOWFRIEND, (WPARAM)this->m_hWnd, (LPARAM)&receive);
		break;
	}

	}

	OutputDebugString(_T("lalalal"));
}

//该函数用于响应注册窗口发送来的注册消息，主窗口需要将相关消息发送给服务端进行注册
afx_msg LRESULT CChatRoomclientDlg::OnRegister(WPARAM wParam, LPARAM lParam)
{
	//wparan为消息来源窗口地址 ，lparam为消息包的指针
	//验证数据包是否传递成功
	//MESSAGECONTENT msg;
	//memcpy(&(msg), (MESSAGECONTENT*)lParam, sizeof(MESSAGECONTENT));
	
	//使用成员变量socket将注册数据包发送出去
	aSocket.Send((void*)lParam, sizeof(MESSAGECONTENT));
	// aSocket.OnReceive();
	return 0;
}


void CChatRoomclientDlg::LoginButton()
{
	// TODO: 在此添加控件通知处理程序代码
	UpdateData(true);

	LOMSG logininfo;

	//将用户名输入注册信息结构体
	memcpy(logininfo.user_name, m_login_name.GetBuffer(), (m_login_name.GetLength() + 1) * sizeof(TCHAR));
	//将密码输入注册信息结构体
	memcpy(logininfo.user_pass, m_login_pass.GetBuffer(), (m_login_pass.GetLength() + 1) * sizeof(TCHAR));
	//用户名长度，用于之后再服务端解包
	logininfo.namelen = wcslen(logininfo.user_name);
	//密码长度
	logininfo.passlen = wcslen(logininfo.user_pass);

	UpdateData(false);

	MESSAGECONTENT msg;
	msg.Msg_Type = LoginMessage;//消息类型为注册消息
	msg.hwnd = this->m_hWnd;//窗口句柄为当前句柄
	msg.dwMsgLen = sizeof(LOMSG);
	//这一步将使用一个结构体，对一个联合体进行赋值
	memcpy(&(msg.m_content), &logininfo, sizeof(LOMSG));
////////////////////////////////////////////////////////////////////////////////////////
	//给服务端发包

	aSocket.Send((void*)&msg, sizeof(MESSAGECONTENT));

	// aSocket.OnReceive();这里不需要主动调用派生自CSocket的对象的接收函数，该类Socket函数会自动调用


	printf("s");
}

//该函数用于转发聊天室消息
afx_msg LRESULT CChatRoomclientDlg::OnChatmessage(WPARAM wParam, LPARAM lParam)
{
	aSocket.Send((void*)lParam, sizeof(MESSAGECONTENT));
	return 0;
}

afx_msg LRESULT CChatRoomclientDlg::OnFinduser(WPARAM wParam, LPARAM lParam)
{
	aSocket.Send((void*)lParam, sizeof(MESSAGECONTENT));
	return 0;
}

//响应聊天记录
afx_msg LRESULT CChatRoomclientDlg::OnHistorymsg(WPARAM wParam, LPARAM lParam)
{
	aSocket.Send((void*)lParam, sizeof(MESSAGECONTENT));
	return 0;
}


afx_msg LRESULT CChatRoomclientDlg::OnChattoone(WPARAM wParam, LPARAM lParam)
{
	
	aSocket.Send((void*)lParam, sizeof(TOONE));
	return 0;
}

//发送加好友消息
afx_msg LRESULT CChatRoomclientDlg::OnFriend(WPARAM wParam, LPARAM lParam)
{

	aSocket.Send((void*)lParam, sizeof(MESSAGECONTENT));
	return 0;
}
