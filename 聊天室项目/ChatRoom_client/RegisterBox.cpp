// RegisterBox.cpp: 实现文件
//

#include "stdafx.h"
#include "ChatRoom_client.h"
#include "RegisterBox.h"
#include "afxdialogex.h"


// RegisterBox 对话框
IMPLEMENT_DYNAMIC(RegisterBox, CDialogEx)
RegisterBox::RegisterBox(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DIALOG1, pParent)
	, m_MaleorFemale(FALSE)
	, m_login_name(_T(""))
	, m_login_pass(_T(""))
	, m_login_pass_check(_T(""))
{

}

RegisterBox::~RegisterBox()
{
}

void RegisterBox::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Radio(pDX, IDC_RADIO1, m_MaleorFemale);
	DDX_Text(pDX, IDC_EDIT1, m_login_name);
	DDX_Text(pDX, IDC_EDIT4, m_login_pass);

	DDX_Text(pDX, IDC_EDIT3, m_login_pass_check);
}


BEGIN_MESSAGE_MAP(RegisterBox, CDialogEx)
	ON_BN_CLICKED(IDC_BUTTON1, &RegisterBox::Login_Button)
END_MESSAGE_MAP()



void RegisterBox::Login_Button()
{
	// TODO: 在此添加控件通知处理程序代码
	UpdateData(true);

	if (m_login_pass_check != m_login_pass)
	{
		MessageBox(L"密码输入不一致");
	}

	//定义登陆信息结构体
	REMSG registerinfo;
	//将Cstring类对象的用户名转换为chr*类型的user_name,用于打包传递
	//这里本质上CString底层是wachar_t定义的，每个字符是两个字节，所以数据结构中保存用户名和密码的结构体必须使用wchar_t定义
	//因为如果使用char定义就会导致char类型变量只能接收wchar_t字符串的一个字符，
	//此时就导致了在python端中对数据的编码和解码只能使用utf8或utf16
	//且注意GetLength()返回的为字符的个数，一个汉字也算一个字符，而且不包含字符串后面的\0，而memcpy是以复制的字节数为单位的
	//所以需要用(m_login_name.GetLength() + 1) * sizeof(TCHAR)表示需要复制的字符串长度

	//将用户名输入注册信息结构体
	memcpy(registerinfo.user_name, m_login_name.GetBuffer(), (m_login_name.GetLength() + 1) * sizeof(TCHAR));
	//将密码输入注册信息结构体
	memcpy(registerinfo.user_pass, m_login_pass.GetBuffer(), (m_login_pass.GetLength() + 1) * sizeof(TCHAR));
	//用户名长度，用于之后再服务端解包
	registerinfo.namelen = wcslen(registerinfo.user_name);
	//密码长度
	registerinfo.passlen = wcslen(registerinfo.user_pass);

	if (m_MaleorFemale)//当该bool值为1时性别为女
	{
		registerinfo.user_sex = 0;
	}
	else//反之性别为男
	{
		registerinfo.user_sex = 1;
	}
	UpdateData(false);

	//定义需要发送的数据包
	MESSAGECONTENT msg;
	msg.Msg_Type = RegisterMessage;//消息类型为注册消息
	msg.hwnd = this->m_hWnd;//窗口句柄为当前句柄
	msg.dwMsgLen = sizeof(REMSG);
	//这一步将使用一个结构体，对一个联合体进行赋值
	memcpy(&(msg.m_content),&registerinfo,sizeof(REMSG));


	//主窗口名称 IDD_CHATROOM_CLIENT_DIALOG(Dialog)
	//或取主窗口句柄，并给主窗口发送消息
	//HWND pWnd = FindWindowA(NULL, "登陆");
	//if (pWnd == NULL)
	//	MessageBox(L"Error!");

	//将当前窗口句柄以及数据包分别作为wparam和lparam传递给主窗口 注意这里是将数据包的指针传到了主窗口
	::SendMessage(g_LogHandle,WM_REGISTER,(WPARAM)this->m_hWnd,(LPARAM)&msg);

	//这是一种通过将窗口句柄保存为全局变量，向特定窗口传递消息的方法
	//::SendMessage(Dlglog_hWnd, WM_REGISTER, 12, 13);
}